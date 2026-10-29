#pragma once
#include <DxLib.h>
#include "AttackBase.h"

class AttackMop : public AttackBase
{

public:

	// 攻撃が出ている時間と、次に出せるまでの待ち時間(秒)
	static constexpr float ACTIVE_TIME = 0.25f;
	static constexpr float COOLDOWN_TIME = 0.5f;

	// 攻撃ごとの時間
	static constexpr AttackBase::PARAM ATTACK_PARAM = {
		ACTIVE_TIME,
		COOLDOWN_TIME
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
	AttackMop(void);

	// デストラクタ
	~AttackMop(void) override;

	// 描画
	void Draw(void) const override;

	// 相手の体(カプセル)に当たっているか
	bool IsHitBody(
		const BodyCollision& body, const VECTOR& bodyPos) const override;

	// 持ち手側の座標
	const VECTOR& GetStartPos(void) const;

	// 先端側の座標
	const VECTOR& GetEndPos(void) const;

	// 攻撃(カプセル)の半径
	float GetRadius(void) const;

protected:

	// 出ている間の処理(見ている向きに、まっすぐ伸ばす)
	void UpdateActive(float rate) override;

private:

	// 持ち手側の座標
	VECTOR startPos_;

	// 先端側の座標
	VECTOR endPos_;

};