#pragma once
#include "../Object/Collision/BodyCollision.h"
#include <DxLib.h>

class Camera
{

public:

	// カメラの初期座標
	static constexpr VECTOR DERFAULT_POS = { 0.0f, 200.0f, -500.0f };
	
	// 当たり判定の体の形(基準は目の位置)
	static constexpr float CAPSULE_RADIUS = 30.0f;
	static constexpr float CAPSULE_TOP_Y = 0.0f;
	static constexpr float CAPSULE_DOWN_Y = -60.0f;
	static constexpr float FOOT_Y = -110.0f;

	// カメラの初期角度
	static constexpr VECTOR DERFAULT_ANGLES = {
	30.0f * DX_PI_F / 180.0f, 0.0f, 0.0f
	};

	// カメラのクリップ範囲
	static constexpr float VIEW_NEAR = 1.0f;
	static constexpr float VIEW_FAR = 30000.0f;

	// マウス感度
	static constexpr float MOUSE_SENSITIVITY = 0.1f * DX_PI_F / 180.0f;

	// 上下の向きの限界角度
	static constexpr float ANGLE_X_LIMIT = 80.0f * DX_PI_F / 180.0f;

	// 中心計算用
	static constexpr int CENTER_DIV = 2;

	// 矢印キーで回す1フレームの角度
	static constexpr float ARROW_ROT_POW = 1.0f * DX_PI_F / 180.0f;

	// 移動方向(カメラから見た前後左右)
	static constexpr VECTOR DIR_FRONT = { 0.0f, 0.0f, 1.0f };
	static constexpr VECTOR DIR_BACK = { 0.0f, 0.0f, -1.0f };
	static constexpr VECTOR DIR_LEFT = { -1.0f, 0.0f, 0.0f };
	static constexpr VECTOR DIR_RIGHT = { 1.0f, 0.0f, 0.0f };

	// 移動速度
	static constexpr float MOVE_SPEED = 180.0f; // 歩行
	static constexpr float DASH_SPEED = 360.0f; // ダッシュ

	// マイクロ秒を秒に直すための割る数
	static constexpr float MICRO_SEC_TO_SEC = 1000000.0f;

	// 1フレームの経過時間の上限(秒)。処理が止まった後に急に飛ばないようにする
	static constexpr float DELTA_TIME_MAX = 0.1f;

	// ジャンプの初速(1秒あたりに上がる距離)
	static constexpr float JUMP_SPEED = 450.0f;

	// カメラモード
	enum class MODE
	{
		NONE,
		FIXED_POINT, // 定点カメラ
		FREE, // フリーモード
	};

	// コンストラクタ
	Camera(void);

	// デストラクタ
	~Camera(void);

	// 初期化
	void Init(void);

	// 更新
	void Update(void);

	// 描画前のカメラ設定
	void SetBeforeDraw(void);
	void SetBeforeDrawFixedPoint(void);
	void SetBeforeDrawFree(void);

	// デバッグ用描画
	void DrawDebug(void);

	// 解放
	void Release(void);

	// カメラモードの変更
	void ChangeMode(MODE mode);

	void MoveXYZDirection(void);

	// 座標の取得
	const VECTOR& GetPos(void) const;

	// 角度の取得
	const VECTOR& GetAngles(void) const;

	// マウスでカメラの角度を変える
	void RotateByMouse(void);

	// マウスカーソルを画面中央に戻す
	void ResetMousePos(void);

	// 経過時間の更新
	void UpdateDeltaTime(void);

	// ジャンプの入力処理
	void ProcessJump(void);

private:

	// カメラモード
	MODE mode_;

	// カメラの位置
	VECTOR pos_;

	// カメラの角度
	VECTOR angles_;

	// 前フレームの時刻
	LONGLONG preTime_;

	// 前フレームからの経過時間(秒)
	float deltaTime_;

	// 当たり判定
	BodyCollision body_;

};
