#pragma once
#include "SceneBase.h"
#include "PauseScene.h"
#include "../Object/Dirt/DirtManager.h"
#include "../Manager/EnemyManager.h"
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
	static constexpr const char* PROMPT_TEXT = "掃除する\n\n E長押し";
	static constexpr unsigned int PROMPT_COLOR = 0xffffff;

	// 操作表示を照準からどれだけ下に出すか
	static constexpr int PROMPT_OFFSET_Y = 40;

	// 掃除した数の表示
	static constexpr const char* COUNT_TEXT = "掃除した数 %d/ %d";
	static constexpr unsigned int COUNT_COLOR = 0xffffff;

	// 掃除した数を出す、画面左端からの距離
	static constexpr int COUNT_POS_X = 20;

	// タイトルに戻る確認の文字
	static constexpr const char* BACK_TEXT = "タイトルに戻りますか？";

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

	// 掃除した数の描画
	void DrawCleanCount(void);

private:

	// 敵の管理
	EnemyManager* enemyManager_;

	// 汚れの管理
	DirtManager* dirtManager_;

	// ゲームクリアへの移行を始めたか
	bool isClear_;

	// グリッド線
	Grid* grid_;

	// ステージ
	Stage* stage_;

	// プレイヤー
	Player* player_;

	// タイトルに戻る確認の選択肢
	PauseScene backMenu_;

	// 確認の選択肢の更新
	void UpdateBackMenu(void);

};