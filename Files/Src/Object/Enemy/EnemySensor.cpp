#include <cmath>
#include "EnemySensor.h"

EnemySensor::EnemySensor(void)
{
	param_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	state_ = STATE::SEARCH;
	isEnabled_ = false;
}

EnemySensor::~EnemySensor(void)
{
}

void EnemySensor::Init(const PARAM& param)
{
	param_ = param;
	state_ = STATE::SEARCH;
	isEnabled_ = true;
	isTargetHidden_ = false;
}

void EnemySensor::Update(
	const VECTOR& pos, const VECTOR& frontDir, const VECTOR& targetPos)
{
	if (!isEnabled_)
	{
		return;
	}

	switch (state_)
	{
	case STATE::SEARCH:
		// 扇の中にプレイヤーが入ったら、検知する(隠れている間は見つけない)
		if (!isTargetHidden_ && IsInFan(pos, frontDir, targetPos))
		{
			state_ = STATE::CHASE;
		}
		break;
	case STATE::CHASE:
		// 円の外に出たら、索敵に戻す(向きは関係なく、円の中にいる間は検知したまま)
		if (!IsInCircle(pos, targetPos))
		{
			state_ = STATE::SEARCH;
		}
		// 隠れたときに、隠れ範囲の外にいたら、見失って索敵に戻す
		else if (isTargetHidden_ && !IsInHideRange(pos, targetPos))
		{
			state_ = STATE::SEARCH;
		}
		break;
	}
}

void EnemySensor::Draw(const VECTOR& pos, const VECTOR& frontDir) const
{
	if (!isEnabled_)
	{
		return;
	}

	// 隠れ範囲は、状態に関係なく、いつも描く
	DrawHideRange(pos);

	if (state_ == STATE::CHASE)
	{
		// 検知中は、敵の位置を中心にした円
		VECTOR center_ = pos;
		center_.y += DRAW_OFFSET_Y;
		DrawSector(
			center_, 0.0f, CIRCLE_ANGLE,
			param_.chaseRange_, CIRCLE_DIV_NUM, COLOR_CHASE);
	}
	else
	{
		// 検知前は、頭の向きを中心に、左右に開く扇
		VECTOR center_ = GetOrigin(pos, frontDir);
		center_.y += DRAW_OFFSET_Y;
		float frontAngle_ = atan2f(frontDir.x, frontDir.z);
		DrawSector(
			center_, frontAngle_ - param_.fanAngle_ / HALF_DIV, param_.fanAngle_,
			param_.fanRange_, FAN_DIV_NUM, COLOR_SEARCH);
	}
}

bool EnemySensor::IsChasing(void) const
{
	return isEnabled_ && state_ == STATE::CHASE;
}

void EnemySensor::SetTargetHidden(bool isHidden)
{
	isTargetHidden_ = isHidden;
}

bool EnemySensor::IsInHideRange(const VECTOR& pos, const VECTOR& targetPos) const
{
	// 敵の位置から相手への水平方向の距離(高さの違いは無視する)
	VECTOR diff_ = VSub(targetPos, pos);
	diff_.y = 0.0f;
	return VSize(diff_) <= HIDE_RANGE;
}

VECTOR EnemySensor::GetOrigin(const VECTOR& pos, const VECTOR& frontDir) const
{
	// 足元の中心から、頭の向きに進んだ位置(頭の先)
	return VAdd(pos, VScale(frontDir, param_.originDist_));
}

bool EnemySensor::IsInFan(
	const VECTOR& pos, const VECTOR& frontDir, const VECTOR& targetPos) const
{
	// 頭の先から相手への水平方向のベクトル(高さの違いは無視する)
	VECTOR diff_ = VSub(targetPos, GetOrigin(pos, frontDir));
	diff_.y = 0.0f;
	float dist_ = VSize(diff_);

	// 扇の半径より遠い
	if (dist_ > param_.fanRange_)
	{
		return false;
	}

	// ほぼ真上・真下で、向きが決まらないときは範囲の中とみなす
	if (dist_ < OVERLAP_DIST)
	{
		return true;
	}

	// 頭の向きとの差が、扇の半分の角度以内か(角度の比較は内積で行う)
	float dot_ = VDot(frontDir, VNorm(diff_));
	return dot_ >= cosf(param_.fanAngle_ / HALF_DIV);
}

bool EnemySensor::IsInCircle(const VECTOR& pos, const VECTOR& targetPos) const
{
	// 敵の位置から相手への水平方向の距離(高さの違いは無視する)
	VECTOR diff_ = VSub(targetPos, pos);
	diff_.y = 0.0f;
	return VSize(diff_) <= param_.chaseRange_;
}

void EnemySensor::DrawSector(
	const VECTOR& center, float startAngle, float totalAngle,
	float radius, int divNum, unsigned int color) const
{
	// 半透明で、ライトを切って、両面とも描く設定にする
	int prevCulling_ = GetUseBackCulling();
	SetUseLighting(FALSE);
	SetWriteZBuffer3D(FALSE);
	SetUseBackCulling(DX_CULLING_NONE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, DRAW_ALPHA);

	// 細かい三角形に分けて描く
	float stepAngle_ = totalAngle / static_cast<float>(divNum);
	for (int i_ = 0; i_ < divNum; i_++)
	{
		float angle1_ = startAngle + stepAngle_ * static_cast<float>(i_);
		float angle2_ = angle1_ + stepAngle_;
		VECTOR p1_ = VAdd(center, VScale(GetDirXZ(angle1_), radius));
		VECTOR p2_ = VAdd(center, VScale(GetDirXZ(angle2_), radius));
		DrawTriangle3D(center, p1_, p2_, color, TRUE);
	}

	// 設定を元に戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	SetUseBackCulling(prevCulling_);
	SetWriteZBuffer3D(TRUE);
	SetUseLighting(TRUE);
}

VECTOR EnemySensor::GetDirXZ(float angleY)
{
	// Y軸の回転(カメラと同じ向き)。角度0で+Z、増えると+X側へ回る
	return VGet(sinf(angleY), 0.0f, cosf(angleY));
}

void EnemySensor::DrawHideRange(const VECTOR& pos) const
{
	// ライトを切って、円周を細かい線に分けて描く
	SetUseLighting(FALSE);

	VECTOR center_ = pos;
	center_.y += DRAW_OFFSET_Y + HIDE_DRAW_OFFSET_Y;

	float stepAngle_ = CIRCLE_ANGLE / static_cast<float>(CIRCLE_DIV_NUM);
	for (int i_ = 0; i_ < CIRCLE_DIV_NUM; i_++)
	{
		float angle1_ = stepAngle_ * static_cast<float>(i_);
		float angle2_ = angle1_ + stepAngle_;
		VECTOR p1_ = VAdd(center_, VScale(GetDirXZ(angle1_), HIDE_RANGE));
		VECTOR p2_ = VAdd(center_, VScale(GetDirXZ(angle2_), HIDE_RANGE));
		DrawLine3D(p1_, p2_, COLOR_HIDE);
	}

	// 設定を元に戻す
	SetUseLighting(TRUE);
}