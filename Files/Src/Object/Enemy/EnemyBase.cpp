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
	isAlive_ = true;
	// 性能は、初期値を入れておく(子クラスが、JSONの値で上書きする)
	ApplyParam(EnemyParam());
	stunTimer_ = 0.0f;
}

EnemyBase::~EnemyBase(void)
{
}

void EnemyBase::Init(void)
{
}

void EnemyBase::Update(const VECTOR& targetPos)
{
	if (IsStun())
	{
		// 怯んでいる間は、動かずに、時間だけ進める
		stunTimer_ -= GetDeltaTime();
	}
	else
	{
		// 敵ごとの動き
		UpdateProcess(targetPos);
	}

	// 向きが変わっていたら、当たり判定も同じ向きにする
	body_.SetAngleY(angleY_);

	// 壁・重力・地面の判定(全員共通。怯んでいる間も、落ちる動きは続ける)
	body_.Update(pos_, GetDeltaTime());
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

void EnemyBase::Damage(int damage)
{
	// 倒れているとき、怯んでいる間は、ダメージを受けない
	if (!isAlive_ || IsStun())
	{
		return;
	}

	hp_ -= damage;
	if (hp_ <= 0)
	{
		isAlive_ = false;
		return;
	}

	// 怯んで、赤く点滅する
	stunTimer_ = STUN_TIME;
}

void EnemyBase::Defeat(void)
{
	isAlive_ = false;
}

const BodyCollision& EnemyBase::GetBody(void) const
{
	return body_;
}

int EnemyBase::GetAttackPower(void) const
{
	return attackPower_;
}

void EnemyBase::ApplyParam(const EnemyParam& param)
{
	hp_ = param.hp_;
	searchSpeed_ = param.searchSpeed_;
	chaseSpeed_ = param.chaseSpeed_;
	attackPower_ = param.attackPower_;
}

void EnemyBase::MoveForward(float speed)
{
	if (speed <= 0.0f)
	{
		return;
	}

	// 向いている方向へ、速さ×経過時間だけ進む
	pos_ = VAdd(pos_, VScale(GetFrontDir(), speed * GetDeltaTime()));
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
	// 索敵中は、索敵時の速さで、向いている方向へ進む(速さが0なら、その場で停止)
	MoveForward(searchSpeed_);
}

void EnemyBase::UpdateChase(const VECTOR& targetPos)
{
	// 検知中は、発見時の速さで、相手のほうへ真っすぐ向かう
	ChaseStraight(targetPos, chaseSpeed_);
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

bool EnemyBase::IsStun(void) const
{
	return stunTimer_ > 0.0f;
}

COLOR_F EnemyBase::GetColorScale(void) const
{
	// 怯んでいないときは、通常の色
	if (!IsStun())
	{
		return NORMAL_COLOR_SCALE;
	}

	// 怯み始めてからの時間で、赤と通常を、一定の間隔で交互にする(最初は赤)
	int phase_ = static_cast<int>((STUN_TIME - stunTimer_) / BLINK_INTERVAL);
	if (phase_ % BLINK_DIV == 0)
	{
		return FLASH_COLOR_SCALE;
	}
	return NORMAL_COLOR_SCALE;
}