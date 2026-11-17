#include <cmath>
#include <cstdio>
#include <cstring>
#include <vector>
#include <DxLib.h>
#include "../Common/GameResult.h"
#include "../Manager/Camera.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ParamManager.h"
#include "../Object/Collision/BodyCollision.h"
#include "../Object/Grid.h"
#include "../Object/Stage/Stage.h"
#include "../Object/Player.h"
#include "GameScene.h"

GameScene::GameScene(void) : SceneBase(), backMenu_(BACK_TEXT)
{
	grid_ = nullptr;
	stage_ = nullptr;
	player_ = nullptr;
	dirtManager_ = nullptr;
	enemyManager_ = nullptr;
	timeLeft_ = TIME_LIMIT;
	aimedDoor_ = -1;
	preTime_ = 0;
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	// 敵とアイテムの性能(JSON)を、最新の内容に読み直す(ゲームを始めるたびに)
	ParamManager::GetInstance().Load();

	// 前のゲームのお金を消して、制限時間を最初に戻す
	GameResult::Reset();
	timeLeft_ = TIME_LIMIT;

	// カメラを操作できるモードにして、最初の位置に戻す
	Camera* camera_ = SceneManager::GetInstance().GetCamera();
	camera_->ChangeMode(Camera::MODE::FREE);
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

	// アイテムの初期化
	itemManager_.Init();

	// ドアの初期化
	goal_.Init();

	// 当たり判定にステージのモデルを教える(カメラも敵も共通)
	BodyCollision::SetStageModel(stage_->GetModelId());

	// 扉(ステージのモデルが決まってから)
	doorMng_.Init(stage_->GetModelId());
	aimedDoor_ = -1;
	preTime_ = GetNowHiPerformanceCount();
}

void GameScene::Update(void)
{

	InputManager& input_ = InputManager::GetInstance();

	// 確認の選択肢が出ている間は、ゲームを止めて、選択肢だけ動かす
	if (backMenu_.IsOpen())
	{
		UpdateBackMenu();
		return;
	}

	// ESCキーで、タイトルに戻るかの確認を出す
	if (input_.IsTrgDown(KEY_INPUT_ESCAPE))
	{
		backMenu_.Open();

		// カメラの操作を止めて、マウスカーソルを表示する
		SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::FIXED_POINT);
		return;
	}

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

	// 制限時間を進めて、時間切れになったら、クリアへ
	timeLeft_ -= SceneManager::GetInstance().GetDeltaTime();
	if (timeLeft_ <= 0.0f)
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME_CLEAR);
		return;
	}

	// グリッド更新
	grid_->Update();

	// ステージ更新
	stage_->Update();

	// プレイヤー更新
	player_->Update();

	// アイテムの更新(スロットの選択、左クリックでの攻撃)
	itemManager_.Update();

	// 持っているアイテムによって、歩く速さを変える
	SceneManager::GetInstance().GetCamera()->SetMoveSpeedRate(
		itemManager_.GetMoveSpeedRate());

	// 汚れの掃除(掃除できるアイテムを持っているとき、Eの長押し)。消したらお金を得る
	int reward_ = dirtManager_->Update(
		itemManager_.CanClean(), itemManager_.GetCleanTime(),
		itemManager_.GetCleanReach());
	GameResult::AddMoney(reward_);

	// ドアをEの長押しで開けたら、クリアへ
	if (goal_.Update())
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME_CLEAR);
		return;
	}

	// 敵の更新(プレイヤーの位置を渡す)
	Camera* camera_ = SceneManager::GetInstance().GetCamera();
	enemyManager_->Update(camera_->GetPos());

	// 扉の更新
	UpdateDoor();

	// 攻撃に当たった敵に、ダメージを与える
	enemyManager_->DamageHitEnemies(itemManager_);

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

	// ドアの描画
	goal_.Draw();

	// 敵の描画
	enemyManager_->Draw();

	// 攻撃の描画
	itemManager_.DrawAttack();

	// 仮の表示
	DrawString(0, 100, "GameScene", 0xffffff);

	// 掃除した数、所持金、残り時間の表示
	DrawCleanCount();
	DrawMoney();
	DrawTime();

	// アイテムスロット
	itemManager_.DrawSlot();

	// 照準と操作表示は、ゲームが動いているときだけ(最後に描いて手前に出す)
	if (!backMenu_.IsOpen())
	{
		DrawCrosshair();
		DrawPrompt();
		DrawDoorPrompt();
	}

	// 確認の選択肢(出ているときだけ描かれる。いちばん手前)
	backMenu_.Draw();
}

