#include <DxLib.h>
#include "../Manager/SceneManager.h"
#include "AttackBase.h"

AttackBase::AttackBase(const PARAM& param)
{
	param_ = param;
	isActive_ = false;
	activeTimer_ = 0.0f;
	cooldownTimer_ = 0.0f;
}

AttackBase::~AttackBase(void)
{
}

void AttackBase::Init(void)
{
	isActive_ = false;
	activeTimer_ = 0.0f;
	cooldownTimer_ = 0.0f;
}

bool AttackBase::Trigger(void)
{
	// 出ている間と、待ち時間中は出せない
	if (isActive_ || cooldownTimer_ > 0.0f)
	{
		return false;
	}

	isActive_ = true;
	activeTimer_ = param_.activeTime_;
	cooldownTimer_ = param_.cooldownTime_;
	OnStart();

	return true;
}

void AttackBase::Update(void)
{
	float deltaTime_ = SceneManager::GetInstance().GetDeltaTime();

	// 次に出せるまでの待ち時間を進める
	if (cooldownTimer_ > 0.0f)
	{
		cooldownTimer_ -= deltaTime_;
	}

	if (!isActive_)
	{
		return;
	}

	// 経過の割合を求めて、攻撃ごとの処理を呼ぶ
	float rate_ = (param_.activeTime_ - activeTimer_) / param_.activeTime_;
	if (rate_ < RATE_MIN)
	{
		rate_ = RATE_MIN;
	}
	if (rate_ > RATE_MAX)
	{
		rate_ = RATE_MAX;
	}
	UpdateActive(rate_);

	// 出ている時間が終わったら消す
	activeTimer_ -= deltaTime_;
	if (activeTimer_ <= 0.0f)
	{
		isActive_ = false;
	}
}

bool AttackBase::IsActive(void) const
{
	return isActive_;
}

void AttackBase::OnStart(void)
{
}