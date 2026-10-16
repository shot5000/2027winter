#include "../../Manager/InputManager.h"
#include "DirtManager.h"

DirtManager::DirtManager(void)
{
}

DirtManager::~DirtManager(void)
{
}

void DirtManager::Init(void)
{
	// 座標のリストの数だけ汚れを作る
	dirts_.clear();
	for (int i_ = 0; i_ < DIRT_COUNT; i_++)
	{
		dirts_.emplace_back(DIRT_POS_LIST[i_]);
	}

	// 掃除した数を0に戻す
	cleanCount_ = 0;

	ResetHold();

	// 経過時間の計測を開始
	preTime_ = GetNowHiPerformanceCount();
	deltaTime_ = 0.0f;
}

void DirtManager::Update(void)
{
	UpdateDeltaTime();

	auto& ins = InputManager::GetInstance();

	// Eを押していない、または汚れを狙っていないなら、長押しは無効
	int aimedIndex_ = GetAimedIndex();
	if (!ins.IsNew(KEY_INPUT_E) || aimedIndex_ == NO_INDEX)
	{
		ResetHold();
		return;
	}

	// 別の汚れに狙いが移ったら、時間を最初から数え直す
	if (aimedIndex_ != holdIndex_)
	{
		holdIndex_ = aimedIndex_;
		holdTime_ = 0.0f;
	}

	holdTime_ += deltaTime_;

	// 規定の時間を超えたら、その汚れを消す
	if (holdTime_ >= CLEAN_TIME)
	{
		dirts_.erase(dirts_.begin() + holdIndex_);
		cleanCount_++;
		ResetHold();
	}
}

void DirtManager::Draw(void)
{
	for (Dirt& dirt_ : dirts_)
	{
		dirt_.Draw();
	}
}

bool DirtManager::IsAiming(void) const
{
	return GetAimedIndex() != NO_INDEX;
}

int DirtManager::GetCleanCount(void) const
{
	return cleanCount_;
}

bool DirtManager::IsAllCleaned(void) const
{
	return cleanCount_ >= DIRT_COUNT;

}

int DirtManager::GetAimedIndex(void) const
{
	// 画面の中心
	int screenW_ = 0;
	int screenH_ = 0;
	GetDrawScreenSize(&screenW_, &screenH_);
	float centerX_ = static_cast<float>(screenW_ / CENTER_DIV);
	float centerY_ = static_cast<float>(screenH_ / CENTER_DIV);

	// 画面中央から奥へ向かう線の、始点と終点
	VECTOR start_ = ConvScreenPosToWorldPos(VGet(centerX_, centerY_, SCREEN_Z_NEAR));
	VECTOR end_ = ConvScreenPosToWorldPos(VGet(centerX_, centerY_, SCREEN_Z_FAR));

	VECTOR cameraPos_ = GetCameraPosition();

	// 範囲内で、線に当たっていて、一番近いものを探す
	int aimedIndex_ = NO_INDEX;
	float minDist_ = CLEAN_DISTANCE;
	for (int i_ = 0; i_ < static_cast<int>(dirts_.size()); i_++)
	{
		float dist_ = dirts_[i_].GetDistance(cameraPos_);
		if (dist_ > minDist_)
		{
			continue;
		}
		if (!dirts_[i_].IsHitLine(start_, end_))
		{
			continue;
		}
		aimedIndex_ = i_;
		minDist_ = dist_;
	}

	return aimedIndex_;
}

void DirtManager::UpdateDeltaTime(void)
{
	// 今の時刻との差から、前フレームからの経過時間(秒)を求める
	LONGLONG nowTime_ = GetNowHiPerformanceCount();
	deltaTime_ = static_cast<float>(nowTime_ - preTime_) / MICRO_SEC_TO_SEC;
	preTime_ = nowTime_;

	// 長く止まった直後に一気に進まないよう上限をつける
	if (deltaTime_ > DELTA_TIME_MAX)
	{
		deltaTime_ = DELTA_TIME_MAX;
	}
}

void DirtManager::ResetHold(void)
{
	holdIndex_ = NO_INDEX;
	holdTime_ = 0.0f;
}