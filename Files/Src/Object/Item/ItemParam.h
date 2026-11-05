#pragma once

// アイテムの性能(JSONで決める値)
struct ItemParam
{
	// 読み込めなかったとき、JSONに書かれていないときの初期値
	static constexpr int DEFAULT_ATTACK_POWER = 1;
	static constexpr float DEFAULT_COOLDOWN_TIME = 0.5f;
	static constexpr float DEFAULT_ATTACK_REACH = 150.0f;
	static constexpr float DEFAULT_ATTACK_RADIUS = 30.0f;
	static constexpr float DEFAULT_CLEAN_SPEED = 1.0f;
	static constexpr float DEFAULT_CLEAN_REACH = 300.0f;
	static constexpr float DEFAULT_MOVE_SPEED_RATE = 1.0f;
	static constexpr int DEFAULT_DURABILITY = 100;
	static constexpr float DEFAULT_VOLUME = 1.0f;
	static constexpr int DEFAULT_PRICE = 0;

	int attackPower_ = DEFAULT_ATTACK_POWER;			// 攻撃力(敵に与えるダメージ)
	float cooldownTime_ = DEFAULT_COOLDOWN_TIME;		// 攻撃の待ち時間(秒)
	float attackReach_ = DEFAULT_ATTACK_REACH;			// 攻撃のリーチ(攻撃の長さ)
	float attackRadius_ = DEFAULT_ATTACK_RADIUS;		// 攻撃の太さ(カプセルの半径)
	float cleanSpeed_ = DEFAULT_CLEAN_SPEED;			// 掃除の速さ(1.0で、汚れ1つに1秒)
	float cleanReach_ = DEFAULT_CLEAN_REACH;			// 掃除できる距離(カメラから汚れまで)
	float moveSpeedRate_ = DEFAULT_MOVE_SPEED_RATE;		// 持っているときの、歩く速さにかける割合
	int durability_ = DEFAULT_DURABILITY;				// 耐久力(まだ使わない)
	float attackVolume_ = DEFAULT_VOLUME;				// 攻撃時の音量(まだ使わない)
	float cleanVolume_ = DEFAULT_VOLUME;				// 掃除時の音量(まだ使わない)
	int price_ = DEFAULT_PRICE;							// 値段(まだ使わない)
};