#pragma once
#include <DxLib.h>
#include "../Collision/BodyCollision.h"

class EnemyBase
{

public:

	// コンストラクタ(置く座標と、体の形を受け取る)
	EnemyBase(const VECTOR& pos, const BodyCollision::PARAM& bodyParam);

	// デストラクタ
	virtual ~EnemyBase(void);

	// 初期化
	virtual void Init(void);

	// 更新(敵ごとの動きのあとに、壁・重力・地面の判定をする)
	void Update(void);

	// 描画(敵ごとに見た目が違うので、必ず子クラスで作る)
	virtual void Draw(void) = 0;

	// 解放
	virtual void Release(void);

	// プレイヤーに触れているか
	bool IsHitPlayer(
		const BodyCollision& playerBody, const VECTOR& playerPos) const;

	// 座標の取得
	const VECTOR& GetPos(void) const;

protected:

	// 座標(足元)
	VECTOR pos_;

	// 当たり判定
	BodyCollision body_;

	// 敵ごとの動き(移動など)。判定の前に呼ばれる
	virtual void UpdateProcess(void);

};