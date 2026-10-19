#include <vector>
#include <cmath>
#include "../../Utility/AsoUtility.h"
#include "BodyCollision.h"

// ステージのモデルは全員で共有
int BodyCollision::stageModelId_ = BodyCollision::NO_MODEL;

BodyCollision::BodyCollision(void)
{
	param_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	fallSpeed_ = 0.0f;
	isGravity_ = true;
	isGrounded_ = false;
}

BodyCollision::~BodyCollision(void)
{
}

void BodyCollision::Init(const PARAM& param)
{
	param_ = param;
	fallSpeed_ = 0.0f;
	isGravity_ = true;
	isGrounded_ = false;
}

void BodyCollision::SetStageModel(int modelId)
{
	stageModelId_ = modelId;
}

void BodyCollision::Update(VECTOR& pos, float deltaTime)
{
	// ステージがないときは、何もしない(落ち続けるのを防ぐ)
	if (stageModelId_ == NO_MODEL)
	{
		return;
	}

	// まず壁から押し戻す
	CollisionWall(pos);

	// 重力を使わないなら、ここまで
	if (!isGravity_)
	{
		isGrounded_ = false;
		return;
	}

	// 落ちてから、地面に乗せる
	ApplyGravity(pos, deltaTime);
	CollisionGround(pos);
}

void BodyCollision::SetGravityEnabled(bool isEnabled)
{
	isGravity_ = isEnabled;
	fallSpeed_ = 0.0f;
}

void BodyCollision::SetFallSpeed(float fallSpeed)
{
	fallSpeed_ = fallSpeed;
}

bool BodyCollision::IsGrounded(void) const
{
	return isGrounded_;
}

VECTOR BodyCollision::GetCapsuleTop(const VECTOR& pos) const
{
	VECTOR top_ = pos;
	top_.y += param_.topY_;
	return top_;
}

VECTOR BodyCollision::GetCapsuleDown(const VECTOR& pos) const
{
	VECTOR down_ = pos;
	down_.y += param_.downY_;
	return down_;
}

float BodyCollision::GetRadius(void) const
{
	return param_.radius_;
}

bool BodyCollision::IsHit(
	const VECTOR& pos,
	const BodyCollision& other,
	const VECTOR& otherPos) const
{
	return HitCheck_Capsule_Capsule(
		GetCapsuleTop(pos), GetCapsuleDown(pos), param_.radius_,
		other.GetCapsuleTop(otherPos), other.GetCapsuleDown(otherPos),
		other.GetRadius()) == TRUE;
}

void BodyCollision::DrawDebug(const VECTOR& pos) const
{
	DrawCapsule3D(
		GetCapsuleTop(pos), GetCapsuleDown(pos), param_.radius_,
		DEBUG_DIV_NUM, DEBUG_COLOR, DEBUG_COLOR, FALSE);
}

