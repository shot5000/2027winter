#pragma once
#include "EnemyBase.h"

class EnemyGhost : public EnemyBase
{

public:

	// 体の半径
	static constexpr float BODY_RADIUS = 30.0f;

	// 足元から頭の先までの高さ
	static constexpr float BODY_HEIGHT = 140.0f;

	// 足元から、カプセルの底までの浮かせる高さ(これより低い段差は上れる)
	static constexpr float STEP_HEIGHT = 20.0f;

	// 当たり判定の体の形(基準は足元)
	static constexpr BodyCollision::PARAM BODY_PARAM = {
		BODY_RADIUS,				// 半径
		BODY_HEIGHT - BODY_RADIUS,	// 上の球の中心
		BODY_RADIUS + STEP_HEIGHT,	// 下の球の中心
		0.0f						// 足元
	};

	// モデルの大きさ
	static constexpr VECTOR MODEL_SCALE = { 0.5f, 0.5f, 0.5f };

	// モデルを描く高さのずらし(足元からの差。浮いて見えたり、沈んで見えたときに調整する)
	static constexpr float MODEL_OFFSET_Y = 0.0f;

	// コンストラクタ(置く座標を受け取る)
	EnemyGhost(const VECTOR& pos);

	// デストラクタ
	~EnemyGhost(void) override;

	// 初期化
	void Init(void) override;

	// 描画
	void Draw(void) override;

private:

	// モデルのハンドルID
	int modelId_;

};