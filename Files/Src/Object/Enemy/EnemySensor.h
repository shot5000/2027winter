#pragma once
#include <DxLib.h>

class EnemySensor
{

public:

	// 状態
	enum class STATE
	{
		SEARCH, // 索敵中(扇の範囲で探す)
		CHASE,  // 検知した(円の範囲を見る)
	};

	// 索敵の大きさ(敵ごとに決める)
	struct PARAM
	{
		float fanRange_;	// 扇の半径
		float fanAngle_;	// 扇の角度(全体)
		float chaseRange_;	// 円の半径(敵の位置が中心)
		float originDist_;	// 足元の中心から、扇の要(頭の先)までの距離
	};

	// 角度を半分にするための割る数
	static constexpr float HALF_DIV = 2.0f;

	// 真上・真下にいるとき、向きが決まらないので重なっているとみなす距離
	static constexpr float OVERLAP_DIST = 1.0f;

	// 円の角度(全体)
	static constexpr float CIRCLE_ANGLE = 360.0f * DX_PI_F / 180.0f;

	// 範囲の描画：床からの高さ、色、透明度
	static constexpr float DRAW_OFFSET_Y = 30.0f;
	static constexpr unsigned int COLOR_SEARCH = 0x3366ff;
	static constexpr unsigned int COLOR_CHASE = 0xff3333;
	static constexpr int DRAW_ALPHA = 100;

	// 範囲の描画：三角形の分割数(扇、円)
	static constexpr int FAN_DIV_NUM = 24;
	static constexpr int CIRCLE_DIV_NUM = 48;

	// コンストラクタ
	EnemySensor(void);

	// デストラクタ
	~EnemySensor(void);

	// 初期化(呼ばなければ、索敵は動かない)
	void Init(const PARAM& param);

	// 更新(敵の位置、頭の向き、追う相手の位置を渡す)
	void Update(const VECTOR& pos, const VECTOR& frontDir, const VECTOR& targetPos);

	// 範囲の描画
	void Draw(const VECTOR& pos, const VECTOR& frontDir) const;

	// 検知中か
	bool IsChasing(void) const;

private:

	// 大きさ
	PARAM param_;

	// 状態
	STATE state_;

	// 使うか(Initを呼んだときだけ使う)
	bool isEnabled_;

	// 扇の要の位置(頭の先)
	VECTOR GetOrigin(const VECTOR& pos, const VECTOR& frontDir) const;

	// 扇の範囲の中にいるか
	bool IsInFan(
		const VECTOR& pos, const VECTOR& frontDir, const VECTOR& targetPos) const;

	// 円の範囲の中にいるか
	bool IsInCircle(const VECTOR& pos, const VECTOR& targetPos) const;

	// 扇形(円も含む)を、細かい三角形に分けて描く
	void DrawSector(
		const VECTOR& center, float startAngle, float totalAngle,
		float radius, int divNum, unsigned int color) const;

	// 角度から、水平方向の向きのベクトルを求める
	static VECTOR GetDirXZ(float angleY);

};