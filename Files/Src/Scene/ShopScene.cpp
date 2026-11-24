#include <cstdio>
#include <cstring>
#include <DxLib.h>
#include "../Common/GameResult.h"
#include "../Manager/Camera.h"
#include "../Manager/InputManager.h"
#include "../Manager/ParamManager.h"
#include "../Manager/SceneManager.h"
#include "ShopScene.h"

ShopScene::ShopScene(void) : SceneBase()
{
	selected_ = 0;
	isPrevClick_ = false;
	prevMouseX_ = 0;
	prevMouseY_ = 0;
	message_ = nullptr;
	messageTimer_ = 0.0f;
}

ShopScene::~ShopScene(void)
{
}

void ShopScene::Init(void)
{
	// アイテムの性能(JSON)を、最新の内容に読み直す
	ParamManager::GetInstance().Load();

	// マウスカーソルを出す
	SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::FIXED_POINT);

	CreateSlots();
	selected_ = 0;
	message_ = nullptr;
	messageTimer_ = 0.0f;

	// 前の画面で押していたクリックで、買ってしまわないようにする
	isPrevClick_ = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;
	GetMousePoint(&prevMouseX_, &prevMouseY_);
}

void ShopScene::Update(void)
{
	// メッセージを出す残り時間を進める
	if (messageTimer_ > 0.0f)
	{
		messageTimer_ -= SceneManager::GetInstance().GetDeltaTime();
	}

	UpdateMouse();
	UpdateKey();
}

void ShopScene::Draw(void)
{
	// 背景
	int screenW_ = 0;
	int screenH_ = 0;
	GetDrawScreenSize(&screenW_, &screenH_);
	DrawBox(0, 0, screenW_, screenH_, BACK_COLOR, TRUE);

	// タイトル
	AREA titleArea_ = { 0, TITLE_Y, screenW_, LINE_H };
	DrawCenterString(titleArea_, TITLE_TEXT, TEXT_COLOR);

	// メッセージ(時間の間だけ)
	if (message_ != nullptr && messageTimer_ > 0.0f)
	{
		AREA msgArea_ = { 0, MESSAGE_Y, screenW_, LINE_H };
		DrawCenterString(msgArea_, message_, MESSAGE_COLOR);
	}

	// 所持金(右上)
	char moneyText_[TEXT_BUF_SIZE];
	snprintf(moneyText_, sizeof(moneyText_), MONEY_TEXT, GameResult::GetMoney());
	int moneyW_ = GetDrawStringWidth(moneyText_, static_cast<int>(strlen(moneyText_)));
	DrawString(screenW_ - MARGIN_X - moneyW_, TITLE_Y, moneyText_, MONEY_COLOR);

	// アイテム
	for (int i_ = 0; i_ < static_cast<int>(slots_.size()); i_++)
	{
		DrawCell(i_);
	}

	// ゲームを始めるボタンと、操作の説明
	DrawButton();
	AREA button_ = GetButtonArea();
	AREA helpArea_ = { 0, button_.y_ + button_.h_ + HELP_GAP, screenW_, LINE_H };
	DrawCenterString(helpArea_, HELP_TEXT, TEXT_COLOR);
}

void ShopScene::Release(void)
{
	slots_.clear();
}

void ShopScene::UpdateMouse(void)
{
	int mouseX_ = 0;
	int mouseY_ = 0;
	GetMousePoint(&mouseX_, &mouseY_);

	// マウスが動いたときだけ、指しているマスを選ぶ(キー操作を邪魔しないため)
	bool isMoved_ = mouseX_ != prevMouseX_ || mouseY_ != prevMouseY_;
	prevMouseX_ = mouseX_;
	prevMouseY_ = mouseY_;

	// マウスが指している場所
	int pointed_ = -1;
	for (int i_ = 0; i_ < static_cast<int>(slots_.size()); i_++)
	{
		if (IsInside(GetCellArea(i_), mouseX_, mouseY_))
		{
			pointed_ = i_;
		}
	}
	if (IsInside(GetButtonArea(), mouseX_, mouseY_))
	{
		pointed_ = START_INDEX;
	}

	if (isMoved_ && pointed_ >= 0)
	{
		selected_ = pointed_;
	}

	// 左クリックを押した瞬間に、指している場所を決定する
	bool isClick_ = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;
	bool isTrg_ = isClick_ && !isPrevClick_;
	isPrevClick_ = isClick_;
	if (isTrg_ && pointed_ >= 0)
	{
		selected_ = pointed_;
		Decide();
	}
}

