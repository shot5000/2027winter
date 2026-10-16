#pragma once
#include <vector>
#include <DxLib.h>
#include "Dirt.h"

class DirtManager
{

public:

	// 配置する汚れの数
	static constexpr int DIRT_COUNT = 5;

	// 汚れを置く座標(増やすときは DIRT_COUNT も変える)
	static constexpr VECTOR DIRT_POS_LIST[DIRT_COUNT] = {
		{ 0.0f, 0.0f, -300.0f },
		{ 300.0f, 0.0f, -100.0f },
		{ -300.0f, 0.0f, 0.0f },
		{ 500.0f, 0.0f, -400.0f },
		{ -500.0f, 0.0f, -200.0f },
	};

	// 掃除できる距離
	static constexpr float CLEAN_DISTANCE = 300.0f;

	// 掃除にかかる長押しの時間(秒)
	static constexpr float CLEAN_TIME = 0.5f;

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

	// コンストラクタ
	DirtManager(void);

	// デストラクタ
	~DirtManager(void);

	// 初期化
	void Init(void);

	// 更新(長押しの判定と掃除)
	void Update(void);

	// 描画
	void Draw(void);

	// 汚れを狙っているか
	bool IsAiming(void) const;

	// 掃除した数の取得
	int GetCleanCount(void) const;

	// 全部掃除したか
	bool IsAllCleaned(void) const;

private:

	// 汚れの配列
	std::vector<Dirt> dirts_;

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

	// 狙っている汚れの番号を返す(なければNO_INDEX)
	int GetAimedIndex(void) const;

	// 経過時間の更新
	void UpdateDeltaTime(void);

	// 長押しの記録をリセット
	void ResetHold(void);

};