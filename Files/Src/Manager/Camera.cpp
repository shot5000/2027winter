#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "Camera.h"

Camera::Camera(void)
{
	// DxLibの初期設定では、
	// カメラの位置が x = 320.0f, y = 240.0f, z = (画面のサイズによって変化)、
	// 注視点の位置は x = 320.0f, y = 240.0f, z = 1.0f
	// カメラの上方向は x = 0.0f, y = 1.0f, z = 0.0f
	// 右上位置からZ軸のプラス方向を見るようなカメラ
	mode_ = MODE::NONE;
}

Camera::~Camera(void)
{
}

void Camera::Init(void)
{
	ResetState();
}

void Camera::Update(void)
{
}

void Camera::SetBeforeDraw(void)
{
	// クリップ距離を設定する(SetDrawScreenでリセットされる)
	SetCameraNearFar(VIEW_NEAR, VIEW_FAR);
	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
		SetBeforeDrawFixedPoint();
		break;
	case Camera::MODE::FREE:
		SetBeforeDrawFree();
		break;
	}

	// カメラの設定(位置と角度による制御)
	SetCameraPositionAndAngle(
		pos_,
		angles_.x,
		angles_.y,
		angles_.z
	);
}

void Camera::SetBeforeDrawFixedPoint(void)
{
	// 何もしない
}

void Camera::SetBeforeDrawFree(void)
{
	// 経過時間の更新(移動と重力の両方で使う)
	UpdateDeltaTime();

	// マウスで向きを変える
	RotateByMouse();

	// キーで水平に移動
	MoveXYZDirection();

	// ジャンプの入力
	ProcessJump();

	// 壁・重力・地面の判定
	body_.Update(pos_, deltaTime_);
}

void Camera::DrawDebug(void)
{
	DrawFormatString(
		0, 10, 0xffffff,
		"カメラ座標　 ：(% .1f, % .1f, % .1f)",
		pos_.x, pos_.y, pos_.z
	);

	DrawFormatString(
		0, 30, 0xffffff,
		"カメラ角度　 ：(% .1f, % .1f, % .1f)",
		AsoUtility::Rad2DegF(angles_.x),
		AsoUtility::Rad2DegF(angles_.y),
		AsoUtility::Rad2DegF(angles_.z)
	);
}

void Camera::Release(void)
{
}

void Camera::ChangeMode(MODE mode)
{
	// カメラモードの変更
	mode_ = mode;

	// 変更時の初期化処理
	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
		// カーソルを表示に戻す
		SetMouseDispFlag(TRUE);
		break;
	case Camera::MODE::FREE:
		// カーソルを隠して中央に置く(切り替え直後に視点が飛ばないように)
		SetMouseDispFlag(FALSE);
		ResetMousePos();
		break;
	}
}

void Camera::MoveXYZDirection(void)
{
	auto& ins = InputManager::GetInstance();

	// 矢印キーでカメラの角度を変える
	if (ins.IsNew(KEY_INPUT_DOWN)) { angles_.x += ARROW_ROT_POW; }
	if (ins.IsNew(KEY_INPUT_UP)) { angles_.x -= ARROW_ROT_POW; }
	if (ins.IsNew(KEY_INPUT_RIGHT)) { angles_.y += ARROW_ROT_POW; }
	if (ins.IsNew(KEY_INPUT_LEFT)) { angles_.y -= ARROW_ROT_POW; }

	// 押されたキーの方向を足していく(同時押しで斜めになる)
	VECTOR dir_ = AsoUtility::VECTOR_ZERO;
	if (ins.IsNew(KEY_INPUT_W)) { dir_ = VAdd(dir_, DIR_FRONT); }
	if (ins.IsNew(KEY_INPUT_S)) { dir_ = VAdd(dir_, DIR_BACK); }
	if (ins.IsNew(KEY_INPUT_A)) { dir_ = VAdd(dir_, DIR_LEFT); }
	if (ins.IsNew(KEY_INPUT_D)) { dir_ = VAdd(dir_, DIR_RIGHT); }

	// 打ち消し合って0なら動かない
	if (AsoUtility::EqualsVZero(dir_))
	{
		return;
	}

	// 左右の向き(Y軸回転)だけで方向を回す
	MATRIX mat_ = MGetRotY(angles_.y);
	VECTOR moveDir_ = VTransform(dir_, mat_);

	// 高さは動かさない。Yを消した後で長さを1にそろえ直す
	moveDir_.y = 0.0f;
	moveDir_ = VNorm(moveDir_);

	// 左Shiftを押している間はダッシュ
	float speed_ = MOVE_SPEED;
	if (ins.IsNew(KEY_INPUT_LSHIFT))
	{
		speed_ = DASH_SPEED;
	}

	// 方向×1秒あたりの速さ×経過時間で、FPSに関係なく同じ距離だけ進む
	pos_ = VAdd(pos_, VScale(moveDir_, speed_ * deltaTime_));
}

