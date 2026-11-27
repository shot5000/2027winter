#pragma once
#include "EnemyBase.h"
#include "EnemyGhost.h"
#include "../Noise/NoiseManager.h"

// 音が聞こえた場所へ向かう敵(目は見えない)
class EnemyBlind : public EnemyBase
{

public:

	// 性能(JSON)の名前
	static constexpr const char* PARAM_ID = "blind";

	// モデルの色にかける割合(Ghostと見分けるため、橙色にする)
	static constexpr COLOR_F TINT_COLOR_SCALE = { 1.0f, 0.7f, 0.3f, 1.0f };

	// 目的地に着いたとみなす距離(水平)
	static constexpr float ARRIVE_RANGE = 80.0f;

	// 着いてから、その場で聞き耳を立てる時間(秒)
	static constexpr float LISTEN_TIME = 3.0f;

	// 同じ階とみなす高さの差(音は、同じ階の敵にだけ聞こえる)
	static constexpr float SAME_FLOOR_RANGE = 300.0f;

	// 目的地の印(確認用)：半径、分割数、色、床からの高さ
	static constexpr float MARK_RADIUS = 20.0f;
	static constexpr int MARK_DIV_NUM = 12;
	static constexpr unsigned int MARK_COLOR = 0xffaa00;
	static constexpr float MARK_OFFSET_Y = 30.0f;

	// コンストラクタ(置く座標と、最初の向きを受け取る)
	EnemyBlind(const VECTOR& pos, float angleY);

	// デストラクタ
	~EnemyBlind(void) override;

	// 初期化
	void Init(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

protected:

	// 敵ごとの動き(音が聞こえた場所へ向かう)
	void UpdateProcess(const VECTOR& targetPos) override;

private:

	// 状態
	enum class STATE
	{
		WAIT,	// 音が聞こえるまで、その場で止まる
		MOVE,	// 音がした場所へ向かう
		LISTEN,	// 着いて、聞き耳を立てる
	};

	// モデルのハンドルID
	int modelId_;

	// 状態
	STATE state_;

	// 音の聞こえやすさ(音の範囲にかける倍率。1.0で標準)
	float hearingRate_;

	// 最後に聞いた音の番号(同じ音を何度も聞かないため)
	int lastHeardId_;

	// 向かっている場所(音がした場所)
	VECTOR noisePos_;

	// 聞き耳を立てる残り時間(秒)
	float listenTimer_;

	// 新しく聞こえた音があれば、その場所を目的地にする
	void Hear(void);

	// この音が、今の位置で聞こえるか
	bool IsAudible(const NoiseManager::NOISE& noise) const;

	// 水平方向の距離
	static float HorizontalDist(const VECTOR& a, const VECTOR& b);

};