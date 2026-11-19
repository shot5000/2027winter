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

	// 敵やプレイヤーとの距離を測るとき、同じ階とみなす高さの差
	static constexpr float OPEN_HEIGHT_RANGE = 200.0f;

	DoorManager(void);
	~DoorManager(void);

	// stageModelId : 扉のフレームが入っているステージのモデル
	void Init(int stageModelId);
	void Update(float deltaTime);
	void Release(void);

	// 視線の先にある扉の番号(なければ -1)
	int FindAimed(const VECTOR& eyePos, const VECTOR& dir) const;

	// 開いていれば閉める、閉じていれば開ける
	void Toggle(int index);

	// 敵が扉を開ける(条件を満たす扉だけ開く)
	void EnemyOpen(
		const VECTOR& enemyPos, bool canOpenHide, const VECTOR& playerPos);

	// 閉じているか(開きかけでも閉じているとみなす)
	bool IsClosed(int index) const;

	// 閉じた個室・ロッカーの扉の中に隠れているか
	bool IsHidden(const BodyCollision& body, const VECTOR& pos) const;

private:

	// 速さ(1秒で何回分開くか)
	static constexpr float OPEN_SPEED = 2.0f;

	// 引き戸が動く距離
	static constexpr float SLIDE_DIST = 150.0f;

	// 開き戸の開く角度(度)
	static constexpr float HINGE_OPEN_DEG = 90.0f;

	// 開く向きが逆なら -1.0f にする(Blender→DxLibで反転しているとき用)
	static constexpr float HINGE_SIGN = -1.0f;
	static constexpr float SLIDE_SIGN = 1.0f;

	// 視線の届く距離
	static constexpr float AIM_REACH = 200.0f;

	// 敵が扉に触れられる距離
	static constexpr float ENEMY_OPEN_DIST = 200.0f;

	// 個室・ロッカーの扉を開ける条件
	static constexpr float HIDE_PLAYER_DIST = 200.0f;	// 扉とプレイヤーの距離
	static constexpr float HIDE_VERY_CLOSE = 250.0f;	// 敵とプレイヤーが「とても近い」距離

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
	bool IsMoving(const DOOR& door) const;
	void UpdateClosedFrames(void);
	static float HorizontalDist(const VECTOR& a, const VECTOR& b);

	// 同じ階で、水平の距離が近いか
	static bool IsNear(const VECTOR& a, const VECTOR& b, float dist);
};