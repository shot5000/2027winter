#include "Stage.h"
#include "../Application.h"

Stage::Stage(void)
{
}

Stage::~Stage(void)
{
}

void Stage::Init(void)
{
	// ステージモデルの読み込み
	modelId_ = MV1LoadModel(
		(Application::PATH_MODEL + "Stage.mv1").c_str());

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
	// ステージモデルの削除
	MV1DeleteModel(modelId_);

	// 当たり判定用の情報を破棄
	MV1TerminateCollInfo(modelId_, BodyCollision::ALL_FRAME);
}

int Stage::GetModelId(void) const
{
	return modelId_;
}