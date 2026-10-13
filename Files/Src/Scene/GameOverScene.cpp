#include <DxLib.h>
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "GameOverScene.h"

GameOverScene::GameOverScene(void) : SceneBase()
{
}

GameOverScene::~GameOverScene(void)
{
}

void GameOverScene::Init(void)
{
}

void GameOverScene::Update(void)
{

	// スペースキーでタイトルへ
	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_SPACE))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
		return;
	}

}

void GameOverScene::Draw(void)
{
	// 仮の表示
	DrawString(0, 100, "GameOverScene", 0xffffff);
}

void GameOverScene::Release(void)
{
}