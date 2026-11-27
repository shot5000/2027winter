#include "../../Manager/ParamManager.h"
#include "../../Manager/ResourceManager.h"
#include "EnemyBlind.h"

EnemyBlind::EnemyBlind(const VECTOR& pos, float angleY)
	: EnemyBase(pos, EnemyGhost::BODY_PARAM, angleY)
{
	modelId_ = Resource::INVALID_HANDLE;
	state_ = STATE::WAIT;
	lastHeardId_ = NoiseManager::NO_ID;
	noisePos_ = pos;
	listenTimer_ = 0.0f;

	// JSONで決めた、体力・速さ・攻撃力を反映する
	const EnemyParam& param_ = ParamManager::GetInstance().GetEnemy(PARAM_ID);
	ApplyParam(param_);

	// 頭の向きは Ghost のモデルと同じ
	frontOffset_ = EnemyGhost::FRONT_ANGLE_OFFSET;

	// JSONの fanRange を、音の聞こえやすさとして使う(索敵の扇は使わない)
	hearingRate_ = param_.fanRange_;
}

EnemyBlind::~EnemyBlind(void)
{
}

void EnemyBlind::Init(void)
{
	EnemyBase::Init();

	// 同じモデルを複数出せるように、複製したものを使う
	modelId_ = ResourceManager::GetInstance().LoadModelDuplicate(
		ResourceManager::SRC::ENEMY_GHOST);
	MV1SetScale(modelId_, EnemyGhost::MODEL_SCALE);
}

void EnemyBlind::Draw(void)
{
	VECTOR drawPos_ = pos_;
	drawPos_.y += EnemyGhost::MODEL_OFFSET_Y;
	MV1SetPosition(modelId_, drawPos_);
	MV1SetRotationXYZ(modelId_, VGet(0.0f, angleY_, 0.0f));

	// 橙色に、怯んでいるときの赤の点滅を重ねる
	COLOR_F flash_ = GetColorScale();
	COLOR_F color_ = {
		TINT_COLOR_SCALE.r * flash_.r,
		TINT_COLOR_SCALE.g * flash_.g,
		TINT_COLOR_SCALE.b * flash_.b,
		TINT_COLOR_SCALE.a * flash_.a
	};
	MV1SetDifColorScale(modelId_, color_);
	MV1DrawModel(modelId_);

	// 向かっているときは、目的地に印を出す
	if (state_ == STATE::MOVE)
	{
		VECTOR markPos_ = noisePos_;
		markPos_.y += MARK_OFFSET_Y;
		DrawSphere3D(
			markPos_, MARK_RADIUS, MARK_DIV_NUM, MARK_COLOR, MARK_COLOR, TRUE);
	}
}

void EnemyBlind::Release(void)
{
	EnemyBase::Release();

	// 複製したモデルを消す
	ResourceManager::GetInstance().DeleteModelDuplicate(
		ResourceManager::SRC::ENEMY_GHOST, modelId_);
	modelId_ = Resource::INVALID_HANDLE;
}

void EnemyBlind::UpdateProcess(const VECTOR& targetPos)
{
	// プレイヤーの位置は使わない(見えていないので、音だけで動く)
	Hear();

	switch (state_)
	{
	case STATE::WAIT:
		// 音が聞こえるまで、その場で止まる
		break;

	case STATE::MOVE:
		// 着いたら、聞き耳を立てる
		if (HorizontalDist(pos_, noisePos_) <= ARRIVE_RANGE)
		{
			state_ = STATE::LISTEN;
			listenTimer_ = LISTEN_TIME;
			break;
		}
		ChaseStraight(noisePos_, chaseSpeed_);
		break;

	case STATE::LISTEN:
		// 時間が過ぎたら、また止まる
		listenTimer_ -= GetDeltaTime();
		if (listenTimer_ <= 0.0f)
		{
			state_ = STATE::WAIT;
		}
		break;
	}
}

void EnemyBlind::Hear(void)
{
	int latestId_ = lastHeardId_;
	bool isHeard_ = false;
	float nearestDist_ = 0.0f;

	for (const auto& noise_ : NoiseManager::GetNoises())
	{
		// すでに聞いた音は飛ばす
		if (noise_.id_ <= lastHeardId_)
		{
			continue;
		}
		if (noise_.id_ > latestId_)
		{
			latestId_ = noise_.id_;
		}

		// 聞こえない音は飛ばす
		if (!IsAudible(noise_))
		{
			continue;
		}

		// 聞こえた音のうち、いちばん近い音を選ぶ
		float dist_ = HorizontalDist(pos_, noise_.pos_);
		if (!isHeard_ || dist_ < nearestDist_)
		{
			isHeard_ = true;
			nearestDist_ = dist_;
			noisePos_ = noise_.pos_;
		}
	}
	lastHeardId_ = latestId_;

	// 聞こえたら、その場所へ向かう(向かっている途中でも、新しい音を優先する)
	if (isHeard_)
	{
		state_ = STATE::MOVE;
	}
}

bool EnemyBlind::IsAudible(const NoiseManager::NOISE& noise) const
{
	// 違う階の音は聞こえない
	if (fabsf(noise.pos_.y - pos_.y) > SAME_FLOOR_RANGE)
	{
		return false;
	}

	// 音の範囲の中にいれば聞こえる(壁は通り抜ける)
	return HorizontalDist(pos_, noise.pos_) <= noise.radius_ * hearingRate_;
}

float EnemyBlind::HorizontalDist(const VECTOR& a, const VECTOR& b)
{
	VECTOR diff_ = VSub(a, b);
	diff_.y = 0.0f;
	return VSize(diff_);
}