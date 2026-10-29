#include <DxLib.h>
#include "../Attack/AttackMop.h"
#include "AttackManager.h"

AttackManager::AttackManager(void)
{
	type_ = TYPE::MOP;
	isPrevClick_ = false;
}

AttackManager::~AttackManager(void)
{
}

void AttackManager::Init(void)
{
	// TYPE の並びと同じ順に、攻撃を作る
	attacks_.clear();
	attacks_.push_back(std::make_unique<AttackMop>());

	for (auto& attack_ : attacks_)
	{
		attack_->Init();
	}

	type_ = TYPE::MOP;

	// ゲームを始めたときに押していたクリックで、攻撃が出ないようにする
	isPrevClick_ = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;
}

void AttackManager::Update(void)
{
	// 左クリックを押した瞬間か
	bool isClick_ = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;
	bool isTrg_ = isClick_ && !isPrevClick_;
	isPrevClick_ = isClick_;

	// 今選んでいる攻撃を出す
	if (isTrg_)
	{
		attacks_[static_cast<size_t>(type_)]->Trigger();
	}

	// 全部の攻撃を更新する(切り替えたあとも、前の攻撃は最後まで続く)
	for (auto& attack_ : attacks_)
	{
		attack_->Update();
	}
}

void AttackManager::Draw(void) const
{
	for (const auto& attack_ : attacks_)
	{
		attack_->Draw();
	}
}

void AttackManager::Release(void)
{
	attacks_.clear();
}

void AttackManager::SetType(TYPE type)
{
	type_ = type;
}

bool AttackManager::IsHitBody(
	const BodyCollision& body, const VECTOR& bodyPos) const
{
	for (const auto& attack_ : attacks_)
	{
		if (attack_->IsHitBody(body, bodyPos))
		{
			return true;
		}
	}
	return false;
}