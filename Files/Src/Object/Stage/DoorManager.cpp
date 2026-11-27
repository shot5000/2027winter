#include <fstream>
#include <map>
#include <cmath>
#include "../../External/json/json.hpp"
#include "../../Utility/AsoUtility.h"
#include "../Collision/BodyCollision.h"
#include "../Noise/NoiseManager.h"
#include "DoorManager.h"

const char* const DoorManager::SLIDE_PREFIX = "Door_Slide_";
const char* const DoorManager::HINGE_PREFIX = "Door_Hinge_";
const char* const DoorManager::LOCKER_PREFIX = "Locker_Door_";
const char* const DoorManager::JSON_PATH = "Data/Json/DoorData.json";

DoorManager::DoorManager(void)
{
	modelId_ = -1;
}

DoorManager::~DoorManager(void)
{
}

void DoorManager::Init(int stageModelId)
{
	modelId_ = stageModelId;
	doors_.clear();
	LoadFrames();
	UpdateClosedFrames();
}

void DoorManager::LoadFrames(void)
{
	// 開く向きを読む(なければ全部 +1)
	std::map<std::string, int> signs_;
	std::ifstream file_(JSON_PATH);
	if (file_.is_open())
	{
		nlohmann::json json_ = nlohmann::json::parse(file_, nullptr, false);
		if (!json_.is_discarded())
		{
			for (auto it_ = json_.begin(); it_ != json_.end(); ++it_)
			{
				signs_[it_.key()] = it_.value().get<int>();
			}
		}
	}

	// 名前が扉のものを全部集める
	int frameNum_ = MV1GetFrameNum(modelId_);
	for (int i_ = 0; i_ < frameNum_; i_++)
	{
		std::string name_ = MV1GetFrameName(modelId_, i_);

		TYPE type_ = TYPE::HIDE;
		if (name_.rfind(SLIDE_PREFIX, 0) == 0)
		{
			type_ = TYPE::SLIDE;
		}
		else if (name_.rfind(HINGE_PREFIX, 0) != 0 &&
			name_.rfind(LOCKER_PREFIX, 0) != 0)
		{
			continue;
		}

		DOOR door_;
		door_.frame_ = i_;
		door_.type_ = type_;
		door_.sign_ = 1.0f;
		if (signs_.count(name_) > 0)
		{
			door_.sign_ = static_cast<float>(signs_[name_]);
		}
		door_.rate_ = 0.0f;
		door_.isOpenTarget_ = false;
		door_.base_ = MV1GetFrameBaseLocalMatrix(modelId_, i_);
		door_.pos_ = MV1GetFramePosition(modelId_, i_);

		// 扉ごとに当たり判定を用意する
		MV1SetupCollInfo(modelId_, i_, COLL_DIV, COLL_DIV, COLL_DIV);

		doors_.push_back(door_);
	}
}

void DoorManager::Update(float deltaTime)
{
	for (auto& door_ : doors_)
	{
		float target_ = door_.isOpenTarget_ ? 1.0f : 0.0f;
		if (door_.rate_ == target_)
		{
			continue;
		}

		// 目標に向かって動かす
		float step_ = OPEN_SPEED * deltaTime;
		if (door_.rate_ < target_)
		{
			door_.rate_ += step_;
			if (door_.rate_ > target_) { door_.rate_ = target_; }
		}
		else
		{
			door_.rate_ -= step_;
			if (door_.rate_ < target_) { door_.rate_ = target_; }
		}

		ApplyPose(door_);
	}

	UpdateClosedFrames();
}

