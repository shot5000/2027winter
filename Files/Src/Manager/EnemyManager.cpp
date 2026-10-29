#include <algorithm>
#include "AttackManager.h"
#include "../Object/Enemy/EnemyGhost.h"
#include "EnemyManager.h"

EnemyManager::EnemyManager(void)
{
}

EnemyManager::~EnemyManager(void)
{
}

void EnemyManager::Init(void)
{
	// 座標のリストの数だけ、EnemyGhostを作る
	enemies_.clear();
	for (int i_ = 0; i_ < GHOST_COUNT; i_++)
	{
		enemies_.push_back(std::make_unique<EnemyGhost>(
			GHOST_POS_LIST[i_], GHOST_ANGLE_LIST[i_]));
	}

	// それぞれの初期化
	for (auto& enemy_ : enemies_)
	{
		enemy_->Init();
	}
}

void EnemyManager::Update(const VECTOR& targetPos)
{
	for (auto& enemy_ : enemies_)
	{
		enemy_->Update(targetPos);
	}
}

void EnemyManager::Draw(void)
{
	for (auto& enemy_ : enemies_)
	{
		enemy_->Draw();
	}

	// 当たり判定の表示(モデルの上に重ねたいので、あとから描く)
	if (DEBUG_DRAW)
	{
		for (auto& enemy_ : enemies_)
		{
			enemy_->DrawDebug();
		}
	}
}

void EnemyManager::Release(void)
{
	for (auto& enemy_ : enemies_)
	{
		enemy_->Release();
	}
	enemies_.clear();
}

bool EnemyManager::IsHitPlayer(
	const BodyCollision& playerBody, const VECTOR& playerPos) const
{
	for (const auto& enemy_ : enemies_)
	{
		if (enemy_->IsHitPlayer(playerBody, playerPos))
		{
			return true;
		}
	}
	return false;
}

void EnemyManager::DefeatHitEnemies(const AttackManager& attackManager)
{
	// 攻撃に当たっている敵を、倒す
	for (auto& enemy_ : enemies_)
	{
		if (attackManager.IsHitBody(enemy_->GetBody(), enemy_->GetPos()))
		{
			enemy_->Defeat();
		}
	}

	// 倒れた敵を片付ける
	RemoveDefeated();
}

void EnemyManager::RemoveDefeated(void)
{
	// モデルなどの後始末をしてから、配列から外す
	for (auto& enemy_ : enemies_)
	{
		if (!enemy_->IsAlive())
		{
			enemy_->Release();
		}
	}

	enemies_.erase(
		std::remove_if(
			enemies_.begin(), enemies_.end(),
			[](const std::unique_ptr<EnemyBase>& enemy)
			{
				return !enemy->IsAlive();
			}),
		enemies_.end());
}