const VECTOR& Camera::GetPos(void) const
{
	return pos_;
}

const VECTOR& Camera::GetAngles(void) const
{
	return angles_;
}

void Camera::RotateByMouse(void)
{
	// 現在のマウス位置
	int mouseX_ = 0;
	int mouseY_ = 0;
	GetMousePoint(&mouseX_, &mouseY_);

	// 画面の中心
	int screenW_ = 0;
	int screenH_ = 0;
	GetDrawScreenSize(&screenW_, &screenH_);
	int centerX_ = screenW_ / CENTER_DIV;
	int centerY_ = screenH_ / CENTER_DIV;

	// 中心からどれだけ動いたか
	int diffX_ = mouseX_ - centerX_;
	int diffY_ = mouseY_ - centerY_;

	// 横移動 → 左右の向き、縦移動 → 上下の向き
	angles_.y += static_cast<float>(diffX_) * MOUSE_SENSITIVITY;
	angles_.x += static_cast<float>(diffY_) * MOUSE_SENSITIVITY;

	// 上下を向きすぎてひっくり返らないように制限
	if (angles_.x > ANGLE_X_LIMIT) { angles_.x = ANGLE_X_LIMIT; }
	if (angles_.x < -ANGLE_X_LIMIT) { angles_.x = -ANGLE_X_LIMIT; }

	// 次のフレームのために中心へ戻す
	ResetMousePos();
}

void Camera::ResetMousePos(void)
{
	int screenW_ = 0;
	int screenH_ = 0;
	GetDrawScreenSize(&screenW_, &screenH_);
	SetMousePoint(screenW_ / CENTER_DIV, screenH_ / CENTER_DIV);
}

void Camera::UpdateDeltaTime(void)
{
	// 今の時刻との差から、前フレームからの経過時間(秒)を求める
	LONGLONG nowTime_ = GetNowHiPerformanceCount();
	deltaTime_ = static_cast<float>(nowTime_ - preTime_) / MICRO_SEC_TO_SEC;
	preTime_ = nowTime_;

	// 長く止まった直後に大きく動かないよう上限をつける
	if (deltaTime_ > DELTA_TIME_MAX)
	{
		deltaTime_ = DELTA_TIME_MAX;
	}
}

void Camera::ProcessJump(void)
{
	auto& ins = InputManager::GetInstance();

	// 地面に乗っているときに、スペースを押した瞬間だけジャンプ
	if (body_.IsGrounded() && ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		// 落下速度は下向きが+なので、上に飛ぶときは-にする
		body_.SetFallSpeed(-JUMP_SPEED);
	}
}

void Camera::ResetState(void)
{
	// 位置と角度を最初に戻す
	pos_ = DERFAULT_POS;
	angles_ = DERFAULT_ANGLES;

	// 当たり判定の体の形を決める(縦向き)
	BodyCollision::PARAM param_ = {
		CAPSULE_RADIUS,
		VGet(0.0f, CAPSULE_TOP_Y, 0.0f),
		VGet(0.0f, CAPSULE_DOWN_Y, 0.0f),
		FOOT_Y
	};
	body_.Init(param_);

	// 経過時間の計測もやり直す(長く測っていないと、最初の1フレームが大きくなる)
	preTime_ = GetNowHiPerformanceCount();
	deltaTime_ = 0.0f;
}

const BodyCollision& Camera::GetBody(void) const
{
	return body_;
}