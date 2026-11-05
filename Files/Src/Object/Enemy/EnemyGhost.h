#pragma once
#include "EnemyBase.h"

class EnemyGhost : public EnemyBase
{

public:

	// 性能(JSON)の名前
	static constexpr const char* PARAM_ID = "ghost";

	// 体の半径
	static constexpr float BODY_RADIUS = 30.0f;

	// 頭から尻尾までの長さの半分(横向きなので、前後のZ方向)
	static constexpr float BODY_HALF_LENGTH = 75.0f;

	// 当たり判定を頭側へずらす距離(Z方向。頭が-Z側のときは、マイナスにする)
	static constexpr float BODY_SHIFT_Z = -35.0f;

	// 足元から、カプセルの底までの浮かせる高さ(これより低い段差は上れる)
	static constexpr float STEP_HEIGHT = 20.0f;

	// 足元から、カプセルの中心までの高さ
	static constexpr float BODY_CENTER_Y = BODY_RADIUS + STEP_HEIGHT;

	// カプセルの中心から、両端の球の中心までの距離(Z方向)
	static constexpr float BODY_END_Z = BODY_HALF_LENGTH - BODY_RADIUS;

	// 当たり判定の体の形(基準は足元。Z方向に寝かせた横向き)
	static constexpr BodyCollision::PARAM BODY_PARAM = {
		BODY_RADIUS,
		{ 0.0f, BODY_CENTER_Y, BODY_SHIFT_Z + BODY_END_Z },
		{ 0.0f, BODY_CENTER_Y, BODY_SHIFT_Z - BODY_END_Z },
		0.0f
	};

	// モデルの大きさ
	static constexpr VECTOR MODEL_SCALE = { 0.5f, 0.5f, 0.5f };

	// モデルを描く高さのずらし(足元からの差。浮いて見えたり、沈んで見えたときに調整する)
	static constexpr float MODEL_OFFSET_Y = 0.0f;

	// 頭が向いている向きのずれ(角度0のとき、頭は-Z側を向いているので180度)
	static constexpr float FRONT_ANGLE_OFFSET = DX_PI_F;

	// 足元の中心から、頭の先までの距離(扇の要の位置)
	static constexpr float SEARCH_ORIGIN_DIST = BODY_HALF_LENGTH - BODY_SHIFT_Z;

	// コンストラクタ(置く座標と、最初の向きを受け取る)
	EnemyGhost(const VECTOR& pos, float angleY);

	// デストラクタ
	~EnemyGhost(void) override;

	// 初期化
	void Init(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

private:

	// モデルのハンドルID
	int modelId_;

};