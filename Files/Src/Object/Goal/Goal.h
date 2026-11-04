#pragma once
#include <DxLib.h>

class Goal
{

public:

	// ドアの位置(足元の中心)。仮の位置なので、ステージに合わせて変える
	static constexpr VECTOR POS = { 0.0f, 0.0f, -800.0f };

	// ドアの大きさ(幅、高さ、厚み)
	static constexpr float WIDTH = 120.0f;
	static constexpr float HEIGHT = 220.0f;
	static constexpr float DEPTH = 20.0f;

	// ドアを開けられる距離(カメラからドアの中心まで)
	static constexpr float OPEN_DISTANCE = 300.0f;

	// ドアを開けるのにかかる長押しの時間(秒)
	static constexpr float OPEN_TIME = 3.0f;

	// 仮の見た目の色(普通のとき、狙っているとき)
	static constexpr unsigned int NORMAL_COLOR = 0x886644;
	static constexpr unsigned int AIM_COLOR = 0xddaa66;

	// 画面座標を3D座標に変換するときのZ値(0.0fが手前、1.0fが奥)
	static constexpr float SCREEN_Z_NEAR = 0.0f;
	static constexpr float SCREEN_Z_FAR = 1.0f;

	// 画面の中心の計算用
	static constexpr int CENTER_DIV = 2;

	// 半分にするための割る数
	static constexpr float HALF_DIV = 2.0f;

	// 線と箱の判定で調べる軸の数(X、Y、Z)
	static constexpr int AXIS_COUNT = 3;

	// 線が軸と平行とみなす、方向の大きさの限界
	static constexpr float PARALLEL_EPSILON = 0.0001f;

	// コンストラクタ
	Goal(void);

	// デストラクタ
	~Goal(void);

	// 初期化
	void Init(void);

	// 更新(狙ってEを長押しして、開けたら true)
	bool Update(void);

	// 描画
	void Draw(void) const;

	// ドアを狙っているか(画面の中心が、ドアに向いていて、近いか)
	bool IsAiming(void) const;

	// 長押しの進み具合(0.0?1.0)
	float GetHoldRate(void) const;

private:

	// 長押しした時間(秒)
	float holdTime_;

	// ドアの箱の、小さいほうの角と、大きいほうの角
	VECTOR minPos_;
	VECTOR maxPos_;

	// 線(始点と終点)が、ドアの箱を通るか
	bool IsHitLine(const VECTOR& start, const VECTOR& end) const;

};