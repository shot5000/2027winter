#pragma once
#include <DxLib.h>

class BodyCollision
{

public:

	// ステージが設定されていないときの値
	static constexpr int NO_MODEL = -1;

	// モデルのすべてのフレームを判定する指定
	static constexpr int ALL_FRAME = -1;

	// 重力の強さ(1秒あたりに増える落下速度)
	static constexpr float GRAVITY_POW = 980.0f;

	// 最大の落下速度
	static constexpr float MAX_FALL_SPEED = 2000.0f;

	// 壁の押し戻しを試す最大回数
	static constexpr int PUSH_TRY_COUNT = 20;

	// 1回の押し戻しの距離
	static constexpr float PUSH_DISTANCE = 1.0f;

	// 法線のYの絶対値がこれより大きい面は、床か天井とみなして壁にしない
	static constexpr float FLOOR_NORMAL_Y = 0.7f;

	// 押し戻す向きの水平方向の長さがこれより短い面は、壁にしない
	static constexpr float PUSH_DIR_MIN = 0.001f;

	// 足元より下まで伸ばす、地面判定の余裕
	static constexpr float GROUND_MARGIN = 10.0f;

	// デバッグ表示の球の分割数と色
	static constexpr int DEBUG_DIV_NUM = 8;
	static constexpr unsigned int DEBUG_COLOR = 0xff0000;

	// 壁から離れる位置の余裕(ぴったりだと、触れているかの判定が毎回ぶれる)
	static constexpr float PUSH_MARGIN = 0.1f;

	// 体の形(基準座標からのずれで指定する)
	struct PARAM
	{
		float radius_;		// カプセルの半径
		VECTOR topOffset_;	// 基準座標から、カプセルの一方の端(球の中心)まで
		VECTOR downOffset_;	// 基準座標から、もう一方の端(球の中心)まで
		float footY_;		// 足元(地面に接する高さ)
	};

	// コンストラクタ
	BodyCollision(void);

	// デストラクタ
	~BodyCollision(void);

	// 初期化(体の形を決める)
	void Init(const PARAM& param);

	// ステージのモデルを設定(全員共通)
	static void SetStageModel(int modelId);

	// 更新(壁の押し戻し→重力→地面)。移動を済ませてから呼ぶ
	void Update(VECTOR& pos, float deltaTime);

	// 重力のオン・オフ(空を飛ぶ敵はオフ)
	void SetGravityEnabled(bool isEnabled);

	// 落下速度を直接変える(ジャンプや吹き飛ばしで使う。上向きは-)
	void SetFallSpeed(float fallSpeed);

	// 地面に乗っているか
	bool IsGrounded(void) const;

	// カプセルの上と下の座標
	VECTOR GetCapsuleTop(const VECTOR& pos) const;
	VECTOR GetCapsuleDown(const VECTOR& pos) const;

	// カプセルの半径
	float GetRadius(void) const;

	// 別の体と当たっているか(敵とプレイヤーの接触など)
	bool IsHit(
		const VECTOR& pos,
		const BodyCollision& other,
		const VECTOR& otherPos) const;

	// デバッグ用にカプセルを描く
	void DrawDebug(const VECTOR& pos) const;

	// 体の向きを設定(Y軸の角度。カプセルの両端のずれを、この角度で回す)
	void SetAngleY(float angleY);

private:

	// ステージのモデルのハンドルID(全員で共有)
	static int stageModelId_;

	// 体の形
	PARAM param_;

	// 落下速度(下向きが+)
	float fallSpeed_;

	// 重力を使うか
	bool isGravity_;

	// 地面に乗っているか
	bool isGrounded_;

	// 体の向き(Y軸の角度)
	float angleY_;

	// カプセルの両端のうち、低いほうの高さ(基準座標からのずれ)
	float GetLowestOffsetY(void) const;

	// 基準座標からのずれを、体の向きに合わせて回す
	VECTOR RotateOffset(const VECTOR& offset) const;

	// 壁との衝突(カプセル)
	void CollisionWall(VECTOR& pos);

	// 重力で落ちる
	void ApplyGravity(VECTOR& pos, float deltaTime);

	// 地面との衝突(足元の線)
	void CollisionGround(VECTOR& pos);

};