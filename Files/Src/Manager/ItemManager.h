#pragma once
#include <memory>
#include <DxLib.h>
#include "../Object/Item/ItemBase.h"

class ItemManager
{

public:

	// アイテムの番号(増やすときは、ここと Create に足す)
	enum class ITEM_ID
	{
		MOP,
		MAX,
	};

	// スロットの数と、最初のスロットの番号
	static constexpr int SLOT_COUNT = 3;
	static constexpr int FIRST_SLOT = 0;

	// 数字キーで選べるスロット(左から順に1、2、3)
	static constexpr int SELECT_KEYS[SLOT_COUNT] = {
		KEY_INPUT_1, KEY_INPUT_2, KEY_INPUT_3
	};

	// スロットの大きさ、間隔、画面の下からの距離
	static constexpr int SLOT_SIZE = 80;
	static constexpr int SLOT_GAP = 16;
	static constexpr int BOTTOM_MARGIN = 40;

	// スロットの色と、枠の太さ(選んでいるスロットは、色と太さを変える)
	static constexpr unsigned int BOX_COLOR = 0x222222;
	static constexpr unsigned int FRAME_COLOR = 0x888888;
	static constexpr unsigned int SELECT_FRAME_COLOR = 0xffff00;
	static constexpr int FRAME_THICK = 1;
	static constexpr int SELECT_FRAME_THICK = 4;

	// 文字の色と位置
	static constexpr unsigned int TEXT_COLOR = 0xffffff;
	static constexpr int NUMBER_OFFSET = 6;
	static constexpr int NAME_OFFSET_Y = 34;

	// 画面の中心の計算用
	static constexpr int CENTER_DIV = 2;

	// コンストラクタ
	ItemManager(void);

	// デストラクタ
	~ItemManager(void);

	// 初期化(最初のアイテムを持たせる)
	void Init(void);

	// 更新(スロットの選択、左クリックでの攻撃、全アイテムの更新)
	void Update(void);

	// 攻撃の描画(3D。文字などの2Dより前に呼ぶ)
	void DrawAttack(void) const;

	// スロットの描画(2D)
	void DrawSlot(void) const;

	// 解放
	void Release(void);

	// アイテムを作る(購入したときなどに使う)
	static std::unique_ptr<ItemBase> Create(ITEM_ID id);

	// スロットにアイテムを入れる(すでにあれば入れ替える)
	void SetItem(int slotIndex, std::unique_ptr<ItemBase> item);

	// 選んでいるアイテムで、掃除できるか
	bool CanClean(void) const;

	// 選んでいるアイテムの、汚れ1つを消すのにかかる時間(秒)
	float GetCleanTime(void) const;

	// どれかの攻撃が、相手の体(カプセル)に当たっているか
	bool IsHitBody(const BodyCollision& body, const VECTOR& bodyPos) const;

private:

	// 各スロットのアイテム(空のスロットは nullptr)
	std::unique_ptr<ItemBase> items_[SLOT_COUNT];

	// 選んでいるスロットの番号
	int selectedIndex_;

	// 前フレームで、左クリックを押していたか
	bool isPrevClick_;

	// 選んでいるアイテム(なければ nullptr)
	ItemBase* GetSelectedItem(void) const;

	// スロットの選択(数字キーとホイール)
	void UpdateSelect(void);

	// 文字を左右中央にそろえて描く
	void DrawCenterString(
		int centerX, int posY, const char* text, unsigned int color) const;

};