void ShopScene::UpdateKey(void)
{
	InputManager& input_ = InputManager::GetInstance();

	// 選ぶ(WASDと矢印)
	if (input_.IsTrgDown(KEY_INPUT_W) || input_.IsTrgDown(KEY_INPUT_UP))
	{
		MoveSelect(0, -1);
	}
	if (input_.IsTrgDown(KEY_INPUT_S) || input_.IsTrgDown(KEY_INPUT_DOWN))
	{
		MoveSelect(0, 1);
	}
	if (input_.IsTrgDown(KEY_INPUT_A) || input_.IsTrgDown(KEY_INPUT_LEFT))
	{
		MoveSelect(-1, 0);
	}
	if (input_.IsTrgDown(KEY_INPUT_D) || input_.IsTrgDown(KEY_INPUT_RIGHT))
	{
		MoveSelect(1, 0);
	}

	// 決定
	if (input_.IsTrgDown(KEY_INPUT_RETURN) || input_.IsTrgDown(KEY_INPUT_SPACE))
	{
		Decide();
	}
}

void ShopScene::MoveSelect(int dirX, int dirY)
{
	// ゲームを始めるボタンからは、上に動くと一番下の行の左に戻る
	if (selected_ == START_INDEX)
	{
		if (dirY < 0)
		{
			selected_ = (ROW_COUNT - 1) * COLUMN_COUNT;
		}
		return;
	}

	int col_ = selected_ % COLUMN_COUNT;
	int row_ = selected_ / COLUMN_COUNT;

	// 一番下の行から下に動くと、ゲームを始めるボタンへ
	if (dirY > 0 && row_ == ROW_COUNT - 1)
	{
		selected_ = START_INDEX;
		return;
	}

	col_ += dirX;
	row_ += dirY;
	if (col_ < 0 || col_ >= COLUMN_COUNT || row_ < 0 || row_ >= ROW_COUNT)
	{
		return;
	}
	selected_ = row_ * COLUMN_COUNT + col_;
}

void ShopScene::Decide(void)
{
	if (selected_ == START_INDEX)
	{
		StartGame();
		return;
	}
	Buy(selected_);
}

void ShopScene::Buy(int index)
{
	if (index < 0 || index >= static_cast<int>(slots_.size()))
	{
		return;
	}

	SLOT& slot_ = slots_[index];
	if (slot_.isBought_)
	{
		SetMessage(MSG_SOLD);
		return;
	}

	// お金が足りなければ買えない
	if (!GameResult::SpendMoney(slot_.param_.price_))
	{
		SetMessage(MSG_SHORT);
		return;
	}

	// 購入したアイテムの一覧に足す(昇降口に置く)
	GameResult::AddPurchased(static_cast<int>(STOCK[index]));
	slot_.isBought_ = true;
	SetMessage(MSG_BOUGHT);
}

void ShopScene::StartGame(void)
{
	SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
}

void ShopScene::SetMessage(const char* message)
{
	message_ = message;
	messageTimer_ = MESSAGE_TIME;
}

void ShopScene::CreateSlots(void)
{
	slots_.clear();
	for (int i_ = 0; i_ < ITEM_COUNT; i_++)
	{
		SLOT slot_;
		slot_.def_ = &ItemManager::ITEM_TABLE[static_cast<int>(STOCK[i_])];
		slot_.param_ = ParamManager::GetInstance().GetItem(slot_.def_->paramId_);
		slot_.isBought_ = false;

		// ステータス(増やすときは、ここに足す)
		const ItemParam& p_ = slot_.param_;
		bool isCleanOnly_ = slot_.def_->param_.kind_ == ItemBase::KIND::CLEAN;
		float reachRate_ = isCleanOnly_
			? ToRate(p_.cleanReach_, CLEAN_REACH_MAX)
			: ToRate(p_.attackReach_, ATTACK_REACH_MAX);
		float weightRate_ = ToRate(
			MOVE_RATE_MAX - p_.moveSpeedRate_, MOVE_RATE_MAX - MOVE_RATE_MIN);

		slot_.stats_.push_back({ LABEL_ATTACK, ToRate(
			static_cast<float>(p_.attackPower_), ATTACK_POWER_MAX) });
		slot_.stats_.push_back({ LABEL_REACH, reachRate_ });
		slot_.stats_.push_back({ LABEL_CLEAN, ToRate(
			p_.cleanSpeed_, CLEAN_SPEED_MAX) });
		slot_.stats_.push_back({ LABEL_WEIGHT, weightRate_ });

		slots_.push_back(slot_);
	}
}

