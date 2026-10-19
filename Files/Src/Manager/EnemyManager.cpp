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
		enemies_.push_back(std::make_unique<EnemyGhost>(GHOST_POS_LIST[i_]));
	}

	// それぞれの初期化
	for (auto& enemy_ : enemies_)
	{
		enemy_->Init();
	}
}

void EnemyManager::Update(void)
{
	for (auto& enemy_ : enemies_)
	{
		enemy_->Update();
	}
}

void EnemyManager::Draw(void)
{
	for (auto& enemy_ : enemies_)
	{
		enemy_->Draw();
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