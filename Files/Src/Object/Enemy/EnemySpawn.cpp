#include <algorithm>
#include <fstream>
#include <DxLib.h>
#include "../../External/json/json.hpp"
#include "EnemySpawn.h"

EnemySpawn::EnemySpawn(void)
	: engine_(std::random_device{}())
{
}

EnemySpawn::~EnemySpawn(void)
{
}

void EnemySpawn::Load(void)
{
	positions_.clear();
	counts_.clear();

	std::ifstream file_(SPAWN_FILE);
	if (!file_.is_open())
	{
		ErrorLogAdd("敵の出現位置のJSONを開けませんでした：");
		ErrorLogAdd(SPAWN_FILE);
		ErrorLogAdd("\n");
		return;
	}

	nlohmann::json json_ = nlohmann::json::parse(file_, nullptr, false);
	if (json_.is_discarded() || !json_.contains("positions") || !json_.contains("counts"))
	{
		ErrorLogAdd("敵の出現位置のJSONの書き方が正しくありません(positions、countsが必要です)。\n");
		return;
	}

	// 候補の座標を読む(数字が足りない座標は飛ばす)
	for (const auto& posJson_ : json_["positions"])
	{
		if (!posJson_.is_array() || posJson_.size() != POS_ELEMENT_NUM)
		{
			continue;
		}
		positions_.push_back(VGet(
			posJson_[0].get<float>(),
			posJson_[1].get<float>(),
			posJson_[2].get<float>()));
	}

	// 種類ごとの数を読む(数字でないものは飛ばす)
	for (const auto& entry_ : json_["counts"].items())
	{
		if (entry_.value().is_number_integer())
		{
			counts_[entry_.key()] = entry_.value().get<int>();
		}
	}

	// 候補をシャッフルする(あとは、先頭から順に取り出すだけでよい)
	std::shuffle(positions_.begin(), positions_.end(), engine_);
}

std::vector<VECTOR> EnemySpawn::Take(const std::string& id)
{
	std::vector<VECTOR> result_;

	auto it_ = counts_.find(id);
	if (it_ == counts_.end())
	{
		return result_;
	}

	// 数だけ取り出す(候補が足りなければ、そこまで)
	for (int i_ = 0; i_ < it_->second; i_++)
	{
		if (positions_.empty())
		{
			ErrorLogAdd("敵の出現位置の候補が足りません。\n");
			break;
		}
		result_.push_back(positions_.back());
		positions_.pop_back();
	}
	return result_;
}

float EnemySpawn::GetRandomAngle(void)
{
	std::uniform_real_distribution<float> dist_(0.0f, DX_TWO_PI_F);
	return dist_(engine_);
}