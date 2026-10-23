#include <cstring>
#include <DxLib.h>
#include "../Manager/InputManager.h"
#include "PauseScene.h"

PauseScene::PauseScene(const std::string& question)
{
	question_ = question;
	isOpen_ = false;
	index_ = DEFAULT_INDEX;
}

PauseScene::~PauseScene(void)
{
}

void PauseScene::Open(void)
{
	isOpen_ = true;
	index_ = DEFAULT_INDEX;
}

bool PauseScene::IsOpen(void) const
{
	return isOpen_;
}

PauseScene::RESULT PauseScene::Update(void)
{
	if (!isOpen_)
	{
		return RESULT::NONE;
	}

	InputManager& input_ = InputManager::GetInstance();

	// カーソル移動(上下で、端から端へ回る)
	if (input_.IsTrgDown(KEY_INPUT_UP) || input_.IsTrgDown(KEY_INPUT_W))
	{
		index_ = (index_ + CHOICE_COUNT - 1) % CHOICE_COUNT;
	}
	if (input_.IsTrgDown(KEY_INPUT_DOWN) || input_.IsTrgDown(KEY_INPUT_S))
	{
		index_ = (index_ + 1) % CHOICE_COUNT;
	}

	// 決定(EnterかSpace)
	if (input_.IsTrgDown(KEY_INPUT_RETURN) || input_.IsTrgDown(KEY_INPUT_SPACE))
	{
		isOpen_ = false;
		if (index_ == static_cast<int>(CHOICE::YES))
		{
			return RESULT::YES;
		}
		return RESULT::NO;
	}

	// ESCでやめる(「いいえ」と同じ)
	if (input_.IsTrgDown(KEY_INPUT_ESCAPE))
	{
		isOpen_ = false;
		return RESULT::NO;
	}

	return RESULT::NONE;
}

void PauseScene::Draw(void) const
{
	if (!isOpen_)
	{
		return;
	}

	// 画面の中心
	int screenW_ = 0;
	int screenH_ = 0;
	GetDrawScreenSize(&screenW_, &screenH_);
	int centerX_ = screenW_ / CENTER_DIV;
	int centerY_ = screenH_ / CENTER_DIV;

	// 文字が読みやすいように、後ろを暗くする
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, DIM_ALPHA);
	DrawBox(0, 0, screenW_, screenH_, DIM_COLOR, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// 問いかけの文字
	DrawCenterString(
		centerX_, centerY_ + QUESTION_OFFSET_Y, question_.c_str(), NORMAL_COLOR);

	// 選択肢(選んでいるものだけ強調色にする)
	for (int i_ = 0; i_ < CHOICE_COUNT; i_++)
	{
		unsigned int color_ = NORMAL_COLOR;
		if (i_ == index_)
		{
			color_ = HIGHLIGHT_COLOR;
		}

		DrawCenterString(
			centerX_,
			centerY_ + CHOICE_OFFSET_Y + CHOICE_ADD_Y * i_,
			CHOICE_TEXT[i_], color_);
	}
}

void PauseScene::DrawCenterString(
	int centerX, int posY, const char* text, unsigned int color) const
{
	// 文字の幅を測って、左右中央にそろえる
	int textW_ = GetDrawStringWidth(text, static_cast<int>(strlen(text)));
	DrawString(centerX - textW_ / CENTER_DIV, posY, text, color);
}