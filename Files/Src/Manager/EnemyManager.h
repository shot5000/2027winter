#pragma once
#include <memory>
#include <vector>
#include <DxLib.h>
#include "../Object/Enemy/EnemyBase.h"
class ItemManager;

class EnemyManager
{

public:

	// 配置するEnemyGhostの数
	static constexpr int GHOST_COUNT = 2;

	// EnemyGhostを置く座標(増やすときは GHOST_COUNT も変える)
	static constexpr VECTOR GHOST_POS_LIST[GHOST_COUNT] = {
		{ -450.0f, 100.0f, 0.0f },
		{ 300.0f, 100.0f, 200.0f },
	};

	// EnemyGhostの最初の向き(Y軸の角度。0のとき、頭は-Z側を向く)
	static constexpr float GHOST_ANGLE_LIST[GHOST_COUNT] = {
		0.0f,
		0.0f,
	};

	// 攻撃1回で受けるダメージ
	static constexpr int ATTACK_DAMAGE = 1;

	// 当たり判定を表示するか(確認が終わったら false にする)
	static constexpr bool DEBUG_DRAW = true;

	// コンストラクタ
	EnemyManager(void);

	// デストラクタ
	~EnemyManager(void);

	// 初期化
	void Init(void);

	// 更新(追いかける相手の位置を受け取る)
	void Update(const VECTOR& targetPos);

	// 描画
	void Draw(void);

	// 解放
	void Release(void);

	// どれかの敵がプレイヤーに触れているか
	bool IsHitPlayer(
		const BodyCollision& playerBody, const VECTOR& playerPos) const;

	// 攻撃に当たった敵に、ダメージを与える(体力が0になった敵は倒れる)
	void DamageHitEnemies(const ItemManager& itemManager);

private:

	// 敵の配列(種類が違っても、EnemyBaseとしてまとめて持つ)
	std::vector<std::unique_ptr<EnemyBase>> enemies_;

	// 倒れた敵を、後始末して、配列から外す
	void RemoveDefeated(void);

};