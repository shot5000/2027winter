#pragma once
#include <vector>
#include <DxLib.h>
#include "Dirt.h"

class DirtManager
{

public:

	// 配置する汚れの数
	static constexpr int DIRT_COUNT = 5;

	// 汚れの発生位置を書いたJSONのパス
	static constexpr const char* SPAWN_JSON_PATH = "Data/Json/DirtData.json";

	// JSONの1つの位置は [X, Y, Z] の3つの数字
	static constexpr size_t POS_ELEMENT_NUM = 3;

	// 狙っている汚れがないときの番号
	static constexpr int NO_INDEX = -1;

	// 画面座標を3D座標に変換するときのZ値(0.0fが手前、1.0fが奥)
	static constexpr float SCREEN_Z_NEAR = 0.0f;
	static constexpr float SCREEN_Z_FAR = 1.0f;

	// 画面の中心を求めるための割る数
	static constexpr int CENTER_DIV = 2;

	// マイクロ秒を秒に直すための割る数
	static constexpr float MICRO_SEC_TO_SEC = 1000000.0f;

	// 1フレームの経過時間の上限(秒)
	static constexpr float DELTA_TIME_MAX = 0.1f;

	// 汚れ1個を消したときの報酬(お金)
	static constexpr int DIRT_REWARD = 100;

	// 汚れを消していないときの報酬
	static constexpr int NO_REWARD = 0;

	// コンストラクタ
	DirtManager(void);

	// デストラクタ
	~DirtManager(void);

	// 初期化
	void Init(void);

	// 更新(掃除できるときだけ、長押しの判定をする)
	// cleanTime は汚れ1つを消す時間、cleanReach は掃除できる距離
	// このフレームに汚れを消したら、その報酬(お金)を返す。消していなければ NO_REWARD
	int Update(bool canClean, float cleanTime, float cleanReach);

	// 描画
	void Draw(void);

	// 汚れを狙っているか(cleanReach は掃除できる距離)
	bool IsAiming(float cleanReach) const;

	// 掃除した数の取得
	int GetCleanCount(void) const;

private:

	// 汚れの配列
	std::vector<Dirt> dirts_;

	// 汚れの発生位置の候補(JSONから読み込む)
	std::vector<VECTOR> spawnPositions_;

	// 長押ししている汚れの番号
	int holdIndex_;

	// 長押しした時間(秒)
	float holdTime_;

	// 前フレームの時刻
	LONGLONG preTime_;

	// 前フレームからの経過時間(秒)
	float deltaTime_;

	// 掃除した数
	int cleanCount_;

	// JSONから発生位置の候補を読み込む(読み込めたらtrue)
	bool LoadSpawnPositions(void);

	// 候補の中からランダムに num_ 個の位置を選ぶ
	std::vector<VECTOR> PickSpawnPositions(int num_) const;

	// 狙っている汚れの番号を返す(なければ NO_INDEX)
	int GetAimedIndex(float cleanReach) const;

	// 経過時間の更新
	void UpdateDeltaTime(void);

	// 長押しの記録をリセット
	void ResetHold(void);

};