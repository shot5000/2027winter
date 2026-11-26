#pragma once
#include "EnemyBase.h"
#include "EnemyGhost.h"

// 見られている間は止まり、見られていないときだけ近づいてくる敵
class EnemyStatue : public EnemyBase
{

public:

	// 性能(JSON)の名前
	static constexpr const char* PARAM_ID = "statue";

	// モデルの色にかける割合(Ghostと見分けるため、青っぽくする)
	static constexpr COLOR_F TINT_COLOR_SCALE = { 0.4f, 0.6f, 1.0f, 1.0f };

	// 見られているかを調べるとき、視線を向ける高さ(足元から。体の中心)
	// モデルと体の形は Ghost と同じものを使う。専用のモデルにするときは、ここを変える
	static constexpr float VIEW_TARGET_Y = EnemyGhost::BODY_CENTER_Y;

	// コンストラクタ(置く座標と、最初の向きを受け取る)
	EnemyStatue(const VECTOR& pos, float angleY);

	// デストラクタ
	~EnemyStatue(void) override;

	// 初期化
	void Init(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

protected:

	// 敵ごとの動き(見られている間は止まる)
	void UpdateProcess(const VECTOR& targetPos) override;

private:

	// モデルのハンドルID
	int modelId_;

	// 動き出す範囲(水平の距離)
	float activeRange_;

	// 見られたとみなす視野の角度(片側。正面からこの角度の中)
	float viewHalfAngle_;

	// プレイヤーに見られているか
	bool IsSeen(const VECTOR& targetPos) const;

};