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