#pragma once
#include <DxLib.h>
#include "ItemBase.h"

class ItemMop : public ItemBase
{

public:

	// 名前
	static constexpr const char* NAME = "モップ";

	// 攻撃が出ている時間と、次に攻撃できるまでの待ち時間(秒)
	static constexpr float ACTIVE_TIME = 0.25f;
	static constexpr float COOLDOWN_TIME = 0.5f;

	// 汚れ1つを消すのにかかる長押しの時間(秒)
	static constexpr float CLEAN_TIME = 1.0f;

	// アイテムごとの値(攻撃も掃除もできる)
	static constexpr ItemBase::PARAM ITEM_PARAM = {
		NAME,
		ItemBase::KIND::BOTH,
		ACTIVE_TIME,
		COOLDOWN_TIME,
		CLEAN_TIME
	};

	// 攻撃(カプセル)の半径
	static constexpr float RADIUS = 15.0f;

	// 持ち手から先端までの長さ(カプセルの両端の球の中心の間)
	static constexpr float LENGTH = 30.0f;

	// 持ち手の位置：目の位置から、正面へ離す距離と、下へ下げる高さ
	static constexpr float HAND_FRONT_DIST = 30.0f;
	static constexpr float HAND_OFFSET_Y = -20.0f;

	// 仮の見た目(カプセル)の分割数、色、透明度
	static constexpr int DRAW_DIV_NUM = 16;
	static constexpr unsigned int DRAW_COLOR = 0xff8800;
	static constexpr int DRAW_ALPHA = 120;

	// コンストラクタ
	ItemMop(void);

	// デストラクタ
	~ItemMop(void) override;

	// 攻撃の描画
	void Draw(void) const override;

	// 相手の体(カプセル)に、攻撃が当たっているか
	bool IsHitBody(
		const BodyCollision& body, const VECTOR& bodyPos) const override;

protected:

	// 攻撃が出ている間の処理(見ている向きに、まっすぐ伸ばす)
	void UpdateActive(float rate) override;

private:

	// 持ち手側の座標
	VECTOR startPos_;

	// 先端側の座標
	VECTOR endPos_;

};