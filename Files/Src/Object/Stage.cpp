#include "Stage.h"
#include "../Application.h"
#include "../Manager/ResourceManager.h"

Stage::Stage(void)
{
	modelId_ = -1;
}

Stage::~Stage(void)
{
}

void Stage::Init(void)
{
	// ステージモデルの読み込み(ResourceManagerが持っているものを使う)
	modelId_ = ResourceManager::GetInstance().Load(
		ResourceManager::SRC::STAGE).handleId_;

	// 大きさと位置を決める(当たり判定を作る前に行う)
	MV1SetScale(modelId_, VGet(SCALE, SCALE, SCALE));
	MV1SetPosition(modelId_, POS);

	// 光が当たらない面も真っ黒にならないように、環境光の色を上げる
	int materialNum_ = MV1GetMaterialNum(modelId_);
	for (int i_ = 0; i_ < materialNum_; i_++)
	{
		MV1SetMaterialAmbColor(modelId_, i_,
			GetColorF(MATERIAL_AMBIENT, MATERIAL_AMBIENT, MATERIAL_AMBIENT, MATERIAL_ALPHA));
	}

	// 当たり判定用の情報を作る
	MV1SetupCollInfo(modelId_, BodyCollision::ALL_FRAME,
		COLL_DIV_X, COLL_DIV_Y, COLL_DIV_Z);
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