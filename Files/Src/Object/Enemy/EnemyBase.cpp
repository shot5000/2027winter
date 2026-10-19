#include "EnemyBase.h"
#include "../../Manager/SceneManager.h"

EnemyBase::EnemyBase(const VECTOR& pos, const BodyCollision::PARAM& bodyParam)
{
	pos_ = pos;
	body_.Init(bodyParam);
}

EnemyBase::~EnemyBase(void)
{
}

void EnemyBase::Init(void)
{
}

void EnemyBase::Update(void)
{
	// 敵ごとの動き
	UpdateProcess();

	// 移動を済ませたあとに、壁・重力・地面の判定(全員共通)
	body_.Update(pos_, SceneManager::GetInstance().GetDeltaTime());
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

void EnemyBase::UpdateProcess(void)
{
}