float ShopScene::ToRate(float value, float maxValue)
{
	float rate_ = value / maxValue;
	if (rate_ < RATE_MIN)
	{
		rate_ = RATE_MIN;
	}
	if (rate_ > RATE_MAX)
	{
		rate_ = RATE_MAX;
	}
	return rate_;
}

ShopScene::AREA ShopScene::GetCellArea(int index) const
{
	int screenW_ = 0;
	int screenH_ = 0;
	GetDrawScreenSize(&screenW_, &screenH_);

	// 画面の大きさから、マス1つの大きさを決める
	int cellW_ = (screenW_ - MARGIN_X * 2 - CELL_GAP_X * (COLUMN_COUNT - 1)) / COLUMN_COUNT;
	int gridH_ = screenH_ - TOP_AREA - BOTTOM_AREA;
	int cellH_ = (gridH_ - CELL_GAP_Y * (ROW_COUNT - 1)) / ROW_COUNT;

	int col_ = index % COLUMN_COUNT;
	int row_ = index / COLUMN_COUNT;
	AREA area_;
	area_.x_ = MARGIN_X + (cellW_ + CELL_GAP_X) * col_;
	area_.y_ = TOP_AREA + (cellH_ + CELL_GAP_Y) * row_;
	area_.w_ = cellW_;
	area_.h_ = cellH_;
	return area_;
}

ShopScene::AREA ShopScene::GetButtonArea(void) const
{
	int screenW_ = 0;
	int screenH_ = 0;
	GetDrawScreenSize(&screenW_, &screenH_);

	AREA area_;
	area_.w_ = BUTTON_W;
	area_.h_ = BUTTON_H;
	area_.x_ = (screenW_ - BUTTON_W) / CENTER_DIV;
	area_.y_ = screenH_ - BOTTOM_AREA + BUTTON_GAP;
	return area_;
}

bool ShopScene::IsInside(const AREA& area, int x, int y)
{
	return x >= area.x_ && x < area.x_ + area.w_
		&& y >= area.y_ && y < area.y_ + area.h_;
}

void ShopScene::DrawCell(int index) const
{
	const SLOT& slot_ = slots_[index];
	AREA cell_ = GetCellArea(index);

	// 背景
	DrawBox(cell_.x_, cell_.y_, cell_.x_ + cell_.w_, cell_.y_ + cell_.h_, CELL_COLOR, TRUE);

	// アイコン(画像ができるまでは仮の四角)
	AREA icon_ = { cell_.x_ + PADDING, cell_.y_ + PADDING, ICON_SIZE, ICON_SIZE };
	DrawBox(icon_.x_, icon_.y_, icon_.x_ + icon_.w_, icon_.y_ + icon_.h_, ICON_COLOR, TRUE);
	DrawCenterString(icon_, ICON_TEXT, TEXT_COLOR);

	// 名前、値段、説明文
	int textX_ = icon_.x_ + ICON_SIZE + PADDING;
	int textW_ = cell_.x_ + cell_.w_ - PADDING - textX_;
	DrawString(textX_, cell_.y_ + PADDING, slot_.def_->param_.name_, TEXT_COLOR);

	char priceText_[TEXT_BUF_SIZE];
	snprintf(priceText_, sizeof(priceText_), PRICE_TEXT, slot_.param_.price_);
	unsigned int priceColor_ = GameResult::CanAfford(slot_.param_.price_)
		? PRICE_COLOR : SHORT_COLOR;
	DrawString(textX_, cell_.y_ + PADDING + LINE_H, priceText_, priceColor_);

	DrawWrapped(
		slot_.param_.description_, textX_, cell_.y_ + PADDING + LINE_H * 2,
		textW_, TEXT_COLOR);

	// ステータス
	DrawStats(slot_, cell_, textX_, textW_);

	// 購入済みは、暗くして文字を出す
	if (slot_.isBought_)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, SOLD_ALPHA);
		DrawBox(cell_.x_, cell_.y_, cell_.x_ + cell_.w_, cell_.y_ + cell_.h_, 0x000000, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		DrawCenterString(cell_, SOLD_TEXT, TEXT_COLOR);
	}

	DrawFrame(cell_, index == selected_);
}

