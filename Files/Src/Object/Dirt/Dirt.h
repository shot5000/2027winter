#pragma once
#include <DxLib.h>

class Dirt
{

public:

	// 汚れの半径
	static constexpr float DEFAULT_RADIUS = 50.0f;

	// 球体の分割数
	static constexpr int SPHERE_DIV_NUM = 16;

	// 球体の色
	static constexpr unsigned int DIFFUSE_COLOR = 0x606060;
	static constexpr unsigned int SPECULAR_COLOR = 0x606060;

	// コンストラクタ(置く座標を受け取る)
	Dirt(const VECTOR& pos);

	// デストラクタ
	~Dirt(void);

	// 描画
	void Draw(void);

	// 線と当たっているか
	bool IsHitLine(const VECTOR& start, const VECTOR& end) const;

	// 指定した座標からの距離
	float GetDistance(const VECTOR& from) const;

	// 座標の取得
	const VECTOR& GetPos(void) const;

private:

	// 座標
	VECTOR pos_;

	// 半径
	float radius_;

};