#include "../../Manager/ResourceManager.h"
#include "EnemyGhost.h"

EnemyGhost::EnemyGhost(const VECTOR& pos) : EnemyBase(pos, BODY_PARAM)
{
	modelId_ = Resource::INVALID_HANDLE;
}

EnemyGhost::~EnemyGhost(void)
{
}

void EnemyGhost::Init(void)
{
	EnemyBase::Init();

	// 同じモデルを複数出せるように、複製したものを使う(解放はResourceManagerが行う)
	modelId_ = ResourceManager::GetInstance().LoadModelDuplicate(
		ResourceManager::SRC::ENEMY_GHOST);

	// 大きさを設定
	MV1SetScale(modelId_, MODEL_SCALE);
}

void EnemyGhost::Draw(void)
{
	// モデルを敵の位置に置いて描画
	VECTOR drawPos_ = pos_;
	drawPos_.y += MODEL_OFFSET_Y;
	MV1SetPosition(modelId_, drawPos_);
	MV1DrawModel(modelId_);
}