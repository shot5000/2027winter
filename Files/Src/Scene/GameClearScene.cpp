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

	// スペースキーでショップへ
	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_SPACE))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::SHOP);
		return;
	}

}

void GameClearScene::Draw(void)
{
	// 仮の表示
	DrawString(0, 100, "GameClearScene", 0xffffff);

	DrawString(0, 130, "SPACE：ショップ", 0xffffff);

	// 今回獲得したお金と、持ち越す所持金
	DrawFormatString(
		MONEY_POS_X, MONEY_POS_Y, MONEY_COLOR,
		MONEY_TEXT, GameResult::GetEarned());
	DrawFormatString(
		MONEY_POS_X, TOTAL_POS_Y, MONEY_COLOR,
		TOTAL_TEXT, GameResult::GetMoney());
}

void GameClearScene::Release(void)
{
}