#pragma once
#include <memory>
#include <vector>
#include <DxLib.h>
#include "ItemBase.h"
class ItemManager;

// 床に置かれたアイテムの管理(購入したあふれ分、捨てたアイテム)
class FloorItemManager
{

public:

	// 拾うキーと、捨てるキー
	static constexpr int PICK_KEY = KEY_INPUT_E;
	static constexpr int DROP_KEY = KEY_INPUT_G;

	// 拾える距離と、狙える大きさ(球の半径)
	static constexpr float PICK_REACH = 200.0f;
	static constexpr float PICK_RADIUS = 40.0f;

	// 仮の見た目(球)の大きさ、分割数、色
	static constexpr float DRAW_RADIUS = 25.0f;
	static constexpr int DRAW_DIV_NUM = 16;
	static constexpr unsigned int DRAW_COLOR = 0x66ccff;

	// 床に置いたときの、球の中心の高さ
	static constexpr float ITEM_Y = 30.0f;

	// 捨てるとき：目の位置から正面へ離す距離と、目の高さからの差(足元の床に球が乗る高さ)
	static constexpr float DROP_FRONT_DIST = 80.0f;
	static constexpr float DROP_OFFSET_Y = -80.0f;

	// 最初に置く場所(昇降口。スタート地点に合わせて直す)
	static constexpr int START_POS_COUNT = 6;
	static constexpr VECTOR START_POS_LIST[START_POS_COUNT] = {
		{ -250.0f, ITEM_Y, -300.0f },
		{ -150.0f, ITEM_Y, -300.0f },
		{  -50.0f, ITEM_Y, -300.0f },
		{   50.0f, ITEM_Y, -300.0f },
		{  150.0f, ITEM_Y, -300.0f },
		{  250.0f, ITEM_Y, -300.0f },
	};

	// 狙っているアイテムがないときの番号
	static constexpr int NO_ITEM = -1;

	// コンストラクタ
	FloorItemManager(void);

	// デストラクタ
	~FloorItemManager(void);

	// 初期化(床のアイテムを全部なくす)
	void Init(void);

	// 描画
	void Draw(void) const;

	// 解放
	void Release(void);

	// 床にアイテムを置く
	void Add(std::unique_ptr<ItemBase> item, const VECTOR& pos);

	// 最初に置く場所(番号が多いときは、最初から使い回す)
	static VECTOR GetStartPos(int index);

	// 視線の先にあるアイテムの番号(なければ NO_ITEM)
	int FindAimed(const VECTOR& eyePos, const VECTOR& dir) const;

	// アイテムの名前
	const char* GetName(int index) const;

	// 拾う(空きスロットに入れる。入れられなければ false)
	bool PickUp(int index, ItemManager& itemManager);

	// 選んでいるスロットのアイテムを、目の前の床に置く
	void DropSelected(
		ItemManager& itemManager, const VECTOR& eyePos, const VECTOR& frontDir);

private:

	// 床のアイテム1つ
	struct FLOOR_ITEM
	{
		std::unique_ptr<ItemBase> item_;
		VECTOR pos_;
	};

	std::vector<FLOOR_ITEM> items_;

};