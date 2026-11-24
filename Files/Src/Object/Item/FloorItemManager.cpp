#include "ItemManager.h"
#include "FloorItemManager.h"

FloorItemManager::FloorItemManager(void)
{
}

FloorItemManager::~FloorItemManager(void)
{
}

void FloorItemManager::Init(void)
{
	items_.clear();
}

void FloorItemManager::Draw(void) const
{
	// 仮の見た目。モデルができたら差し替える
	for (const auto& floor_ : items_)
	{
		DrawSphere3D(
			floor_.pos_, DRAW_RADIUS, DRAW_DIV_NUM,
			DRAW_COLOR, DRAW_COLOR, TRUE);
	}
}

void FloorItemManager::Release(void)
{
	items_.clear();
}

void FloorItemManager::Add(std::unique_ptr<ItemBase> item, const VECTOR& pos)
{
	FLOOR_ITEM floor_;
	floor_.item_ = std::move(item);
	floor_.pos_ = pos;
	items_.push_back(std::move(floor_));
}

VECTOR FloorItemManager::GetStartPos(int index)
{
	return START_POS_LIST[index % START_POS_COUNT];
}

int FloorItemManager::FindAimed(const VECTOR& eyePos, const VECTOR& dir) const
{
	VECTOR end_ = VAdd(eyePos, VScale(dir, PICK_REACH));

	// 視線が当たっているアイテムのうち、いちばん近いもの
	int found_ = NO_ITEM;
	float nearDist_ = PICK_REACH + PICK_RADIUS;
	for (size_t i_ = 0; i_ < items_.size(); i_++)
	{
		if (HitCheck_Line_Sphere(eyePos, end_, items_[i_].pos_, PICK_RADIUS) != TRUE)
		{
			continue;
		}

		float dist_ = VSize(VSub(items_[i_].pos_, eyePos));
		if (dist_ < nearDist_)
		{
			nearDist_ = dist_;
			found_ = static_cast<int>(i_);
		}
	}
	return found_;
}

const char* FloorItemManager::GetName(int index) const
{
	return items_[index].item_->GetName();
}

bool FloorItemManager::PickUp(int index, ItemManager& itemManager)
{
	if (index < 0 || index >= static_cast<int>(items_.size()))
	{
		return false;
	}

	// 空きスロットに入れられたときだけ、床から消す
	if (!itemManager.AddItem(items_[index].item_))
	{
		return false;
	}
	items_.erase(items_.begin() + index);
	return true;
}

void FloorItemManager::DropSelected(
	ItemManager& itemManager, const VECTOR& eyePos, const VECTOR& frontDir)
{
	std::unique_ptr<ItemBase> item_ = itemManager.TakeSelectedItem();
	if (item_ == nullptr)
	{
		return;
	}

	// 向いている方向(水平)の少し先の、足元に置く
	VECTOR front_ = frontDir;
	front_.y = 0.0f;
	front_ = VNorm(front_);

	VECTOR pos_ = VAdd(eyePos, VScale(front_, DROP_FRONT_DIST));
	pos_.y = eyePos.y + DROP_OFFSET_Y;
	Add(std::move(item_), pos_);
}