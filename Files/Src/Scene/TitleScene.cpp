#include <cstring>
#include <DxLib.h>
#include "../Application.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "TitleScene.h"

TitleScene::TitleScene(void) : SceneBase()
{
	page_ = PAGE::TITLE;
	menuIndex_ = EXIT_DEFAULT_INDEX;
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
	// 最初はタイトル画面
	page_ = PAGE::TITLE;
	menuIndex_ = EXIT_DEFAULT_INDEX;
}

void TitleScene::Update(void)
{
	// 表示中の画面ごとの更新
	switch (page_)
	{
	case PAGE::TITLE:
		UpdateTitle();
		break;
	case PAGE::EXIT_SELECT:
		UpdateExitMenu();
		break;
	}
}

void TitleScene::Draw(void)
{
	// 仮の表示
	DrawString(0, 100, "TitleScene", NORMAL_COLOR);
	DrawString(0, HINT_POS_Y, HINT_TEXT, NORMAL_COLOR);

	// 終了確認は、タイトルの表示の上に重ねて出す
	if (page_ == PAGE::EXIT_SELECT)
	{
		DrawExitMenu();
	}
}

void TitleScene::Release(void)
{
}

void TitleScene::UpdateTitle(void)
{
	InputManager& input_ = InputManager::GetInstance();

	// スペースキーでゲームへ
	if (input_.IsTrgDown(KEY_INPUT_SPACE))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
		return;
	}

	// ESCキーで、終了確認へ
	if (input_.IsTrgDown(KEY_INPUT_ESCAPE))
	{
		page_ = PAGE::EXIT_SELECT;
		menuIndex_ = EXIT_DEFAULT_INDEX;
	}
}

void TitleScene::UpdateExitMenu(void)
{
	InputManager& input_ = InputManager::GetInstance();

	// カーソル移動(上下で、端から端へ回る)
	if (input_.IsTrgDown(KEY_INPUT_UP) || input_.IsTrgDown(KEY_INPUT_W))
	{
		menuIndex_ = (menuIndex_ + EXIT_MENU_COUNT - 1) % EXIT_MENU_COUNT;
	}
	if (input_.IsTrgDown(KEY_INPUT_DOWN) || input_.IsTrgDown(KEY_INPUT_S))
	{
		menuIndex_ = (menuIndex_ + 1) % EXIT_MENU_COUNT;
	}

	// 決定(EnterかSpace)
	if (input_.IsTrgDown(KEY_INPUT_RETURN) || input_.IsTrgDown(KEY_INPUT_SPACE))
	{
		switch (static_cast<EXIT_MENU>(menuIndex_))
		{
		case EXIT_MENU::YES:
			// ゲームを終了する
			Application::GetInstance().Exit();
			break;
		case EXIT_MENU::NO:
			// タイトル画面に戻る
			page_ = PAGE::TITLE;
			break;
		}
		return;
	}

	// もう一度ESCを押したら、タイトル画面に戻る
	if (input_.IsTrgDown(KEY_INPUT_ESCAPE))
	{
		page_ = PAGE::TITLE;
	}
}

void TitleScene::DrawExitMenu(void)
{
	// 画面の中心
	int screenW_ = 0;
	int screenH_ = 0;
	GetDrawScreenSize(&screenW_, &screenH_);
	int centerX_ = screenW_ / CENTER_DIV;
	int centerY_ = screenH_ / CENTER_DIV;

	// 問いかけの文字
	DrawCenterString(
		centerX_, centerY_ + EXIT_TEXT_OFFSET_Y, EXIT_TEXT, NORMAL_COLOR);

	// 選択肢(選んでいるものだけ強調色にする)
	for (int i_ = 0; i_ < EXIT_MENU_COUNT; i_++)
	{
		unsigned int color_ = NORMAL_COLOR;
		if (i_ == menuIndex_)
		{
			color_ = HIGHLIGHT_COLOR;
		}

		DrawCenterString(
			centerX_,
			centerY_ + EXIT_MENU_OFFSET_Y + EXIT_MENU_ADD_Y * i_,
			EXIT_MENU_TEXT[i_], color_);
	}
}

void TitleScene::DrawCenterString(
	int centerX, int posY, const char* text, unsigned int color) const
{
	// 文字の幅を測って、左右中央にそろえる
	int textW_ = GetDrawStringWidth(text, static_cast<int>(strlen(text)));
	DrawString(centerX - textW_ / CENTER_DIV, posY, text, color);
}