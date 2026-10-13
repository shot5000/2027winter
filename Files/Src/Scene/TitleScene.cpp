#include <DxLib.h>
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "TitleScene.h"

TitleScene::TitleScene(void) : SceneBase()
{
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
}

void TitleScene::Update(void)
{

	// スペースキーでゲームへ
	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_SPACE))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
		return;
	}

}

void TitleScene::Draw(void)
{
	// 仮の表示
	DrawString(0, 100, "TitleScene", 0xffffff);
}

void TitleScene::Release(void)
{
}