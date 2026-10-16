#pragma once
#include "SceneBase.h"
#include "../Object/Dirt/DirtManager.h"
class Grid;
class Stage;
class Player;

class GameScene : public SceneBase
{

public:

	// 照準の半径
	static constexpr int CROSSHAIR_RADIUS = 3;

	// 照準の色
	static constexpr unsigned int CROSSHAIR_COLOR = 0xffffff;

	// 画面中心計算用
	static constexpr int CENTER_DIV = 2;

	// 操作表示の文字と色
	static constexpr const char* PROMPT_TEXT = "E長押し";
	static constexpr unsigned int PROMPT_COLOR = 0xffffff;

	// 操作表示を照準からどれだけ下に出すか
	static constexpr int PROMPT_OFFSET_Y = 40;

	// コンストラクタ
	GameScene(void);

	// デストラクタ
	~GameScene(void) override;

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

	// 照準の描画
	void DrawCrosshair(void);

	// 操作表示の描画
	void DrawPrompt(void);

private:

	// 汚れの管理
	DirtManager* dirtManager_;

	// グリッド線
	Grid* grid_;

	// ステージ
	Stage* stage_;

	// プレイヤー
	Player* player_;

};