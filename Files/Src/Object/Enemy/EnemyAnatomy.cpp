#include <cmath>
#include "../../Manager/ParamManager.h"
#include "../../Manager/ResourceManager.h"
#include "EnemyAnatomy.h"

EnemyAnatomy::EnemyAnatomy(const VECTOR& pos, float angleY)
	: EnemyBase(pos, EnemyGhost::BODY_PARAM, angleY)
{
	modelId_ = Resource::INVALID_HANDLE;
	isSeen_ = false;

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

EnemyAnatomy::~EnemyAnatomy(void)
{
}

void EnemyAnatomy::Init(void)
{
	EnemyBase::Init();

	// 同じモデルを複数出せるように、複製したものを使う
	modelId_ = ResourceManager::GetInstance().LoadModelDuplicate(
		ResourceManager::SRC::ENEMY_GHOST);
	MV1SetScale(modelId_, EnemyGhost::MODEL_SCALE);
}

void EnemyAnatomy::Draw(void)
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

	// 動き出す範囲
	DrawActiveRange();
}

void EnemyAnatomy::Release(void)
{
	EnemyBase::Release();

	// 複製したモデルを消す
	ResourceManager::GetInstance().DeleteModelDuplicate(
		ResourceManager::SRC::ENEMY_GHOST, modelId_);
	modelId_ = Resource::INVALID_HANDLE;
}

void EnemyAnatomy::UpdateProcess(const VECTOR& targetPos)
{
	// 見られている間は、その場で止まる
	isSeen_ = IsSeen(targetPos);
	if (isSeen_)
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

bool EnemyAnatomy::IsSeen(const VECTOR& targetPos) const
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

void EnemyAnatomy::DrawActiveRange(void) const
{
	unsigned int color_ = isSeen_ ? COLOR_SEEN : COLOR_MOVING;
	VECTOR center_ = VGet(
		pos_.x, pos_.y + EnemySensor::DRAW_OFFSET_Y, pos_.z);
	float step_ = EnemySensor::CIRCLE_ANGLE / EnemySensor::CIRCLE_DIV_NUM;

	// 半透明で、ライトを切って、両面を描く設定にする
	SetUseLighting(FALSE);
	SetWriteZBuffer3D(FALSE);
	SetUseBackCulling(DX_CULLING_NONE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, EnemySensor::DRAW_ALPHA);

	// 円を、細かい三角形に分けて描く
	for (int i_ = 0; i_ < EnemySensor::CIRCLE_DIV_NUM; i_++)
	{
		VECTOR p0_ = VAdd(center_, VScale(GetDirXZ(step_ * i_), activeRange_));
		VECTOR p1_ = VAdd(center_, VScale(GetDirXZ(step_ * (i_ + 1)), activeRange_));
		DrawTriangle3D(center_, p0_, p1_, color_, TRUE);
	}

	// 設定を元に戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	SetUseBackCulling(DX_CULLING_LEFT);
	SetWriteZBuffer3D(TRUE);
	SetUseLighting(TRUE);
}