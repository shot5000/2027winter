#include <cstring>
#include <DxLib.h>
#include "../../Manager/InputManager.h"
#include "../Noise/NoiseManager.h"
#include "ItemManager.h"

ItemManager::ItemManager(void)
{
	selectedIndex_ = FIRST_SLOT;
	isPrevClick_ = false;
}

ItemManager::~ItemManager(void)
{
}

void ItemManager::Init(void)
{
	// いったん全部のスロットを空にして、最初のスロットにモップを入れる
	Release();
	SetItem(FIRST_SLOT, Create(ITEM_ID::MOP));
	selectedIndex_ = FIRST_SLOT;

	// ゲームを始めたときに押していたクリックで、攻撃が出ないようにする
	isPrevClick_ = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;

	// たまっているホイールの回転を捨てる
	GetMouseWheelRotVol();
}

void ItemManager::Update(void)
{
	// スロットの選択
	UpdateSelect();

	// 左クリックを押した瞬間か
	bool isClick_ = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;
	bool isTrg_ = isClick_ && !isPrevClick_;
	isPrevClick_ = isClick_;

	// 選んでいるアイテムが攻撃できるなら、出す
	ItemBase* selected_ = GetSelectedItem();
	if (isTrg_ && selected_ != nullptr && selected_->CanAttack())
	{
		selected_->Attack();

		// 攻撃の音(目の位置から出る。音量はアイテムのJSONで決める)
		NoiseManager::Emit(
			GetCameraPosition(),
			NoiseManager::ATTACK_BASE_RADIUS * selected_->GetAttackVolume());
	}

	// 持っているアイテム全部を更新する(切り替えたあとも、出ている攻撃は最後まで続く)
	for (auto& item_ : items_)
	{
		if (item_ != nullptr)
		{
			item_->Update();
		}
	}

	// 壊れたアイテムは、攻撃が終わったらスロットから消す
	for (auto& item_ : items_)
	{
		if (item_ != nullptr && item_->IsBroken() && !item_->IsActive())
		{
			item_.reset();
		}
	}
}

void ItemManager::DrawAttack(void) const
{
	for (const auto& item_ : items_)
	{
		if (item_ != nullptr)
		{
			item_->Draw();
		}
	}
}

void ItemManager::DrawSlot(void) const
{
	// 画面の下の中央に、スロットを横に並べる
	int screenW_ = 0;
	int screenH_ = 0;
	GetDrawScreenSize(&screenW_, &screenH_);
	int totalW_ = SLOT_SIZE * SLOT_COUNT + SLOT_GAP * (SLOT_COUNT - 1);
	int startX_ = (screenW_ - totalW_) / CENTER_DIV;
	int posY_ = screenH_ - BOTTOM_MARGIN - SLOT_SIZE;

	for (int i_ = 0; i_ < SLOT_COUNT; i_++)
	{
		int x_ = startX_ + (SLOT_SIZE + SLOT_GAP) * i_;

		// 背景
		DrawBox(x_, posY_, x_ + SLOT_SIZE, posY_ + SLOT_SIZE, BOX_COLOR, TRUE);

		// 枠(選んでいるスロットは、色を変えて太くする)
		unsigned int frameColor_ = FRAME_COLOR;
		int frameThick_ = FRAME_THICK;
		if (i_ == selectedIndex_)
		{
			frameColor_ = SELECT_FRAME_COLOR;
			frameThick_ = SELECT_FRAME_THICK;
		}
		for (int t_ = 0; t_ < frameThick_; t_++)
		{
			DrawBox(
				x_ + t_, posY_ + t_,
				x_ + SLOT_SIZE - t_, posY_ + SLOT_SIZE - t_,
				frameColor_, FALSE);
		}

		// スロットの番号
		DrawFormatString(
			x_ + NUMBER_OFFSET, posY_ + NUMBER_OFFSET, TEXT_COLOR, "%d", i_ + 1);

		// アイテムの名前と、耐久力(持っているときだけ)
		if (items_[i_] != nullptr)
		{
			DrawCenterString(
				x_ + SLOT_SIZE / CENTER_DIV, posY_ + NAME_OFFSET_Y,
				items_[i_]->GetName(), TEXT_COLOR);

			if (items_[i_]->IsBreakable())
			{
				char text_[TEXT_BUF_SIZE];
				snprintf(text_, sizeof(text_), DURABILITY_TEXT,
					items_[i_]->GetDurabilityLeft());
				DrawCenterString(
					x_ + SLOT_SIZE / CENTER_DIV, posY_ + DURABILITY_OFFSET_Y,
					text_, TEXT_COLOR);
			}
		}
	}
}

