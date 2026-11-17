#include <fstream>
#include <map>
#include <cmath>
#include "../../External/json/json.hpp"
#include "../../Utility/AsoUtility.h"
#include "../Collision/BodyCollision.h"
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

void DoorManager::Update(float deltaTime, const std::vector<OCCUPANT>& occupants)
{
	for (auto& door_ : doors_)
	{
		float target_ = door_.isOpenTarget_ ? 1.0f : 0.0f;
		if (door_.rate_ == target_)
		{
			continue;
		}

		// 目標に向かって動かす
		float preRate_ = door_.rate_;
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

		// 閉めている途中で誰かを挟むなら、元に戻して開いたままにする
		if (!door_.isOpenTarget_ && IsBlocked(door_, occupants))
		{
			door_.rate_ = preRate_;
			door_.isOpenTarget_ = true;
			ApplyPose(door_);
		}
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

bool DoorManager::IsBlocked(
	const DOOR& door, const std::vector<OCCUPANT>& occupants) const
{
	for (const auto& occ_ : occupants)
	{
		if (occ_.body_->IsHitFrame(occ_.pos_, door.frame_))
		{
			return true;
		}
	}
	return false;
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
	doors_[index].isOpenTarget_ = !doors_[index].isOpenTarget_;
}

bool DoorManager::IsClosed(int index) const
{
	return doors_[index].rate_ <= 0.0f && !doors_[index].isOpenTarget_;
}

void DoorManager::EnemyOpen(
	const VECTOR& enemyPos, bool isChasing, const VECTOR& playerPos)
{
	float enemyToPlayer_ = HorizontalDist(enemyPos, playerPos);

	for (auto& door_ : doors_)
	{
		// 開いている(開こうとしている)扉は何もしない
		if (door_.isOpenTarget_)
		{
			continue;
		}

		// 扉に手が届く位置にいること
		if (HorizontalDist(enemyPos, door_.pos_) > ENEMY_OPEN_DIST)
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
		if (HorizontalDist(playerPos, door_.pos_) > HIDE_PLAYER_DIST)
		{
			continue;
		}

		// 敵がプレイヤーにとても近いか、発見状態で範囲内にいるとき
		bool isVeryClose_ = enemyToPlayer_ <= HIDE_VERY_CLOSE;
		bool isChaseIn_ = isChasing && enemyToPlayer_ <= HIDE_CHASE_RANGE;
		if (isVeryClose_ || isChaseIn_)
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