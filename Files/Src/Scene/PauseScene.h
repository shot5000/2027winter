#pragma once
#include <string>

class PauseScene
{

public:

	// 選んだ結果
	enum class RESULT
	{
		NONE, // まだ選んでいない
		YES,
		NO,
	};

	// 選択肢
	enum class CHOICE
	{
		YES,
		NO,
		MAX,
	};

	// 選択肢の数と文字
	static constexpr int CHOICE_COUNT = static_cast<int>(CHOICE::MAX);
	static constexpr const char* CHOICE_TEXT[CHOICE_COUNT] =
	{ "はい", "いいえ" };

	// 開いたときのカーソルの位置(誤操作を防ぐため「いいえ」)
	static constexpr int DEFAULT_INDEX = static_cast<int>(CHOICE::NO);

	// 文字の色
	static constexpr unsigned int NORMAL_COLOR = 0xffffff;
	static constexpr unsigned int HIGHLIGHT_COLOR = 0xffff00;

	// 後ろを暗くする色と濃さ
	static constexpr unsigned int DIM_COLOR = 0x000000;
	static constexpr int DIM_ALPHA = 150;

	// 画面の中心の計算用
	static constexpr int CENTER_DIV = 2;

	// 表示位置(画面の中心からのずれ)
	static constexpr int QUESTION_OFFSET_Y = -60;
	static constexpr int CHOICE_OFFSET_Y = 20;
	static constexpr int CHOICE_ADD_Y = 50;

	// コンストラクタ(問いかけの文字を受け取る)
	PauseScene(const std::string& question);

	// デストラクタ
	~PauseScene(void);

	// 開く
	void Open(void);

	// 開いているか
	bool IsOpen(void) const;

	// 更新(選んだら、その結果を返して閉じる。選んでいない間は NONE)
	RESULT Update(void);

	// 描画(開いているときだけ描く)
	void Draw(void) const;

private:

	// 問いかけの文字
	std::string question_;

	// 開いているか
	bool isOpen_;

	// カーソルの位置
	int index_;

	// 文字を左右中央にそろえて描く
	void DrawCenterString(
		int centerX, int posY, const char* text, unsigned int color) const;

};