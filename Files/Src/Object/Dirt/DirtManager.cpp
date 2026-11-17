#include <algorithm>
#include <fstream>
#include <numeric>
#include <random>
#include "../../External/json/json.hpp"
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
	// JSONの候補から、汚れの数だけランダムに位置を選んで汚れを作る
	dirts_.clear();
	if (LoadSpawnPositions())
	{
		for (const VECTOR& pos_ : PickSpawnPositions(DIRT_COUNT))
		{
			dirts_.emplace_back(pos_);
		}
	}

	// 掃除した数を0に戻す
	cleanCount_ = 0;

	ResetHold();

	// 経過時間の計測を開始
	preTime_ = GetNowHiPerformanceCount();
	deltaTime_ = 0.0f;
}

int DirtManager::Update(bool canClean, float cleanTime, float cleanReach)
{
	UpdateDeltaTime();

	// 掃除できないとき(掃除できるアイテムを持っていないとき)は、長押しの記録も消す
	if (!canClean)
	{
		ResetHold();
		return NO_REWARD;
	}

	auto& ins = InputManager::GetInstance();

	// Eを押していない、または汚れを狙っていないなら、長押しは無効
	int aimedIndex_ = GetAimedIndex(cleanReach);
	if (!ins.IsNew(KEY_INPUT_E) || aimedIndex_ == NO_INDEX)
	{
		ResetHold();
		return NO_REWARD;
	}

	// 別の汚れに狙いが移ったら、時間を最初から数え直す
	if (aimedIndex_ != holdIndex_)
	{
		holdIndex_ = aimedIndex_;
		holdTime_ = 0.0f;
	}

	holdTime_ += deltaTime_;

	// 規定の時間を超えたら、その汚れを消して、掃除した数を増やし、報酬を返す
	if (holdTime_ >= cleanTime)
	{
		dirts_.erase(dirts_.begin() + holdIndex_);
		cleanCount_++;
		ResetHold();
		return DIRT_REWARD;
	}

	return NO_REWARD;
}

void DirtManager::Draw(void)
{
	for (Dirt& dirt_ : dirts_)
	{
		dirt_.Draw();
	}
}

bool DirtManager::IsAiming(float cleanReach) const
{
	return GetAimedIndex(cleanReach) != NO_INDEX;
}

int DirtManager::GetCleanCount(void) const
{
	return cleanCount_;
}

bool DirtManager::LoadSpawnPositions(void)
{
	spawnPositions_.clear();

	std::ifstream file_(SPAWN_JSON_PATH);
	if (!file_.is_open())
	{
		return false;
	}

	// 書き方が間違っていても止まらないよう、例外なしで読む
	nlohmann::json json_ = nlohmann::json::parse(file_, nullptr, false);
	if (json_.is_discarded() || !json_.contains("spawnPositions"))
	{
		return false;
	}

	for (const auto& data_ : json_["spawnPositions"])
	{
		// 数字が3つ並んでいないものは飛ばす
		if (!data_.is_array() || data_.size() != POS_ELEMENT_NUM)
		{
			continue;
		}

		VECTOR pos_ = VGet(
			data_[0].get<float>(),
			data_[1].get<float>(),
			data_[2].get<float>());
		spawnPositions_.push_back(pos_);
	}

	return !spawnPositions_.empty();
}

std::vector<VECTOR> DirtManager::PickSpawnPositions(int num_) const
{
	// 候補の番号を並べてシャッフルし、先頭から使う(同じ場所が重ならない)
	int candidateNum_ = static_cast<int>(spawnPositions_.size());
	std::vector<int> indexes_(candidateNum_);
	std::iota(indexes_.begin(), indexes_.end(), 0);

	std::random_device seed_;
	std::mt19937 engine_(seed_());
	std::shuffle(indexes_.begin(), indexes_.end(), engine_);

	// 候補より多くは選べない
	int pickNum_ = num_;
	if (pickNum_ > candidateNum_)
	{
		pickNum_ = candidateNum_;
	}

	std::vector<VECTOR> positions_;
	for (int i_ = 0; i_ < pickNum_; i_++)
	{
		positions_.push_back(spawnPositions_[indexes_[i_]]);
	}
	return positions_;
}

int DirtManager::GetAimedIndex(float cleanReach) const
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
	float minDist_ = cleanReach;
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