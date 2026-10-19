#include "Stage.h"
#include "../Application.h"
#include "../Manager/ResourceManager.h"

Stage::Stage(void)
{
}

Stage::~Stage(void)
{
}

void Stage::Init(void)
{
	// ステージモデルの読み込み(ResourceManagerが持っているものを使う)
	modelId_ = ResourceManager::GetInstance().Load(
		ResourceManager::SRC::STAGE).handleId_;

	// ステージモデルの位置
	pos_ = VGet(0.0f, 80.0f, 0.0f);

	// ステージモデルの位置設定
	MV1SetPosition(modelId_, pos_);

	// 当たり判定用の情報を作る
	MV1SetupCollInfo(modelId_, BodyCollision::ALL_FRAME);
}

void Stage::Update(void)
{
}

void Stage::Draw(void)
{
	// ステージモデルの描画
	MV1DrawModel(modelId_);
}

void Stage::Release(void)
{
	// 当たり判定用の情報を破棄
	MV1TerminateCollInfo(modelId_, BodyCollision::ALL_FRAME);

	// ゲームのたびに読み込み直すため、モデルを解放する
	ResourceManager::GetInstance().Unload(ResourceManager::SRC::STAGE);
}

int Stage::GetModelId(void) const
{
	return modelId_;
}