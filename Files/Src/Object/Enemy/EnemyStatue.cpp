#include <cmath>
#include "../../Manager/ParamManager.h"
#include "../../Manager/ResourceManager.h"
#include "EnemyStatue.h"

EnemyStatue::EnemyStatue(const VECTOR& pos, float angleY)
	: EnemyBase(pos, EnemyGhost::BODY_PARAM, angleY)
{
	modelId_ = Resource::INVALID_HANDLE;

	// JSONで決めた、体力・速さ・攻撃力を反映する
	const EnemyParam& param_ = ParamManager::GetInstance().GetEnemy(PARAM_ID);
	ApplyParam(param_);

	// 頭の向きは Ghost のモデルと同じ
	frontOffset_ = EnemyGhost::FRONT_ANGLE_OFFSET;

	// 動き出す範囲と、見られたとみなす視野(索敵の扇は使わない)
	activeRange_ = param_.chaseRange_;
	viewHalfAngle_ =
		param_.fanAngleDeg_ * EnemyParam::DEG_TO_RAD / EnemySensor::HALF_DIV;
}

EnemyStatue::~EnemyStatue(void)
{
}

void EnemyStatue::Init(void)
{
	EnemyBase::Init();

	// 同じモデルを複数出せるように、複製したものを使う
	modelId_ = ResourceManager::GetInstance().LoadModelDuplicate(
		ResourceManager::SRC::ENEMY_GHOST);
	MV1SetScale(modelId_, EnemyGhost::MODEL_SCALE);
}

void EnemyStatue::Draw(void)
{
	VECTOR drawPos_ = pos_;
	drawPos_.y += EnemyGhost::MODEL_OFFSET_Y;
	MV1SetPosition(modelId_, drawPos_);
	MV1SetRotationXYZ(modelId_, VGet(0.0f, angleY_, 0.0f));

	// 青っぽい色に、怯んでいるときの赤の点滅を重ねる
	COLOR_F flash_ = GetColorScale();
	COLOR_F color_ = {
		TINT_COLOR_SCALE.r * flash_.r,
		TINT_COLOR_SCALE.g * flash_.g,
		TINT_COLOR_SCALE.b * flash_.b,
		TINT_COLOR_SCALE.a * flash_.a
	};
	MV1SetDifColorScale(modelId_, color_);
	MV1DrawModel(modelId_);
}

void EnemyStatue::Release(void)
{
	EnemyBase::Release();

	// 複製したモデルを消す
	ResourceManager::GetInstance().DeleteModelDuplicate(
		ResourceManager::SRC::ENEMY_GHOST, modelId_);
	modelId_ = Resource::INVALID_HANDLE;
}

void EnemyStatue::UpdateProcess(const VECTOR& targetPos)
{
	// 見られている間は、その場で止まる
	if (IsSeen(targetPos))
	{
		return;
	}

	// 動き出す範囲の外にいるときは、動かない
	VECTOR diff_ = VSub(targetPos, pos_);
	diff_.y = 0.0f;
	if (VSize(diff_) > activeRange_)
	{
		return;
	}

	// 見られていないので、プレイヤーのほうへ進む
	ChaseStraight(targetPos, chaseSpeed_);
}

bool EnemyStatue::IsSeen(const VECTOR& targetPos) const
{
	// プレイヤーの目から、敵の体の中心への向き
	VECTOR center_ = VGet(pos_.x, pos_.y + VIEW_TARGET_Y, pos_.z);
	VECTOR toEnemy_ = VSub(center_, targetPos);
	float dist_ = VSize(toEnemy_);

	// 重なっているときは、向きが決まらないので、見られているとして止まる
	if (dist_ < ARRIVE_DIST)
	{
		return true;
	}

	// プレイヤーが向いている向きと比べて、視野の外なら見られていない
	VECTOR dir_ = VScale(toEnemy_, 1.0f / dist_);
	float dot_ = VDot(GetCameraFrontVector(), dir_);
	if (dot_ < cosf(viewHalfAngle_))
	{
		return false;
	}

	// 視野の中でも、壁や閉じた扉の向こうなら、見られていない
	return !BodyCollision::IsBlocked(targetPos, center_);
}