#pragma once
#include <memory>
#include <vector>
#include <DxLib.h>
#include "EnemyBase.h"
class ItemManager;
class DoorManager;

class EnemyManager
{

public:

	// 配置するEnemyGhostの数
	static constexpr int GHOST_COUNT = 2;
	// EnemyGhostを置く座標(増やすときは GHOST_COUNT も変える)
	static constexpr VECTOR GHOST_POS_LIST[GHOST_COUNT] = {
		{ -600.0f, 0.0f, 0.0f },
		{ 300.0f, 0.0f, 200.0f },
	};
	// EnemyGhostの最初の向き(Y軸の角度。0のとき、頭は-Z側を向く)
	static constexpr float GHOST_ANGLE_LIST[GHOST_COUNT] = {
		0.0f,
		0.0f,
	};

	// 配置するEnemyAnatomyの数
	static constexpr int ANATOMY_COUNT = 1;
	// EnemyAnatomyを置く座標
	static constexpr VECTOR ANATOMY_POS_LIST[ANATOMY_COUNT] = {
		{ 600.0f, 0.0f, 0.0f },
	};
	// EnemyAnatomyの最初の向き
	static constexpr float ANATOMY_ANGLE_LIST[ANATOMY_COUNT] = {
	0.0f,
	};

	// 配置するEnemyBlindの数
	static constexpr int BLIND_COUNT = 1;
	// EnemyBlindを置く座標
	static constexpr VECTOR BLIND_POS_LIST[BLIND_COUNT] = {
		{ -600.0f, 0.0f, 600.0f },
	};
	// EnemyBlindの最初の向き
	static constexpr float BLIND_ANGLE_LIST[BLIND_COUNT] = {
		0.0f,
	};

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

	// 追う相手(プレイヤー)が隠れているかを、全員に伝える
	void SetTargetHidden(bool isHidden);

	// 敵が扉を開ける(近くの扉を、ルールに合えば開く)
	void OpenDoors(DoorManager& doorMng, const VECTOR& playerPos);

private:

	// 敵の配列(種類が違っても、EnemyBaseとしてまとめて持つ)
	std::vector<std::unique_ptr<EnemyBase>> enemies_;

	// 倒れた敵を、後始末して、配列から外す
	void RemoveDefeated(void);

};