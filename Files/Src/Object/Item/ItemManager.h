#pragma once
#include <memory>
#include <DxLib.h>
#include "ItemBase.h"
#include "ItemStick.h"

class ItemManager
{

public:

	// アイテムの番号(増やすときは、ここと ITEM_TABLE に足す)
	enum class ITEM_ID
	{
		MOP,
		BROOM,
		VACUUM,
		SPRAY,
		TALISMAN,
		PURIFY_WAND,
		BAT,
		SPEAR,
		SPONGE,
		ROLLER,
		MAX,
	};

	// アイテムの定義(JSONの名前と、コードで決める値)
	struct ITEM_DEF
	{
		const char* paramId_;
		ItemBase::PARAM param_;
	};

	// アイテムの表(ITEM_IDと同じ順番で並べる)
	static constexpr ITEM_DEF ITEM_TABLE[static_cast<int>(ITEM_ID::MAX)] = {
		{ "mop", { "モップ", ItemBase::KIND::BOTH, ItemStick::ACTIVE_TIME } },
		{ "broom", { "ほうき", ItemBase::KIND::BOTH, ItemStick::ACTIVE_TIME } },
		{ "vacuum", { "ハンディ掃除機", ItemBase::KIND::BOTH, ItemStick::ACTIVE_TIME } },
		{ "spray", { "消毒スプレー", ItemBase::KIND::BOTH, ItemStick::ACTIVE_TIME } },
		{ "talisman", { "お札", ItemBase::KIND::ATTACK, ItemStick::ACTIVE_TIME } },
		{ "purifyWand", { "お祓い棒", ItemBase::KIND::ATTACK, ItemStick::ACTIVE_TIME } },
		{ "bat", { "バット", ItemBase::KIND::ATTACK, ItemStick::ACTIVE_TIME } },
		{ "spear", { "槍", ItemBase::KIND::ATTACK, ItemStick::ACTIVE_TIME } },
		{ "sponge", { "スポンジ", ItemBase::KIND::CLEAN, ItemStick::ACTIVE_TIME } },
		{ "roller", { "粘着ローラー", ItemBase::KIND::CLEAN, ItemStick::ACTIVE_TIME } },
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

	// 攻撃が当たっていないときのダメージ
	static constexpr int NO_DAMAGE = 0;

	// アイテムを持っていないときの、歩く速さにかける割合
	static constexpr float NO_ITEM_MOVE_RATE = 1.0f;

	// 耐久力の残りの表示
	static constexpr const char* DURABILITY_TEXT = "残り%d";
	static constexpr int DURABILITY_OFFSET_Y = 56;
	static constexpr int TEXT_BUF_SIZE = 16;

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

	// 選んでいるアイテムで、掃除できる距離(アイテムを持っていなければ0)
	float GetCleanReach(void) const;

	// 選んでいるアイテムの、掃除の音量(アイテムを持っていなければ0)
	float GetCleanVolume(void) const;

	// 攻撃が相手の体(カプセル)に当たっていたら、そのダメージを返す(複数なら一番大きい値。当たっていなければ NO_DAMAGE)
	int GetHitDamage(const BodyCollision& body, const VECTOR& bodyPos) const;

	// 選んでいるアイテムの、歩く速さにかける割合
	float GetMoveSpeedRate(void) const;

	// 掃除したときに、選んでいるアイテムの耐久力を減らす
	void ConsumeCleanDurability(void);

	// 空いているスロットがあるか
	bool HasEmptySlot(void) const;

	// 空いている最初のスロットに入れる(入れられたら true。入れたときだけ、渡したアイテムは空になる)
	bool AddItem(std::unique_ptr<ItemBase>& item);

	// 選んでいるスロットのアイテムを取り出す(空なら nullptr)
	std::unique_ptr<ItemBase> TakeSelectedItem(void);

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