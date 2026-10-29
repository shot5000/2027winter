#include <cmath>
#include "EnemyBase.h"
#include "../../Manager/SceneManager.h"

EnemyBase::EnemyBase(
	const VECTOR& pos,
	const BodyCollision::PARAM& bodyParam,
	float angleY)
{
	pos_ = pos;
	angleY_ = angleY;
	frontOffset_ = 0.0f;
	body_.Init(bodyParam);
	body_.SetAngleY(angleY_);
}

EnemyBase::~EnemyBase(void)
{
}

void EnemyBase::Init(void)
{
}

void EnemyBase::Update(const VECTOR& targetPos)
{
	// 敵ごとの動き
	UpdateProcess(targetPos);

	// 向きが変わっていたら、当たり判定も同じ向きにする
	body_.SetAngleY(angleY_);

	// 移動を済ませたあとに、壁・重力・地面の判定(全員共通)
	body_.Update(pos_, GetDeltaTime());

	isAlive_ = true;
}

void EnemyBase::DrawDebug(void) const
{
	body_.DrawDebug(pos_);
}

void EnemyBase::Release(void)
{
}

bool EnemyBase::IsHitPlayer(
	const BodyCollision& playerBody, const VECTOR& playerPos) const
{
	// 敵とプレイヤーのカプセルが重なっているか
	return body_.IsHit(pos_, playerBody, playerPos);
}

const VECTOR& EnemyBase::GetPos(void) const
{
	return pos_;
}

bool EnemyBase::IsAlive(void) const
{
	return isAlive_;
}

void EnemyBase::Defeat(void)
{
	isAlive_ = false;
}

const BodyCollision& EnemyBase::GetBody(void) const
{
	return body_;
}

void EnemyBase::UpdateProcess(const VECTOR& targetPos)
{
	// 索敵を更新して、状態に応じた動きを呼ぶ
	sensor_.Update(pos_, GetFrontDir(), targetPos);
	if (sensor_.IsChasing())
	{
		UpdateChase(targetPos);
	}
	else
	{
		UpdateSearch(targetPos);
	}
}

void EnemyBase::UpdateSearch(const VECTOR& targetPos)
{
}

void EnemyBase::UpdateChase(const VECTOR& targetPos)
{
}

void EnemyBase::ChaseStraight(const VECTOR& targetPos, float speed)
{
	// 相手への水平方向のベクトル(高さは重力が決める)
	VECTOR diff_ = VSub(targetPos, pos_);
	diff_.y = 0.0f;

	// ほぼ重なっているときは動かない
	if (VSize(diff_) < ARRIVE_DIST)
	{
		return;
	}

	// 頭が相手のほうを向くように、向きを変える
	angleY_ = atan2f(diff_.x, diff_.z) - frontOffset_;

	// その方向へ、速さ×経過時間だけ進む
	VECTOR dir_ = VNorm(diff_);
	pos_ = VAdd(pos_, VScale(dir_, speed * GetDeltaTime()));
}

void EnemyBase::DrawSensor(void) const
{
	sensor_.Draw(pos_, GetFrontDir());
}

VECTOR EnemyBase::GetFrontDir(void) const
{
	return GetDirXZ(angleY_ + frontOffset_);
}

VECTOR EnemyBase::GetDirXZ(float angleY)
{
	// Y軸の回転(カメラと同じ向き)。角度0で+Z、増えると+X側へ回る
	return VGet(sinf(angleY), 0.0f, cosf(angleY));
}

float EnemyBase::GetDeltaTime(void) const
{
	return SceneManager::GetInstance().GetDeltaTime();
}