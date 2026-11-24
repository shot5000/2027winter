#pragma once
#include <string>
#include <vector>
#include "SceneBase.h"
#include "../Object/Item/ItemManager.h"
#include "../Object/Item/ItemParam.h"

class ShopScene : public SceneBase
{

public:

	// 並べるアイテムの数と、列の数
	static constexpr int ITEM_COUNT = 6;
	static constexpr int COLUMN_COUNT = 2;
	static constexpr int ROW_COUNT = ITEM_COUNT / COLUMN_COUNT;

	// 並べるアイテム(入れ替えるときは、ここを変える)
	static constexpr ItemManager::ITEM_ID STOCK[ITEM_COUNT] = {
		ItemManager::ITEM_ID::BROOM,
		ItemManager::ITEM_ID::VACUUM,
		ItemManager::ITEM_ID::SPRAY,
		ItemManager::ITEM_ID::TALISMAN,
		ItemManager::ITEM_ID::BAT,
		ItemManager::ITEM_ID::SPONGE,
	};

	// 選んでいる場所の番号(ゲームを始めるボタンは、アイテムの次の番号)
	static constexpr int START_INDEX = ITEM_COUNT;

	// ステータスの表示の基準(この値で、ゲージがいっぱいになる)
	static constexpr float ATTACK_POWER_MAX = 10.0f;
	static constexpr float CLEAN_SPEED_MAX = 2.0f;
	static constexpr float ATTACK_REACH_MAX = 60.0f;
	static constexpr float CLEAN_REACH_MAX = 200.0f;

	// 重さの表示の基準(歩く速さの割合が小さいほど重い)
	static constexpr float MOVE_RATE_MIN = 0.8f;
	static constexpr float MOVE_RATE_MAX = 1.3f;

	// ゲージの割合の、最小と最大
	static constexpr float RATE_MIN = 0.0f;
	static constexpr float RATE_MAX = 1.0f;

	// ステータスの名前
	static constexpr const char* LABEL_ATTACK = "攻撃力";
	static constexpr const char* LABEL_CLEAN = "掃除力";
	static constexpr const char* LABEL_REACH = "リーチ";
	static constexpr const char* LABEL_WEIGHT = "重さ";

	// 画面の配置
	static constexpr int MARGIN_X = 30;
	static constexpr int TITLE_Y = 16;
	static constexpr int MESSAGE_Y = 40;
	static constexpr int TOP_AREA = 70;
	static constexpr int BOTTOM_AREA = 130;
	static constexpr int CELL_GAP_X = 16;
	static constexpr int CELL_GAP_Y = 12;

	// マスの中の配置
	static constexpr int PADDING = 10;
	static constexpr int ICON_SIZE = 100;
	static constexpr int LINE_H = 20;
	static constexpr int STAT_COLUMN_COUNT = 2;
	static constexpr int LABEL_W = 56;
	static constexpr int GAUGE_H = 10;
	static constexpr int GAUGE_GAP = 8;

	// ゲームを始めるボタン
	static constexpr int BUTTON_W = 240;
	static constexpr int BUTTON_H = 48;
	static constexpr int BUTTON_GAP = 15;
	static constexpr int HELP_GAP = 12;

	// 枠の太さ(選んでいるマスは太くする)
	static constexpr int FRAME_THICK = 1;
	static constexpr int SELECT_FRAME_THICK = 3;

	// 色
	static constexpr unsigned int BACK_COLOR = 0x202020;
	static constexpr unsigned int CELL_COLOR = 0x333333;
	static constexpr unsigned int ICON_COLOR = 0x555555;
	static constexpr unsigned int FRAME_COLOR = 0x888888;
	static constexpr unsigned int SELECT_FRAME_COLOR = 0xffff00;
	static constexpr unsigned int TEXT_COLOR = 0xffffff;
	static constexpr unsigned int PRICE_COLOR = 0xffff00;
	static constexpr unsigned int SHORT_COLOR = 0xff4444;
	static constexpr unsigned int MONEY_COLOR = 0xffff00;
	static constexpr unsigned int GAUGE_BACK_COLOR = 0x444444;
	static constexpr unsigned int GAUGE_COLOR = 0x66ccff;
	static constexpr unsigned int MESSAGE_COLOR = 0xffaa00;

	// 購入済みのマスにかぶせる暗さ(透明度)
	static constexpr int SOLD_ALPHA = 170;

	// 文字
	static constexpr const char* TITLE_TEXT = "ショップ";
	static constexpr const char* MONEY_TEXT = "所持金 %d円";
	static constexpr const char* PRICE_TEXT = "%d円";
	static constexpr const char* ICON_TEXT = "ICON";
	static constexpr const char* SOLD_TEXT = "購入済み";
	static constexpr const char* START_TEXT = "ゲームを始める";
	static constexpr const char* HELP_TEXT = "WASD・矢印:選ぶ  Enter・Space・左クリック:決定";
	static constexpr const char* MSG_BOUGHT = "購入しました";
	static constexpr const char* MSG_SOLD = "すでに購入しています";
	static constexpr const char* MSG_SHORT = "お金が足りません";

	// メッセージを出す時間(秒)
	static constexpr float MESSAGE_TIME = 1.5f;

	// 文字を作るバッファの大きさ
	static constexpr int TEXT_BUF_SIZE = 32;

	// 画面中心の計算用
	static constexpr int CENTER_DIV = 2;

	// コンストラクタ
	ShopScene(void);

	// デストラクタ
	~ShopScene(void) override;

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:

	// ステータス1つ(名前と、ゲージの割合)
	struct STAT
	{
		const char* label_;
		float rate_;
	};

	// ショップに並ぶアイテム1つ
	struct SLOT
	{
		const ItemManager::ITEM_DEF* def_;
		ItemParam param_;
		std::vector<STAT> stats_;
		bool isBought_;
	};

	// 長方形の範囲
	struct AREA
	{
		int x_;
		int y_;
		int w_;
		int h_;
	};

	// 並んでいるアイテム
	std::vector<SLOT> slots_;

	// 選んでいる場所(0?アイテム数-1がアイテム、START_INDEXがゲームを始めるボタン)
	int selected_;

	// 前フレームの、左クリックとマウスの位置
	bool isPrevClick_;
	int prevMouseX_;
	int prevMouseY_;

	// 画面に出すメッセージと、残り時間
	const char* message_;
	float messageTimer_;

	// 更新の中身
	void UpdateMouse(void);
	void UpdateKey(void);
	void MoveSelect(int dirX, int dirY);

	// 決定(アイテムなら購入、ボタンならゲーム開始)
	void Decide(void);
	void Buy(int index);
	void StartGame(void);
	void SetMessage(const char* message);

	// 並べるアイテムを作る
	void CreateSlots(void);
	static float ToRate(float value, float maxValue);

	// マスとボタンの範囲
	AREA GetCellArea(int index) const;
	AREA GetButtonArea(void) const;
	static bool IsInside(const AREA& area, int x, int y);

	// 描画
	void DrawCell(int index) const;
	void DrawStats(const SLOT& slot, const AREA& cell, int textX, int textW) const;
	void DrawButton(void) const;
	void DrawFrame(const AREA& area, bool isSelected) const;
	void DrawCenterString(
		const AREA& area, const char* text, unsigned int color) const;
	int DrawWrapped(
		const std::string& text, int x, int y, int maxWidth, unsigned int color) const;

};