void ItemManager::Release(void)
{
	for (auto& item_ : items_)
	{
		item_.reset();
	}
}

std::unique_ptr<ItemBase> ItemManager::Create(ITEM_ID id)
{
	// 表から、アイテムの定義を引く
	int index_ = static_cast<int>(id);
	if (index_ < 0 || index_ >= static_cast<int>(ITEM_ID::MAX))
	{
		return nullptr;
	}

	const ITEM_DEF& def_ = ITEM_TABLE[index_];
	return std::make_unique<ItemStick>(def_.param_, def_.paramId_);
}

void ItemManager::SetItem(int slotIndex, std::unique_ptr<ItemBase> item)
{
	if (slotIndex < 0 || slotIndex >= SLOT_COUNT)
	{
		return;
	}

	items_[slotIndex] = std::move(item);
	if (items_[slotIndex] != nullptr)
	{
		items_[slotIndex]->Init();
	}
}

bool ItemManager::CanClean(void) const
{
	ItemBase* selected_ = GetSelectedItem();
	return selected_ != nullptr && selected_->CanClean();
}

float ItemManager::GetCleanTime(void) const
{
	ItemBase* selected_ = GetSelectedItem();
	if (selected_ == nullptr)
	{
		return 0.0f;
	}
	return selected_->GetCleanTime();
}

float ItemManager::GetCleanReach(void) const
{
	ItemBase* selected_ = GetSelectedItem();
	if (selected_ == nullptr)
	{
		return 0.0f;
	}
	return selected_->GetCleanReach();
}

float ItemManager::GetCleanVolume(void) const
{
	ItemBase* selected_ = GetSelectedItem();
	if (selected_ == nullptr)
	{
		return 0.0f;
	}
	return selected_->GetCleanVolume();
}

int ItemManager::GetHitDamage(
	const BodyCollision& body, const VECTOR& bodyPos) const
{
	int damage_ = NO_DAMAGE;
	for (const auto& item_ : items_)
	{
		if (item_ == nullptr || !item_->IsHitBody(body, bodyPos))
		{
			continue;
		}

		// 複数の攻撃が当たっているときは、大きいほうを使う
		if (item_->GetAttackPower() > damage_)
		{
			damage_ = item_->GetAttackPower();
		}
	}
	return damage_;
}

float ItemManager::GetMoveSpeedRate(void) const
{
	ItemBase* selected_ = GetSelectedItem();
	if (selected_ == nullptr)
	{
		return NO_ITEM_MOVE_RATE;
	}
	return selected_->GetMoveSpeedRate();
}

void ItemManager::ConsumeCleanDurability(void)
{
	ItemBase* selected_ = GetSelectedItem();
	if (selected_ != nullptr)
	{
		selected_->ConsumeDurability();
	}
}

bool ItemManager::HasEmptySlot(void) const
{
	for (const auto& item_ : items_)
	{
		if (item_ == nullptr)
		{
			return true;
		}
	}
	return false;
}

bool ItemManager::AddItem(std::unique_ptr<ItemBase>& item)
{
	for (int i_ = 0; i_ < SLOT_COUNT; i_++)
	{
		if (items_[i_] == nullptr)
		{
			SetItem(i_, std::move(item));
			return true;
		}
	}
	return false;
}

std::unique_ptr<ItemBase> ItemManager::TakeSelectedItem(void)
{
	return std::move(items_[selectedIndex_]);
}

ItemBase* ItemManager::GetSelectedItem(void) const
{
	return items_[selectedIndex_].get();
}

void ItemManager::UpdateSelect(void)
{
	InputManager& input_ = InputManager::GetInstance();

	// 数字キーで、スロットを選ぶ
	for (int i_ = 0; i_ < SLOT_COUNT; i_++)
	{
		if (input_.IsTrgDown(SELECT_KEYS[i_]))
		{
			selectedIndex_ = i_;
		}
	}

	// ホイールで選ぶ(奥へ回すと前のスロット、手前へ回すと次のスロット)
	int wheel_ = GetMouseWheelRotVol();
	if (wheel_ > 0)
	{
		selectedIndex_ = (selectedIndex_ + SLOT_COUNT - 1) % SLOT_COUNT;
	}
	if (wheel_ < 0)
	{
		selectedIndex_ = (selectedIndex_ + 1) % SLOT_COUNT;
	}
}

void ItemManager::DrawCenterString(
	int centerX, int posY, const char* text, unsigned int color) const
{
	// 文字の幅を測って、左右中央にそろえる
	int textW_ = GetDrawStringWidth(text, static_cast<int>(strlen(text)));
	DrawString(centerX - textW_ / CENTER_DIV, posY, text, color);
}