#include <DxLib.h>
#include "../../Manager/SceneManager.h"
#include "ItemBase.h"

ItemBase::ItemBase(const PARAM& param, const ItemParam& itemParam)
{
	param_ = param;
	itemParam_ = itemParam;
	isActive_ = false;
	activeTimer_ = 0.0f;
	cooldownTimer_ = 0.0f;
	durabilityLeft_ = itemParam_.durability_;
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
	cooldownTimer_ = itemParam_.cooldownTime_;
	OnStart();
	ConsumeDurability();

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

bool ItemBase::IsActive(void) const
{
	return isActive_;
}

int ItemBase::GetAttackPower(void) const
{
	return itemParam_.attackPower_;
}

float ItemBase::GetAttackReach(void) const
{
	return itemParam_.attackReach_;
}

float ItemBase::GetAttackRadius(void) const
{
	return itemParam_.attackRadius_;
}

float ItemBase::GetMoveSpeedRate(void) const
{
	return itemParam_.moveSpeedRate_;
}

int ItemBase::GetDurability(void) const
{
	return itemParam_.durability_;
}

float ItemBase::GetAttackVolume(void) const
{
	return itemParam_.attackVolume_;
}

float ItemBase::GetCleanVolume(void) const
{
	return itemParam_.cleanVolume_;
}

int ItemBase::GetPrice(void) const
{
	return itemParam_.price_;
}

float ItemBase::GetCleanReach(void) const
{
	return itemParam_.cleanReach_;
}

float ItemBase::GetCleanTime(void) const
{
	// 掃除速度が速いほど、かかる時間は短くなる
	float speed_ = itemParam_.cleanSpeed_;
	if (speed_ < MIN_CLEAN_SPEED)
	{
		speed_ = MIN_CLEAN_SPEED;
	}
	return BASE_CLEAN_TIME / speed_;
}

void ItemBase::ConsumeDurability(void)
{
	if (IsBreakable() && durabilityLeft_ > 0)
	{
		durabilityLeft_--;
	}
}

bool ItemBase::IsBreakable(void) const
{
	return itemParam_.durability_ > 0;
}

bool ItemBase::IsBroken(void) const
{
	return IsBreakable() && durabilityLeft_ <= 0;
}

int ItemBase::GetDurabilityLeft(void) const
{
	return durabilityLeft_;
}

const char* ItemBase::GetDescription(void) const
{
	return itemParam_.description_.c_str();
}

void ItemBase::OnStart(void)
{
}

void ItemBase::UpdateActive(float rate)
{
}