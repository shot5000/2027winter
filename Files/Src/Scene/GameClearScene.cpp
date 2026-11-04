#include <DxLib.h>
#include "../Common/GameResult.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "GameClearScene.h"

GameClearScene::GameClearScene(void) : SceneBase()
{
}

GameClearScene::~GameClearScene(void)
{
}

void GameClearScene::Init(void)
{
}

void GameClearScene::Update(void)
{

	// スペースキーでタイトルへ
	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_SPACE))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
		return;
	}

}

void GameClearScene::Draw(void)
{
	// 仮の表示
	DrawString(0, 100, "GameClearScene", 0xffffff);

	DrawString(0, 130, "SPACE：タイトル画面", 0xffffff);

	// 獲得したお金
	DrawFormatString(
		MONEY_POS_X, MONEY_POS_Y, MONEY_COLOR,
		MONEY_TEXT, GameResult::GetMoney());
}

void GameClearScene::Release(void)
{
}