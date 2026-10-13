#include <DxLib.h>
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
}

void GameClearScene::Release(void)
{
}