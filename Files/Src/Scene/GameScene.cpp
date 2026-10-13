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
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{

	// カメラモード変更
	Camera* camera_ = SceneManager::GetInstance().GetCamera();
	camera_->ChangeMode(Camera::MODE::FREE);

	// グリッド初期化
	grid_ = new Grid();
	grid_->Init();

	// ステージ初期化
	stage_ = new Stage();
	stage_->Init();

	// プレイヤー初期化
	player_ = new Player();
	player_->Init();

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

}

void GameScene::Draw(void)
{

	// グリッド描画
	grid_->Draw();

	// ステージ描画
	stage_->Draw();

	// プレイヤー描画
	player_->Draw();

	// 仮の表示
	DrawString(0, 100, "GameScene", 0xffffff);

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

}