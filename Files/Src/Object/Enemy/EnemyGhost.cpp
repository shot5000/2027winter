#include "../../Manager/ParamManager.h"
#include "../../Manager/ResourceManager.h"
#include "EnemyGhost.h"

EnemyGhost::EnemyGhost(const VECTOR& pos, float angleY)
	: EnemyBase(pos, BODY_PARAM, angleY)
{
	modelId_ = Resource::INVALID_HANDLE;

	// JSONで決めた、体力・速さ・攻撃力を反映する
	const EnemyParam& param_ = ParamManager::GetInstance().GetEnemy(PARAM_ID);
	ApplyParam(param_);

	// このモデルの頭の向きと、JSONで決めた索敵の大きさを使う
	frontOffset_ = FRONT_ANGLE_OFFSET;
	EnemySensor::PARAM sensorParam_ = {
		param_.fanRange_,
		param_.fanAngleDeg_ * EnemyParam::DEG_TO_RAD,
		param_.chaseRange_,
		SEARCH_ORIGIN_DIST
	};
	sensor_.Init(sensorParam_);
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
	// モデルを敵の位置に置いて、向きに合わせて回す
	VECTOR drawPos_ = pos_;
	drawPos_.y += MODEL_OFFSET_Y;
	MV1SetPosition(modelId_, drawPos_);
	MV1SetRotationXYZ(modelId_, VGet(0.0f, angleY_, 0.0f));

	// 怯んでいる間は、赤と通常を交互にする
	MV1SetDifColorScale(modelId_, GetColorScale());
	MV1DrawModel(modelId_);

	// 索敵範囲(検知前は扇、検知中は円)
	DrawSensor();
}

void EnemyGhost::Release(void)
{
	EnemyBase::Release();

	// 倒れたときや、ゲームが終わるときに、複製したモデルを消す
	ResourceManager::GetInstance().DeleteModelDuplicate(
		ResourceManager::SRC::ENEMY_GHOST, modelId_);
	modelId_ = Resource::INVALID_HANDLE;
}