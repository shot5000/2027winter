#pragma once
#include "SceneBase.h"

class TitleScene : public SceneBase
{

public:

	// 表示中の画面
	enum class PAGE
	{
		TITLE,
		EXIT_SELECT,
	};

	// ゲーム終了確認の選択肢
	enum class EXIT_MENU
	{
		YES,
		NO,
		MAX,
	};

	// 終了確認の選択肢の数
	static constexpr int EXIT_MENU_COUNT = static_cast<int>(EXIT_MENU::MAX);

	// 終了確認の文字
	static constexpr const char* EXIT_TEXT = "ゲームを終了しますか？";
	static constexpr const char* EXIT_MENU_TEXT[EXIT_MENU_COUNT] =
	{ "はい", "いいえ" };

	// 終了確認を開いたときのカーソルの位置(誤操作を防ぐため「いいえ」)
	static constexpr int EXIT_DEFAULT_INDEX = static_cast<int>(EXIT_MENU::NO);

	// 文字の色
	static constexpr unsigned int NORMAL_COLOR = 0xffffff;
	static constexpr unsigned int HIGHLIGHT_COLOR = 0xffff00;

	// 画面の中心の計算用
	static constexpr int CENTER_DIV = 2;

	// 終了確認の表示位置(画面の中心からのずれ)
	static constexpr int EXIT_TEXT_OFFSET_Y = -60;
	static constexpr int EXIT_MENU_OFFSET_Y = 20;
	static constexpr int EXIT_MENU_ADD_Y = 50;

	// 操作の案内
	static constexpr const char* HINT_TEXT = "SPACE：ゲーム開始　ESC：ゲーム終了";
	static constexpr int HINT_POS_Y = 130;

	// コンストラクタ
	TitleScene(void);

	// デストラクタ
	~TitleScene(void) override;

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:

	// 表示中の画面
	PAGE page_;

	// 終了確認のカーソルの位置
	int menuIndex_;

	// タイトル画面の更新
	void UpdateTitle(void);

	// 終了確認の更新
	void UpdateExitMenu(void);

	// 終了確認の描画
	void DrawExitMenu(void);

	// 文字を左右中央にそろえて描く
	void DrawCenterString(
		int centerX, int posY, const char* text, unsigned int color) const;

};