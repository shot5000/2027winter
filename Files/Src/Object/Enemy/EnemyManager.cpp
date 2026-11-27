#include <algorithm>
#include "../Item/ItemManager.h"
#include "EnemyGhost.h"
#include "EnemyManager.h"
#include "../Stage/DoorManager.h"
#include "EnemyRoute.h"
#include "EnemyAnatomy.h"
#include "EnemyBlind.h"
#include "../Noise/NoiseManager.h"
#include "EnemySpawn.h"

EnemyManager::EnemyManager(void)
{
}

EnemyManager::~EnemyManager(void)
{
}

void EnemyManager::Init(void)
{
	// 全員共通の、巡回する道を読む
	std::vector<VECTOR> route_ = EnemyRoute::Load();

	// 出現位置の候補を読む(種類ごとに、重ならないように取り出す)
	EnemySpawn spawn_;
	spawn_.Load();

	enemies_.clear();

	// EnemyGhost
	for (const VECTOR& pos_ : spawn_.Take(EnemyGhost::PARAM_ID))
	{
		auto ghost_ = std::make_unique<EnemyGhost>(pos_, spawn_.GetRandomAngle());
		ghost_->SetRoute(route_);
		enemies_.push_back(std::move(ghost_));
	}

	// EnemyAnatomy
	for (const VECTOR& pos_ : spawn_.Take(EnemyAnatomy::PARAM_ID))
	{
		enemies_.push_back(std::make_unique<EnemyAnatomy>(
			pos_, spawn_.GetRandomAngle()));
	}

	// EnemyBlind
	for (const VECTOR& pos_ : spawn_.Take(EnemyBlind::PARAM_ID))
	{
		enemies_.push_back(std::make_unique<EnemyBlind>(
			pos_, spawn_.GetRandomAngle()));
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

	NoiseManager::DrawDebug();
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

void EnemyManager::DamageHitEnemies(const ItemManager& itemManager)
{
	// 攻撃に当たっている敵に、その攻撃のダメージを与える
	for (auto& enemy_ : enemies_)
	{
		int damage_ = itemManager.GetHitDamage(enemy_->GetBody(), enemy_->GetPos());
		if (damage_ > ItemManager::NO_DAMAGE)
		{
			enemy_->Damage(damage_);
		}
	}

	// 倒れた敵を片付ける
	RemoveDefeated();
}

void EnemyManager::SetTargetHidden(bool isHidden)
{
	for (auto& enemy_ : enemies_)
	{
		enemy_->SetTargetHidden(isHidden);
	}
}

void EnemyManager::OpenDoors(DoorManager& doorMng, const VECTOR& playerPos)
{
	for (auto& enemy_ : enemies_)
	{
		doorMng.EnemyOpen(
			enemy_->GetPos(), enemy_->CanOpenHideDoor(playerPos), playerPos);
	}
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