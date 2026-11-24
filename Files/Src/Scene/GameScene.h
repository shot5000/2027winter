#pragma once
#include "SceneBase.h"
#include "PauseScene.h"
#include "../Object/Enemy/EnemyManager.h"
#include "../Object/Dirt/DirtManager.h"
#include "../Object/Goal/Goal.h"
#include "../Object/Item/ItemManager.h"
#include "../Object/Item/FloorItemManager.h"
#include "../Object/Stage/DoorManager.h"
class Grid;
class Stage;
class Player;

class GameScene : public SceneBase
{

public:

	// 制限時間(分)と、1分の秒数
	static constexpr int TIME_LIMIT_MIN = 5;
	static constexpr int SEC_PER_MIN = 60;

	// 制限時間(秒)
	static constexpr float TIME_LIMIT = static_cast<float>(TIME_LIMIT_MIN * SEC_PER_MIN);

	// 残り時間の表示(確認用。出したくないときは false にする)
	static constexpr bool IS_DRAW_TIME = true;
	static constexpr const char* TIME_TEXT = "残り %d:%02d";
	static constexpr unsigned int TIME_COLOR = 0xffffff;
	static constexpr int TIME_POS_Y = 20;

	// 文字を作るバッファの大きさ(所持金、残り時間)
	static constexpr int TEXT_BUF_SIZE = 32;

	// 照準の半径
	static constexpr int CROSSHAIR_RADIUS = 3;

	// 照準の色
	static constexpr unsigned int CROSSHAIR_COLOR = 0xffffff;

	// 画面中心計算用
	static constexpr int CENTER_DIV = 2;

	// 操作表示の文字と色
	static constexpr const char* PROMPT_TEXT = "掃除する \n\n E長押し";
	static constexpr const char* GOAL_PROMPT_TEXT = "E長押し：ドアを開ける";
	static constexpr unsigned int PROMPT_COLOR = 0xffffff;

	// 操作表示を照準からどれだけ下に出すか
	static constexpr int PROMPT_OFFSET_Y = 40;

	// ドアを開ける長押しのゲージ：大きさ、操作表示からの距離、色
	static constexpr int GAUGE_W = 200;
	static constexpr int GAUGE_H = 10;
	static constexpr int GAUGE_OFFSET_Y = 30;
	static constexpr unsigned int GAUGE_BACK_COLOR = 0x444444;
	static constexpr unsigned int GAUGE_COLOR = 0xffff00;

	// 掃除した数の表示
	static constexpr const char* COUNT_TEXT = "掃除した数 %d/ %d";
	static constexpr unsigned int COUNT_COLOR = 0xffffff;

	// 掃除した数を出す、画面左端からの距離
	static constexpr int COUNT_POS_X = 20;

	// 所持金の表示
	static constexpr const char* MONEY_TEXT = "所持金 %d円";
	static constexpr unsigned int MONEY_COLOR = 0xffff00;

	// 所持金を出す、画面の右端と上端からの距離
	static constexpr int MONEY_MARGIN_X = 20;
	static constexpr int MONEY_POS_Y = 20;

	// タイトルに戻る確認の文字
	static constexpr const char* BACK_TEXT = "タイトルに戻りますか？";

	// 経過時間の計算(マイクロ秒→秒)と、1フレームの上限
	static constexpr float MICRO_SEC_TO_SEC = 1000000.0f;
	static constexpr float DELTA_TIME_MAX = 0.1f;

	// 扉の表示の文字
	static constexpr const char* DOOR_PROMPT_TEXT = "Eで開閉";

	// 床のアイテムの操作表示
	static constexpr const char* FLOOR_PROMPT_TEXT = "%s  Eで拾う";
	static constexpr const char* FULL_PROMPT_TEXT = "スロットがいっぱいです";

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

	// 所持金の描画
	void DrawMoney(void);

	// 残り時間の描画
	void DrawTime(void);

	// 扉の更新(視線の先の扉の判定と、Eキーでの開閉)
	void UpdateDoor(void);

	// 扉の表示の描画
	void DrawDoorPrompt(void);

	// 床のアイテムの更新(拾う、捨てる)
	void UpdateFloorItem(void);

private:

	// タイトルに戻る確認の選択肢
	PauseScene backMenu_;

	// アイテム(スロットと、攻撃)の管理
	ItemManager itemManager_;

	// ドア
	Goal goal_;

	// 敵の管理
	EnemyManager* enemyManager_;

	// 汚れの管理
	DirtManager* dirtManager_;

	// 残り時間(秒)
	float timeLeft_;

	// グリッド線
	Grid* grid_;

	// ステージ
	Stage* stage_;

	// プレイヤー
	Player* player_;

	// 扉の管理
	DoorManager doorMng_;

	// 視線の先の扉の番号(なければ-1)
	int aimedDoor_;

	// 前のフレームの時刻
	LONGLONG preTime_;

	// 床に置かれたアイテム
	FloorItemManager floorItems_;

	// 視線の先の、床のアイテムの番号(なければ-1)
	int aimedFloorItem_;

	// 確認の選択肢の更新
	void UpdateBackMenu(void);

	// 照準の下に、操作表示の文字を出す
	void DrawPromptText(const char* text);

	// ゴールを開ける長押しのゲージの描画
	void DrawGoalGauge(void);

};