void DoorManager::ApplyPose(const DOOR& door)
{
	MATRIX mat_ = door.base_;

	if (door.type_ == TYPE::SLIDE)
	{
		// 親の空間で横にずらす
		float move_ = door.sign_ * SLIDE_SIGN * SLIDE_DIST * door.rate_;
		mat_ = MMult(door.base_, MGetTranslate(VGet(move_, 0.0f, 0.0f)));
	}
	else
	{
		// 蝶番(フレームの原点)を中心に、縦軸で回す
		float angle_ = AsoUtility::Deg2RadF(HINGE_OPEN_DEG)
			* door.sign_ * HINGE_SIGN * door.rate_;
		VECTOR hinge_ = VGet(
			door.base_.m[3][0], door.base_.m[3][1], door.base_.m[3][2]);
		MATRIX rot_ = MMult(
			MGetTranslate(VScale(hinge_, -1.0f)),
			MMult(MGetRotY(angle_), MGetTranslate(hinge_)));
		mat_ = MMult(door.base_, rot_);
	}

	MV1SetFrameUserLocalMatrix(modelId_, door.frame_, mat_);
	MV1RefreshCollInfo(modelId_, door.frame_);
}

bool DoorManager::IsMoving(const DOOR& door) const
{
	// 目標の位置に着いていなければ、動いている途中
	float target_ = door.isOpenTarget_ ? 1.0f : 0.0f;
	return door.rate_ != target_;
}

void DoorManager::UpdateClosedFrames(void)
{
	// 完全に閉じている扉だけが壁になる
	std::vector<int> closed_;
	for (const auto& door_ : doors_)
	{
		if (door_.rate_ <= 0.0f)
		{
			closed_.push_back(door_.frame_);
		}
	}
	BodyCollision::SetDoorFrames(closed_);
}

void DoorManager::Release(void)
{
	for (const auto& door_ : doors_)
	{
		MV1ResetFrameUserLocalMatrix(modelId_, door_.frame_);
	}
	doors_.clear();
	BodyCollision::SetDoorFrames({});
}

int DoorManager::FindAimed(const VECTOR& eyePos, const VECTOR& dir) const
{
	VECTOR end_ = VAdd(eyePos, VScale(dir, AIM_REACH));

	// 壁が手前にあれば、その向こうの扉は選ばない
	float wallDist_ = AIM_REACH + 1.0f;
	for (int frame_ : BodyCollision::GetStageFrames())
	{
		MV1_COLL_RESULT_POLY hit_ = MV1CollCheck_Line(
			BodyCollision::GetStageModel(), frame_, eyePos, end_);
		if (hit_.HitFlag == 1)
		{
			float dist_ = VSize(VSub(hit_.HitPosition, eyePos));
			if (dist_ < wallDist_) { wallDist_ = dist_; }
		}
	}

	int found_ = -1;
	float nearDist_ = wallDist_;
	for (size_t i_ = 0; i_ < doors_.size(); i_++)
	{
		MV1_COLL_RESULT_POLY hit_ = MV1CollCheck_Line(
			modelId_, doors_[i_].frame_, eyePos, end_);
		if (hit_.HitFlag != 1)
		{
			continue;
		}
		float dist_ = VSize(VSub(hit_.HitPosition, eyePos));
		if (dist_ < nearDist_)
		{
			nearDist_ = dist_;
			found_ = static_cast<int>(i_);
		}
	}
	return found_;
}

void DoorManager::Toggle(int index)
{
	if (index < 0 || index >= static_cast<int>(doors_.size()))
	{
		return;
	}

	// 動いている途中は、操作を受け付けない
	if (IsMoving(doors_[index]))
	{
		return;
	}

	doors_[index].isOpenTarget_ = !doors_[index].isOpenTarget_;

	// 扉の音(敵が聞きつける)
	NoiseManager::Emit(doors_[index].pos_, NoiseManager::DOOR_RADIUS);
}

bool DoorManager::IsClosed(int index) const
{
	return doors_[index].rate_ <= 0.0f && !doors_[index].isOpenTarget_;
}