void BodyCollision::CollisionWall(VECTOR& pos)
{
	if (stageModelId_ == NO_MODEL)
	{
		return;
	}

	// カプセルと当たっている面を全部取る
	MV1_COLL_RESULT_POLY_DIM hits_ = MV1CollCheck_Capsule(
		stageModelId_, ALL_FRAME,
		GetCapsuleTop(pos), GetCapsuleDown(pos), param_.radius_
	);

	// 壁として扱う面の番号と、押し戻す向きと、水平方向の割合を集める
	std::vector<int> wallIndexes_;
	std::vector<VECTOR> pushDirs_;
	std::vector<float> pushRatios_;
	for (int i_ = 0; i_ < hits_.HitNum; i_++)
	{
		const MV1_COLL_RESULT_POLY& poly_ = hits_.Dim[i_];

		// 面の向きが逆でも押し戻せるように、法線を自分の側に向ける
		VECTOR normal_ = poly_.Normal;
		if (VDot(normal_, VSub(pos, poly_.Position[0])) < 0.0f)
		{
			normal_ = VSub(AsoUtility::VECTOR_ZERO, normal_);
		}

		// 床や天井は壁として扱わない(床は足元の線で処理する)
		if (fabsf(normal_.y) > FLOOR_NORMAL_Y)
		{
			continue;
		}

		// 押し戻すのは水平方向だけ(上下に押すと地面の処理と喧嘩する)
		VECTOR pushDir_ = normal_;
		pushDir_.y = 0.0f;
		float ratio_ = VSize(pushDir_);
		if (ratio_ < PUSH_DIR_MIN)
		{
			continue;
		}

		wallIndexes_.push_back(i_);
		pushDirs_.push_back(VNorm(pushDir_));
		pushRatios_.push_back(ratio_);
	}

	// めり込んだ分だけ押し戻す。角などで重なる壁があるので、数回繰り返す
	for (int tryCnt_ = 0; tryCnt_ < PUSH_TRY_COUNT; tryCnt_++)
	{
		bool isPushed_ = false;
		for (size_t w_ = 0; w_ < wallIndexes_.size(); w_++)
		{
			const MV1_COLL_RESULT_POLY& poly_ = hits_.Dim[wallIndexes_[w_]];

			// 面とカプセルの中心の線との距離
			float dist_ = Segment_Triangle_MinLength(
				GetCapsuleDown(pos), GetCapsuleTop(pos),
				poly_.Position[0], poly_.Position[1], poly_.Position[2]);

			// 半径より遠ければ、めり込んでいない
			if (dist_ >= param_.radius_)
			{
				continue;
			}

			// めり込んだ深さ(余裕を足す)
			float depth_ = param_.radius_ - dist_ + PUSH_MARGIN;

			// 水平方向に動かすと、面に対して動く量は割合ぶんだけ減るので割る
			float pushLen_ = depth_ / pushRatios_[w_];
			pos = VAdd(pos, VScale(pushDirs_[w_], pushLen_));
			isPushed_ = true;
		}

		// どの壁にもめり込んでいなければ終わり
		if (!isPushed_)
		{
			break;
		}
	}

	// 取った当たり判定の情報を片付ける
	MV1CollResultPolyDimTerminate(hits_);
}

void BodyCollision::ApplyGravity(VECTOR& pos, float deltaTime)
{
	// 落下速度を増やして、上限で止める
	fallSpeed_ += GRAVITY_POW * deltaTime;
	if (fallSpeed_ > MAX_FALL_SPEED)
	{
		fallSpeed_ = MAX_FALL_SPEED;
	}

	// 下へ移動
	pos.y -= fallSpeed_ * deltaTime;
}

void BodyCollision::CollisionGround(VECTOR& pos)
{
	isGrounded_ = false;

	if (stageModelId_ == NO_MODEL)
	{
		return;
	}

	// 上に向かって動いている間は、地面に吸い付かせない
	if (fallSpeed_ < 0.0f)
	{
		return;
	}

	// カプセルの一番下から、足元の少し下まで線を引く
	// (これより低い段差は上れて、高いものは壁として止まる)
	VECTOR start_ = pos;
	start_.y += param_.downY_ - param_.radius_;
	VECTOR end_ = pos;
	end_.y += param_.footY_ - GROUND_MARGIN;

	MV1_COLL_RESULT_POLY_DIM hits_ = MV1CollCheck_LineDim(
		stageModelId_, ALL_FRAME, start_, end_
	);

	// 当たった中で、一番高い位置を地面とする
	bool isHit_ = false;
	float groundY_ = 0.0f;
	for (int i_ = 0; i_ < hits_.HitNum; i_++)
	{
		float hitY_ = hits_.Dim[i_].HitPosition.y;
		if (!isHit_ || hitY_ > groundY_)
		{
			groundY_ = hitY_;
			isHit_ = true;
		}
	}

	MV1CollResultPolyDimTerminate(hits_);

	if (!isHit_)
	{
		return;
	}

	// 足元を地面の高さに合わせて、落下速度を0に戻す
	pos.y = groundY_ - param_.footY_;
	fallSpeed_ = 0.0f;
	isGrounded_ = true;
}