void ShopScene::DrawStats(
	const SLOT& slot, const AREA& cell, int textX, int textW) const
{
	// マスの下から、ステータスを2列で並べる
	int statCount_ = static_cast<int>(slot.stats_.size());
	int rowCount_ = (statCount_ + STAT_COLUMN_COUNT - 1) / STAT_COLUMN_COUNT;
	int top_ = cell.y_ + cell.h_ - PADDING - rowCount_ * LINE_H;
	int statW_ = textW / STAT_COLUMN_COUNT;
	int gaugeW_ = statW_ - LABEL_W - GAUGE_GAP;

	for (int i_ = 0; i_ < statCount_; i_++)
	{
		int x_ = textX + statW_ * (i_ % STAT_COLUMN_COUNT);
		int y_ = top_ + LINE_H * (i_ / STAT_COLUMN_COUNT);

		DrawString(x_, y_, slot.stats_[i_].label_, TEXT_COLOR);

		// ゲージ(背景の上に、割合だけ重ねる)
		int gaugeX_ = x_ + LABEL_W;
		int gaugeY_ = y_ + (LINE_H - GAUGE_H) / CENTER_DIV;
		DrawBox(gaugeX_, gaugeY_, gaugeX_ + gaugeW_, gaugeY_ + GAUGE_H, GAUGE_BACK_COLOR, TRUE);
		int fillW_ = static_cast<int>(gaugeW_ * slot.stats_[i_].rate_);
		DrawBox(gaugeX_, gaugeY_, gaugeX_ + fillW_, gaugeY_ + GAUGE_H, GAUGE_COLOR, TRUE);
	}
}

void ShopScene::DrawButton(void) const
{
	AREA button_ = GetButtonArea();
	DrawBox(
		button_.x_, button_.y_, button_.x_ + button_.w_, button_.y_ + button_.h_,
		CELL_COLOR, TRUE);
	DrawCenterString(button_, START_TEXT, TEXT_COLOR);
	DrawFrame(button_, selected_ == START_INDEX);
}

void ShopScene::DrawFrame(const AREA& area, bool isSelected) const
{
	unsigned int color_ = isSelected ? SELECT_FRAME_COLOR : FRAME_COLOR;
	int thick_ = isSelected ? SELECT_FRAME_THICK : FRAME_THICK;
	for (int t_ = 0; t_ < thick_; t_++)
	{
		DrawBox(
			area.x_ + t_, area.y_ + t_,
			area.x_ + area.w_ - t_, area.y_ + area.h_ - t_,
			color_, FALSE);
	}
}

void ShopScene::DrawCenterString(
	const AREA& area, const char* text, unsigned int color) const
{
	// 文字の幅を測って、範囲の左右中央にそろえる(縦は、範囲の中央)
	int textW_ = GetDrawStringWidth(text, static_cast<int>(strlen(text)));
	int x_ = area.x_ + (area.w_ - textW_) / CENTER_DIV;
	int y_ = area.y_ + (area.h_ - LINE_H) / CENTER_DIV;
	DrawString(x_, y_, text, color);
}

int ShopScene::DrawWrapped(
	const std::string& text, int x, int y, int maxWidth, unsigned int color) const
{
	// 1文字ずつ足していって、幅を超えたら次の行にする
	std::string line_;
	int lineCount_ = 0;
	size_t pos_ = 0;
	while (pos_ < text.size())
	{
		int bytes_ = GetCharBytes(GetUseCharCodeFormat(), text.c_str() + pos_);
		if (bytes_ <= 0)
		{
			bytes_ = 1;
		}
		std::string ch_ = text.substr(pos_, bytes_);
		std::string next_ = line_ + ch_;

		if (!line_.empty()
			&& GetDrawStringWidth(next_.c_str(), static_cast<int>(next_.size())) > maxWidth)
		{
			DrawString(x, y + LINE_H * lineCount_, line_.c_str(), color);
			lineCount_++;
			next_ = ch_;
		}
		line_ = next_;
		pos_ += bytes_;
	}

	if (!line_.empty())
	{
		DrawString(x, y + LINE_H * lineCount_, line_.c_str(), color);
		lineCount_++;
	}
	return lineCount_;
}