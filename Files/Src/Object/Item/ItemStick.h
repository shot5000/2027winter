#pragma once
#include <DxLib.h>
#include "ItemBase.h"

// まっすぐ伸びる攻撃をするアイテム(モップ、ほうき、バットなど)。違いは表とJSONで決める
class ItemStick : public ItemBase
{

public:

	// 攻撃が出ている時間(秒)
	static constexpr float ACTIVE_TIME = 0.25f;

	// 持ち手の位置：目の位置から、正面へ離す距離と、下へ下げる高さ
	static constexpr float HAND_FRONT_DIST = 30.0f;
	static constexpr float HAND_OFFSET_Y = -20.0f;

	// 仮の見た目(カプセル)の分割数、色、透明度
	static constexpr int DRAW_DIV_NUM = 16;
	static constexpr unsigned int DRAW_COLOR = 0xff8800;
	static constexpr int DRAW_ALPHA = 120;

	// コンストラクタ(コードで決める値と、JSONの名前を受け取る)
	ItemStick(const ItemBase::PARAM& param, const char* paramId);

	// デストラクタ
	~ItemStick(void) override;

	// 攻撃の描画
	void Draw(void) const override;

	// 相手の体(カプセル)に、攻撃が当たっているか
	bool IsHitBody(
		const BodyCollision& body, const VECTOR& bodyPos) const override;

protected:

	// 攻撃が出ている間の処理(見ている向きに、リーチの長さだけ、まっすぐ伸ばす)
	void UpdateActive(float rate) override;

private:

	// 持ち手側の座標
	VECTOR startPos_;

	// 先端側の座標
	VECTOR endPos_;

};