void GameScene::Release(void)
{
	// グリッド解放
	grid_->Release();
	delete grid_;

	// 扉(ステージを消す前に)
	doorMng_.Release();

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

	// アイテム解放
	itemManager_.Release();

	// ステージを消した後に、当たり判定から外す
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
	// ドアを狙っているときは、ドアを開ける案内(どのアイテムでも開けられる)
	if (goal_.IsAiming())
	{
		DrawPromptText(GOAL_PROMPT_TEXT);
		DrawGoalGauge();
		return;
	}

	// 掃除できるアイテムを持っていて、汚れを狙っているときは、掃除の案内
	if (itemManager_.CanClean() && dirtManager_->IsAiming(itemManager_.GetCleanReach()))
	{
		DrawPromptText(PROMPT_TEXT);
	}
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

void GameScene::DrawMoney(void)
{
	// 所持金の文字を作る
	char text_[TEXT_BUF_SIZE];
	snprintf(text_, sizeof(text_), MONEY_TEXT, GameResult::GetMoney());

	// 文字の幅を測って、画面の右上にそろえる
	int screenW_ = 0;
	int screenH_ = 0;
	GetDrawScreenSize(&screenW_, &screenH_);
	int textW_ = GetDrawStringWidth(text_, static_cast<int>(strlen(text_)));
	DrawString(screenW_ - MONEY_MARGIN_X - textW_, MONEY_POS_Y, text_, MONEY_COLOR);
}

void GameScene::DrawTime(void)
{
	if (!IS_DRAW_TIME)
	{
		return;
	}

	// 残りの秒数を、分と秒に分ける(端数は切り上げる)
	int totalSec_ = static_cast<int>(ceilf(timeLeft_));
	char text_[TEXT_BUF_SIZE];
	snprintf(text_, sizeof(text_), TIME_TEXT, totalSec_ / SEC_PER_MIN, totalSec_ % SEC_PER_MIN);

	// 文字の幅を測って、画面の上の中央にそろえる
	int screenW_ = 0;
	int screenH_ = 0;
	GetDrawScreenSize(&screenW_, &screenH_);
	int textW_ = GetDrawStringWidth(text_, static_cast<int>(strlen(text_)));
	DrawString((screenW_ - textW_) / CENTER_DIV, TIME_POS_Y, text_, TIME_COLOR);
}

void GameScene::UpdateDoor(void)
{
	// 経過時間
	LONGLONG nowTime_ = GetNowHiPerformanceCount();
	float deltaTime_ = static_cast<float>(nowTime_ - preTime_) / MICRO_SEC_TO_SEC;
	preTime_ = nowTime_;
	if (deltaTime_ > DELTA_TIME_MAX)
	{
		deltaTime_ = DELTA_TIME_MAX;
	}

	Camera* camera_ = SceneManager::GetInstance().GetCamera();

	// 扉の近くにいる人(閉めるときに挟まないように)
	std::vector<DoorManager::OCCUPANT> occupants_;
	occupants_.push_back({ &camera_->GetBody(), camera_->GetPos() });
	doorMng_.Update(deltaTime_, occupants_);

	// 視線の向き(カメラの角度から作る)
	const VECTOR& angles_ = camera_->GetAngles();
	MATRIX mat_ = MMult(MGetRotX(angles_.x), MGetRotY(angles_.y));
	VECTOR dir_ = VTransform(VGet(0.0f, 0.0f, 1.0f), mat_);

	// 汚れやゴールを狙っているときは、そちらを優先する
	aimedDoor_ = -1;
	bool isAimDirt_ = itemManager_.CanClean()
		&& dirtManager_->IsAiming(itemManager_.GetCleanReach());
	if (isAimDirt_ || goal_.IsAiming())
	{
		return;
	}
	aimedDoor_ = doorMng_.FindAimed(camera_->GetPos(), dir_);

	// Eキーで開け閉め
	if (aimedDoor_ >= 0 && InputManager::GetInstance().IsTrgDown(KEY_INPUT_E))
	{
		doorMng_.Toggle(aimedDoor_);
	}
}

void GameScene::DrawDoorPrompt(void)
{
	if (aimedDoor_ < 0)
	{
		return;
	}

	// 画面の中心に文字をそろえて、照準の少し下に出す
	int screenW_ = 0;
	int screenH_ = 0;
	GetDrawScreenSize(&screenW_, &screenH_);
	int centerX_ = screenW_ / CENTER_DIV;
	int centerY_ = screenH_ / CENTER_DIV;

	int textW_ = GetDrawStringWidth(
		DOOR_PROMPT_TEXT, static_cast<int>(strlen(DOOR_PROMPT_TEXT)));
	DrawString(centerX_ - textW_ / CENTER_DIV, centerY_ + PROMPT_OFFSET_Y,
		DOOR_PROMPT_TEXT, PROMPT_COLOR);
}

void GameScene::UpdateBackMenu(void)
{
	switch (backMenu_.Update())
	{
	case PauseScene::RESULT::YES:
		// タイトルへ
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
		break;
	case PauseScene::RESULT::NO:
		// ゲームに戻る(カメラの操作を再開する)
		SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::FREE);
		break;
	default:
		break;
	}
}

void GameScene::DrawPromptText(const char* text)
{
	// 画面の中心
	int screenW_ = 0;
	int screenH_ = 0;
	GetDrawScreenSize(&screenW_, &screenH_);
	int centerX_ = screenW_ / CENTER_DIV;
	int centerY_ = screenH_ / CENTER_DIV;

	// 文字の幅を測って、左右中央にそろえる
	int textW_ = GetDrawStringWidth(text, static_cast<int>(strlen(text)));
	int drawX_ = centerX_ - textW_ / CENTER_DIV;

	// 照準の少し下に表示
	DrawString(drawX_, centerY_ + PROMPT_OFFSET_Y, text, PROMPT_COLOR);
}

void GameScene::DrawGoalGauge(void)
{
	// 長押しを始めていないときは出さない
	float rate_ = goal_.GetHoldRate();
	if (rate_ <= 0.0f)
	{
		return;
	}

	// 操作表示の下に、横長のゲージを出す
	int screenW_ = 0;
	int screenH_ = 0;
	GetDrawScreenSize(&screenW_, &screenH_);
	int centerX_ = screenW_ / CENTER_DIV;
	int centerY_ = screenH_ / CENTER_DIV;
	int left_ = centerX_ - GAUGE_W / CENTER_DIV;
	int top_ = centerY_ + PROMPT_OFFSET_Y + GAUGE_OFFSET_Y;

	DrawBox(left_, top_, left_ + GAUGE_W, top_ + GAUGE_H, GAUGE_BACK_COLOR, TRUE);
	DrawBox(
		left_, top_,
		left_ + static_cast<int>(GAUGE_W * rate_), top_ + GAUGE_H,
		GAUGE_COLOR, TRUE);
}