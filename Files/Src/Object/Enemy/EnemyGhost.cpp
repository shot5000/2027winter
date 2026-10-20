#include "../../Manager/ResourceManager.h"
#include "EnemyGhost.h"

EnemyGhost::EnemyGhost(const VECTOR& pos, float angleY)
	: EnemyBase(pos, BODY_PARAM, angleY)
{
	modelId_ = Resource::INVALID_HANDLE;

	// このモデルの頭の向きと、索敵の大きさを決める
	frontOffset_ = FRONT_ANGLE_OFFSET;
	sensor_.Init(SENSOR_PARAM);
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
	MV1DrawModel(modelId_);

	// 索敵範囲(検知前は扇、検知中は円)
	DrawSensor();
}

void EnemyGhost::UpdateChase(const VECTOR& targetPos)
{
	// プレイヤーのほうへ真っすぐ向かう
	ChaseStraight(targetPos, CHASE_SPEED);
}