#include <DxLib.h>
#include "../Manager/Camera.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Object/Grid.h"
#include "../Object/Stage.h"
#include "../Object/Player.h"
#include "GameScene.h"

GameScene::GameScene(void) : SceneBase()
{
	grid_ = nullptr;
	stage_ = nullptr;
	player_ = nullptr;
	dirtManager_ = nullptr;
	isClear_ = false;
	enemyManager_ = nullptr;
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	// クリアへの移行をまだ始めていない状態にする
	isClear_ = false;

	// カメラモード変更
	Camera* camera_ = SceneManager::GetInstance().GetCamera();
	camera_->ChangeMode(Camera::MODE::FREE);
	// カメラを最初の位置に戻す(前のゲームの位置を引き継がない)
	camera_->ResetState();

	// グリッド初期化
	grid_ = new Grid();
	grid_->Init();

	// ステージ初期化
	stage_ = new Stage();
	stage_->Init();

	// プレイヤー初期化
	player_ = new Player();
	player_->Init();

	// 汚れの生成
	dirtManager_ = new DirtManager();
	dirtManager_->Init();

	// 敵の生成
	enemyManager_ = new EnemyManager();
	enemyManager_->Init();

	// 当たり判定にステージのモデルを教える(カメラも敵も共通)
	BodyCollision::SetStageModel(stage_->GetModelId());
}

void GameScene::Update(void)
{

	InputManager& input_ = InputManager::GetInstance();

	// Zキーでクリアへ
	if (input_.IsTrgDown(KEY_INPUT_Z))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME_CLEAR);
		return;
	}

	// Xキーでゲームオーバーへ
	if (input_.IsTrgDown(KEY_INPUT_X))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME_OVER);
		return;
	}

	// グリッド更新
	grid_->Update();

	// ステージ更新
	stage_->Update();

	// プレイヤー更新
	player_->Update();

	// 汚れの掃除
	dirtManager_->Update();

	// 全部掃除したら、ゲームクリアへ
	if (dirtManager_->IsAllCleaned() && !isClear_)
	{
		isClear_ = true;
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME_CLEAR);
		return;
	}

	// 敵の更新(プレイヤーの位置を渡す)
	Camera* camera_ = SceneManager::GetInstance().GetCamera();
	enemyManager_->Update(camera_->GetPos());

	// 敵に触れたら、ゲームオーバーへ
	if (enemyManager_->IsHitPlayer(camera_->GetBody(), camera_->GetPos()))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME_OVER);
		return;
	}
}

void GameScene::Draw(void)
{

	// グリッド描画
	grid_->Draw();

	// ステージ描画
	stage_->Draw();

	// プレイヤー描画
	player_->Draw();

	// 汚れの描画
	dirtManager_->Draw();

	// 敵の描画
	enemyManager_->Draw();

	// 仮の表示
	DrawString(0, 100, "GameScene", 0xffffff);

	// 掃除した数の表示
	DrawCleanCount();

	// 照準は最後に描いて、手前に表示する
	DrawCrosshair();
	DrawPrompt();
}

void GameScene::Release(void)
{
	// グリッド解放
	grid_->Release();
	delete grid_;

	// ステージ解放
	stage_->Release();
	delete stage_;

	// プレイヤー解放
	player_->Release();
	delete player_;

	// 汚れ解放
	delete dirtManager_;

	// 敵解放
	enemyManager_->Release();
	delete enemyManager_;

	// ステージを消す前に、当たり判定から外す
	BodyCollision::SetStageModel(BodyCollision::NO_MODEL);

	// ゲームシーンの外では、カメラを自由に動かさない
	SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::FIXED_POINT);
}

void GameScene::DrawCrosshair(void)
{
	// 画面の中心を求める
	int screenW_ = 0;
	int screenH_ = 0;
	GetDrawScreenSize(&screenW_, &screenH_);
	int centerX_ = screenW_ / CENTER_DIV;
	int centerY_ = screenH_ / CENTER_DIV;

	// 中心に塗りつぶしの円(点)を描く
	DrawCircle(centerX_, centerY_, CROSSHAIR_RADIUS, CROSSHAIR_COLOR, TRUE);
}

void GameScene::DrawPrompt(void)
{
	// 汚れを狙っていないときは出さない
	if (!dirtManager_->IsAiming())
	{
		return;
	}

	// 画面の中心
	int screenW_ = 0;
	int screenH_ = 0;
	GetDrawScreenSize(&screenW_, &screenH_);
	int centerX_ = screenW_ / CENTER_DIV;
	int centerY_ = screenH_ / CENTER_DIV;

	// 文字の幅を測って、左右中央にそろえる
	int textW_ = GetDrawStringWidth(PROMPT_TEXT, static_cast<int>(strlen(PROMPT_TEXT)));
	int drawX_ = centerX_ - textW_ / CENTER_DIV;

	// 照準の少し下に表示
	DrawString(drawX_, centerY_ + PROMPT_OFFSET_Y, PROMPT_TEXT, PROMPT_COLOR);
}

void GameScene::DrawCleanCount(void)
{
	// 画面左の、縦の真ん中に出す
	int screenW_ = 0;
	int screenH_ = 0;
	GetDrawScreenSize(&screenW_, &screenH_);
	int posY_ = screenH_ / CENTER_DIV;

	DrawFormatString(
		COUNT_POS_X, posY_, COUNT_COLOR,
		COUNT_TEXT,
		dirtManager_->GetCleanCount(), DirtManager::DIRT_COUNT
	);
}
