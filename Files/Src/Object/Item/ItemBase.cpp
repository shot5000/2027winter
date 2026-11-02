#include <DxLib.h>
#include "../../Manager/SceneManager.h"
#include "ItemBase.h"

ItemBase::ItemBase(const PARAM& param)
{
	param_ = param;
	isActive_ = false;
	activeTimer_ = 0.0f;
	cooldownTimer_ = 0.0f;
}

ItemBase::~ItemBase(void)
{
}

void ItemBase::Init(void)
{
	isActive_ = false;
	activeTimer_ = 0.0f;
	cooldownTimer_ = 0.0f;
}

bool ItemBase::Attack(void)
{
	// 攻撃できないアイテム、出ている間、待ち時間中は出せない
	if (!CanAttack() || isActive_ || cooldownTimer_ > 0.0f)
	{
		return false;
	}

	isActive_ = true;
	activeTimer_ = param_.activeTime_;
	cooldownTimer_ = param_.cooldownTime_;
	OnStart();

	return true;
}

void ItemBase::Update(void)
{
	float deltaTime_ = SceneManager::GetInstance().GetDeltaTime();

	// 次に攻撃できるまでの待ち時間を進める
	if (cooldownTimer_ > 0.0f)
	{
		cooldownTimer_ -= deltaTime_;
	}

	if (!isActive_)
	{
		return;
	}

	// 経過の割合を求めて、アイテムごとの処理を呼ぶ
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

void ItemBase::Draw(void) const
{
}

bool ItemBase::IsHitBody(
	const BodyCollision& body, const VECTOR& bodyPos) const
{
	return false;
}

bool ItemBase::CanAttack(void) const
{
	return param_.kind_ != KIND::CLEAN;
}

bool ItemBase::CanClean(void) const
{
	return param_.kind_ != KIND::ATTACK;
}

const char* ItemBase::GetName(void) const
{
	return param_.name_;
}

float ItemBase::GetCleanTime(void) const
{
	return param_.cleanTime_;
}

bool ItemBase::IsActive(void) const
{
	return isActive_;
}

void ItemBase::OnStart(void)
{
}

void ItemBase::UpdateActive(float rate)
{
}