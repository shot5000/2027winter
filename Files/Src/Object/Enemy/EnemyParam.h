#pragma once
#include <DxLib.h>

// 敵の性能(JSONで決める値)
struct EnemyParam
{
	// 度をラジアンに直す割合
	static constexpr float DEG_TO_RAD = DX_PI_F / 180.0f;

	// 読み込めなかったとき、JSONに書かれていないときの初期値
	static constexpr int DEFAULT_HP = 3;
	static constexpr float DEFAULT_SEARCH_SPEED = 0.0f;
	static constexpr float DEFAULT_CHASE_SPEED = 200.0f;
	static constexpr float DEFAULT_FAN_RANGE = 400.0f;
	static constexpr float DEFAULT_FAN_ANGLE_DEG = 90.0f;
	static constexpr float DEFAULT_CHASE_RANGE = 800.0f;
	static constexpr int DEFAULT_ATTACK_POWER = 1;

	int hp_ = DEFAULT_HP;					// 体力
	float searchSpeed_ = DEFAULT_SEARCH_SPEED;	// 索敵中の移動の速さ(1秒あたりに進む距離)
	float chaseSpeed_ = DEFAULT_CHASE_SPEED;	// 検知中の移動の速さ
	float fanRange_ = DEFAULT_FAN_RANGE;		// 索敵の扇の半径
	float fanAngleDeg_ = DEFAULT_FAN_ANGLE_DEG;	// 索敵の扇の角度(度。全体)
	float chaseRange_ = DEFAULT_CHASE_RANGE;	// 検知後の円の半径
	int attackPower_ = DEFAULT_ATTACK_POWER;	// 攻撃力
};