bool DoorManager::IsHidden(const BodyCollision& body, const VECTOR& pos) const
{
	// カプセルの真ん中から、水平の四方向へ線を引く
	VECTOR center_ = VScale(
		VAdd(body.GetCapsuleTop(pos), body.GetCapsuleDown(pos)), 0.5f);
	const VECTOR DIRS[] = {
		VGet(1.0f, 0.0f, 0.0f), VGet(-1.0f, 0.0f, 0.0f),
		VGet(0.0f, 0.0f, 1.0f), VGet(0.0f, 0.0f, -1.0f)
	};

	bool isHideDoorFound_ = false;
	for (const VECTOR& dir_ : DIRS)
	{
		VECTOR end_ = VAdd(center_, VScale(dir_, BodyCollision::ENCLOSED_RAY_LEN));

		// 壁(ステージ)にいちばん近く当たる距離
		float wallDist_ = BodyCollision::ENCLOSED_RAY_LEN + 1.0f;
		for (int frame_ : BodyCollision::GetStageFrames())
		{
			MV1_COLL_RESULT_POLY hit_ = MV1CollCheck_Line(
				BodyCollision::GetStageModel(), frame_, center_, end_);
			if (hit_.HitFlag == 1)
			{
				float dist_ = VSize(VSub(hit_.HitPosition, center_));
				if (dist_ < wallDist_) { wallDist_ = dist_; }
			}
		}

		// 閉じている扉にいちばん近く当たる距離(個室・ロッカーの扉かも覚えておく)
		float doorDist_ = BodyCollision::ENCLOSED_RAY_LEN + 1.0f;
		bool isNearestHide_ = false;
		for (const auto& door_ : doors_)
		{
			if (door_.isOpenTarget_ || door_.rate_ > 0.0f)
			{
				continue;
			}
			MV1_COLL_RESULT_POLY hit_ = MV1CollCheck_Line(
				modelId_, door_.frame_, center_, end_);
			if (hit_.HitFlag != 1)
			{
				continue;
			}
			float dist_ = VSize(VSub(hit_.HitPosition, center_));
			if (dist_ < doorDist_)
			{
				doorDist_ = dist_;
				isNearestHide_ = (door_.type_ == TYPE::HIDE);
			}
		}

		// どの方向も、何かに囲まれていなければ、隠れていない
		float nearDist_ = (wallDist_ < doorDist_) ? wallDist_ : doorDist_;
		if (nearDist_ > BodyCollision::ENCLOSED_RAY_LEN)
		{
			return false;
		}

		// 最初に当たったものが、閉じた個室・ロッカーの扉
		if (doorDist_ <= wallDist_ && isNearestHide_)
		{
			isHideDoorFound_ = true;
		}
	}
	return isHideDoorFound_;
}

void DoorManager::EnemyOpen(
	const VECTOR& enemyPos, bool canOpenHide, const VECTOR& playerPos)
{
	float enemyToPlayer_ = HorizontalDist(enemyPos, playerPos);

	for (auto& door_ : doors_)
	{
		// 開いている(開こうとしている)扉や、動いている途中の扉は何もしない
		if (door_.isOpenTarget_ || IsMoving(door_))
		{
			continue;
		}

		// 扉に手が届く位置にいること(同じ階)
		if (!IsNear(enemyPos, door_.pos_, ENEMY_OPEN_DIST))
		{
			continue;
		}

		// 教室の扉はいつでも開ける
		if (door_.type_ == TYPE::SLIDE)
		{
			door_.isOpenTarget_ = true;
			continue;
		}

		// 個室・ロッカーは、プレイヤーが扉のすぐそばにいるときだけ
		if (!IsNear(playerPos, door_.pos_, HIDE_PLAYER_DIST))
		{
			continue;
		}

		// 敵がプレイヤーにとても近いか、隠れた相手を追っているとき
		bool isVeryClose_ = enemyToPlayer_ <= HIDE_VERY_CLOSE;
		if (isVeryClose_ || canOpenHide)
		{
			door_.isOpenTarget_ = true;
		}
	}
}

float DoorManager::HorizontalDist(const VECTOR& a, const VECTOR& b)
{
	float dx_ = a.x - b.x;
	float dz_ = a.z - b.z;
	return sqrtf(dx_ * dx_ + dz_ * dz_);
}

bool DoorManager::IsNear(const VECTOR& a, const VECTOR& b, float dist)
{
	if (fabsf(a.y - b.y) > OPEN_HEIGHT_RANGE)
	{
		return false;
	}
	return HorizontalDist(a, b) <= dist;
}