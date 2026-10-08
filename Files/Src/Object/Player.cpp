#include "Player.h"
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Common/AnimationController.h"

Player::Player(void)
{
}

Player::~Player(void)
{
}

void Player::Init(void)
{
	// モデルの読み込み
	modelId_ = MV1LoadModel(
		(Application::PATH_MODEL + "Player/Idle.mv1").c_str());	modelId_ = MV1LoadModel("data/Model/Player/Player.mv1");
	
	// モデルの角度
	angles_ = { 0.0f, 0.0f, 0.0f };
	localAngles_ = { 0.0f, AsoUtility::Deg2RadF(180.0f), 0.0f };

	// モデルの回転行列
	MATRIX mat = MGetIdent();
	mat = MMult(mat, MGetRotX(angles_.x));
	mat = MMult(mat, MGetRotY(angles_.y));
	mat = MMult(mat, MGetRotZ(angles_.z));

	// モデルのローカル回転行列
	MATRIX localMat = MGetIdent();
	localMat = MMult(localMat, MGetRotX(localAngles_.x));
	localMat = MMult(localMat, MGetRotY(localAngles_.y));
	localMat = MMult(localMat, MGetRotZ(localAngles_.z));

	// 行列の合成(子, 親と指定すると親⇒子の順に適用される)
	mat = MMult(localMat, mat);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(modelId_, mat);

	// モデルの位置
	pos_ = AsoUtility::VECTOR_ZERO;
	// モデルの位置設定
	MV1SetPosition(modelId_, pos_);

	// モデルアニメーション制御の初期化
	animationController_ = new AnimationController(modelId_);

	// アニメーションの追加
	animationController_->Add(
		static_cast<int>(ANIM_TYPE::IDLE),
		30.0f,
		(Application::PATH_MODEL + "Player/Idle.mv1").c_str()
	);

	animationController_->Add(
		static_cast<int>(ANIM_TYPE::WALK),
		30.0f,
		(Application::PATH_MODEL + "Player/Walk.mv1").c_str()
	);

	// 初期アニメーションの再生
	animationController_->Play(static_cast<int>(ANIM_TYPE::IDLE));
}

void Player::Update(void)
{
	// モデルのY軸回転
	angles_.y += AsoUtility::Deg2RadF(0.5f);

	// モデルの回転行列
	MATRIX mat = MGetIdent();
	mat = MMult(mat, MGetRotX(angles_.x));
	mat = MMult(mat, MGetRotY(angles_.y));
	mat = MMult(mat, MGetRotZ(angles_.z));

	// モデルのローカル回転行列
	MATRIX localMat = MGetIdent();
	localMat = MMult(localMat, MGetRotX(localAngles_.x));
	localMat = MMult(localMat, MGetRotY(localAngles_.y));
	localMat = MMult(localMat, MGetRotZ(localAngles_.z));

	// 行列の合成(子, 親と指定すると親⇒子の順に適用される)
	mat = MMult(localMat, mat);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(modelId_, mat);

	// アニメーションコントローラーの更新
	animationController_->Update();
}

void Player::Draw(void)
{
	// モデルの描画
	MV1DrawModel(modelId_);

	DrawFormatString(
		0, 50, 0xffffff,
		"キャラ角度　　：(%.1f, %.1f, %.1f)",
		AsoUtility::Rad2DegF(angles_.x),
		AsoUtility::Rad2DegF(angles_.y),
		AsoUtility::Rad2DegF(angles_.z)
	);
}

void Player::Release(void)
{
	// モデルの削除
	MV1DeleteModel(modelId_);
}
