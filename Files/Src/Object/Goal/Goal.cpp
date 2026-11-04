#include <cmath>
#include <utility>
#include <DxLib.h>
#include "../../Manager/InputManager.h"
#include "../../Manager/SceneManager.h"
#include "Goal.h"

Goal::Goal(void)
{
	holdTime_ = 0.0f;
	minPos_ = POS;
	maxPos_ = POS;
}

Goal::~Goal(void)
{
}

void Goal::Init(void)
{
	holdTime_ = 0.0f;

	// 足元の中心から、箱の両端の角を求める
	minPos_ = VGet(POS.x - WIDTH / HALF_DIV, POS.y, POS.z - DEPTH / HALF_DIV);
	maxPos_ = VGet(POS.x + WIDTH / HALF_DIV, POS.y + HEIGHT, POS.z + DEPTH / HALF_DIV);
}

bool Goal::Update(void)
{
	// ドアを狙って、Eを押している間だけ、長押しの時間を進める
	bool isHold_ = IsAiming() && InputManager::GetInstance().IsNew(KEY_INPUT_E);
	if (!isHold_)
	{
		holdTime_ = 0.0f;
		return false;
	}

	holdTime_ += SceneManager::GetInstance().GetDeltaTime();

	// 規定の時間を超えたら、ドアが開いた
	if (holdTime_ >= OPEN_TIME)
	{
		holdTime_ = 0.0f;
		return true;
	}

	return false;
}

void Goal::Draw(void) const
{
	// 狙っているときは、色を変えて分かりやすくする
	unsigned int color_ = NORMAL_COLOR;
	if (IsAiming())
	{
		color_ = AIM_COLOR;
	}

	// 仮の見た目(箱)。モデルができたら、ここを差し替える
	DrawCube3D(minPos_, maxPos_, color_, color_, TRUE);
}

bool Goal::IsAiming(void) const
{
	// カメラからドアの中心まで遠いときは、狙えない
	VECTOR center_ = VScale(VAdd(minPos_, maxPos_), 1.0f / HALF_DIV);
	if (VSize(VSub(center_, GetCameraPosition())) > OPEN_DISTANCE)
	{
		return false;
	}

	// 画面の中心から奥へ向かう線の、始点と終点
	int screenW_ = 0;
	int screenH_ = 0;
	GetDrawScreenSize(&screenW_, &screenH_);
	float centerX_ = static_cast<float>(screenW_ / CENTER_DIV);
	float centerY_ = static_cast<float>(screenH_ / CENTER_DIV);
	VECTOR start_ = ConvScreenPosToWorldPos(VGet(centerX_, centerY_, SCREEN_Z_NEAR));
	VECTOR end_ = ConvScreenPosToWorldPos(VGet(centerX_, centerY_, SCREEN_Z_FAR));

	return IsHitLine(start_, end_);
}

float Goal::GetHoldRate(void) const
{
	return holdTime_ / OPEN_TIME;
}

bool Goal::IsHitLine(const VECTOR& start, const VECTOR& end) const
{
	// 軸(X、Y、Z)ごとに、線が箱の中にいる範囲(0.0?1.0の割合)を絞り込む
	VECTOR dir_ = VSub(end, start);
	const float startValues_[AXIS_COUNT] = { start.x, start.y, start.z };
	const float dirValues_[AXIS_COUNT] = { dir_.x, dir_.y, dir_.z };
	const float minValues_[AXIS_COUNT] = { minPos_.x, minPos_.y, minPos_.z };
	const float maxValues_[AXIS_COUNT] = { maxPos_.x, maxPos_.y, maxPos_.z };

	float near_ = 0.0f;
	float far_ = 1.0f;
	for (int i_ = 0; i_ < AXIS_COUNT; i_++)
	{
		// 線がその軸と平行なら、始点が箱の幅の中にあるかだけを見る
		if (fabsf(dirValues_[i_]) < PARALLEL_EPSILON)
		{
			if (startValues_[i_] < minValues_[i_] || startValues_[i_] > maxValues_[i_])
			{
				return false;
			}
			continue;
		}

		// 箱の手前の面と奥の面に、線が当たる割合
		float t1_ = (minValues_[i_] - startValues_[i_]) / dirValues_[i_];
		float t2_ = (maxValues_[i_] - startValues_[i_]) / dirValues_[i_];
		if (t1_ > t2_)
		{
			std::swap(t1_, t2_);
		}

		// 範囲を絞り込んで、なくなったら、通らない
		if (t1_ > near_)
		{
			near_ = t1_;
		}
		if (t2_ < far_)
		{
			far_ = t2_;
		}
		if (near_ > far_)
		{
			return false;
		}
	}

	return true;
}