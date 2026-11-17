#pragma once
#include <string>
#include <vector>
#include <DxLib.h>

class BodyCollision;

// 扉(教室の引き戸・個室とロッカーの開き戸)をまとめて扱う
class DoorManager
{
public:

	// 扉の種類
	enum class TYPE
	{
		SLIDE,	// 教室の引き戸
		HIDE,	// トイレ個室・ロッカーの開き戸
	};

	// 扉の近くにいる人(プレイヤーや敵)。閉めるときに挟まないか調べる
	struct OCCUPANT
	{
		const BodyCollision* body_;
		VECTOR pos_;
	};

	DoorManager(void);
	~DoorManager(void);

	// stageModelId : 扉のフレームが入っているステージのモデル
	void Init(int stageModelId);
	void Update(float deltaTime, const std::vector<OCCUPANT>& occupants);
	void Release(void);

	// 視線の先にある扉の番号(なければ -1)
	int FindAimed(const VECTOR& eyePos, const VECTOR& dir) const;

	// 開いていれば閉める、閉じていれば開ける
	void Toggle(int index);

	// 敵が扉を開ける(条件を満たす扉だけ開く)
	void EnemyOpen(
		const VECTOR& enemyPos, bool isChasing, const VECTOR& playerPos);

	bool IsClosed(int index) const;

private:

	// 速さ(1秒で何回分開くか)
	static constexpr float OPEN_SPEED = 2.0f;

	// 引き戸が動く距離
	static constexpr float SLIDE_DIST = 150.0f;

	// 開き戸の開く角度(度)
	static constexpr float HINGE_OPEN_DEG = 90.0f;

	// 開く向きが逆なら -1.0f にする(Blender→DxLibで反転しているとき用)
	static constexpr float HINGE_SIGN = 1.0f;
	static constexpr float SLIDE_SIGN = 1.0f;

	// 視線の届く距離
	static constexpr float AIM_REACH = 200.0f;

	// 敵が扉に触れられる距離
	static constexpr float ENEMY_OPEN_DIST = 200.0f;

	// 個室・ロッカーの扉を開ける条件
	static constexpr float HIDE_PLAYER_DIST = 200.0f;	// 扉とプレイヤーの距離
	static constexpr float HIDE_VERY_CLOSE = 250.0f;	// 敵とプレイヤーが「とても近い」距離
	static constexpr float HIDE_CHASE_RANGE = 1500.0f;	// 発見状態のときの範囲

	// 当たり判定のきめ細かさ(扉は小さいので小さめ)
	static constexpr int COLL_DIV = 4;

	// フレーム名の頭の文字
	static const char* const SLIDE_PREFIX;
	static const char* const HINGE_PREFIX;
	static const char* const LOCKER_PREFIX;

	// 向きの情報(Blenderから書き出したもの)
	static const char* const JSON_PATH;

	struct DOOR
	{
		int frame_;
		TYPE type_;
		float sign_;		// 開く向き(+1 / -1)
		float rate_;		// 0:閉 ? 1:開
		bool isOpenTarget_;	// 開こうとしているか
		MATRIX base_;		// 閉じているときの行列
		VECTOR pos_;		// 閉じているときの位置
	};

	int modelId_;
	std::vector<DOOR> doors_;

	void LoadFrames(void);
	void ApplyPose(const DOOR& door);
	bool IsBlocked(const DOOR& door, const std::vector<OCCUPANT>& occupants) const;
	void UpdateClosedFrames(void);
	static float HorizontalDist(const VECTOR& a, const VECTOR& b);
};