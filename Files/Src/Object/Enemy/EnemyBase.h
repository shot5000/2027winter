#pragma once
#include <DxLib.h>
#include "../Collision/BodyCollision.h"
#include "EnemySensor.h"

class EnemyBase
{

public:

	// これより近いときは動かない(向きが決まらないので)
	static constexpr float ARRIVE_DIST = 1.0f;

	// コンストラクタ(置く座標、体の形、最初の向きを受け取る)
	EnemyBase(
		const VECTOR& pos,
		const BodyCollision::PARAM& bodyParam,
		float angleY);

	// デストラクタ
	virtual ~EnemyBase(void);

	// 初期化
	virtual void Init(void);

	// 更新(敵ごとの動きのあとに、壁・重力・地面の判定をする)
	void Update(const VECTOR& targetPos);

	// 描画(敵ごとに見た目が違うので、必ず子クラスで作る)
	virtual void Draw(void) = 0;

	// 当たり判定の表示(確認用)
	void DrawDebug(void) const;

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

	// 向き(Y軸の角度)
	float angleY_;

	// 頭が向いている向きのずれ(角度0のときの頭の向きと、+Z方向との差)
	float frontOffset_;

	// 当たり判定
	BodyCollision body_;

	// 索敵(使う敵だけ、子クラスのコンストラクタで Init する)
	EnemySensor sensor_;

	// 敵ごとの動き。判定の前に呼ばれる
	// 初期では、索敵の状態に応じて UpdateSearch / UpdateChase を呼ぶ
	virtual void UpdateProcess(const VECTOR& targetPos);

	// 索敵中の動き(初期では何もしない=その場で停止)
	virtual void UpdateSearch(const VECTOR& targetPos);

	// 検知中の動き(初期では何もしない)
	virtual void UpdateChase(const VECTOR& targetPos);

	// 相手のほうへ頭を向けて、真っすぐ進む
	void ChaseStraight(const VECTOR& targetPos, float speed);

	// 索敵範囲の描画(子クラスの Draw から呼ぶ)
	void DrawSensor(void) const;

	// 頭が向いている向き(水平方向のベクトル)
	VECTOR GetFrontDir(void) const;

	// 角度から、水平方向の向きのベクトルを求める
	static VECTOR GetDirXZ(float angleY);

	// 前フレームからの経過時間(秒)
	float GetDeltaTime(void) const;

};