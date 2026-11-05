#include <fstream>
#include <DxLib.h>
#include "../External/json/json.hpp"
#include "ParamManager.h"

ParamManager* ParamManager::instance_ = nullptr;

namespace
{
	// JSONファイルを読む(読めなかったら false。理由は Log.txt に残す)
	bool ReadJson(const char* path, nlohmann::json& json)
	{
		std::ifstream ifs_(path);
		if (!ifs_.is_open())
		{
			ErrorLogAdd("JSONを開けませんでした。初期値を使います：");
			ErrorLogAdd(path);
			ErrorLogAdd("\n");
			return false;
		}

		try
		{
			ifs_ >> json;
		}
		catch (const nlohmann::json::exception&)
		{
			ErrorLogAdd("JSONの書き方が正しくありません。初期値を使います：");
			ErrorLogAdd(path);
			ErrorLogAdd("\n");
			return false;
		}

		return true;
	}
}

ParamManager::ParamManager(void)
{
}

ParamManager::~ParamManager(void)
{
}

void ParamManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new ParamManager();
	}
}

ParamManager& ParamManager::GetInstance(void)
{
	return *instance_;
}

void ParamManager::DeleteInstance(void)
{
	if (instance_ == nullptr)
	{
		return;
	}

	delete instance_;
	instance_ = nullptr;
}

void ParamManager::Load(void)
{
	LoadEnemy();
	LoadItem();
}

const EnemyParam& ParamManager::GetEnemy(const std::string& id) const
{
	auto it_ = enemyMap_.find(id);
	if (it_ == enemyMap_.end())
	{
		return defaultEnemy_;
	}
	return it_->second;
}

const ItemParam& ParamManager::GetItem(const std::string& id) const
{
	auto it_ = itemMap_.find(id);
	if (it_ == itemMap_.end())
	{
		return defaultItem_;
	}
	return it_->second;
}

void ParamManager::LoadEnemy(void)
{
	enemyMap_.clear();

	nlohmann::json json_;
	if (!ReadJson(ENEMY_FILE, json_))
	{
		return;
	}

	try
	{
		for (auto& entry_ : json_.items())
		{
			const nlohmann::json& data_ = entry_.value();

			// 書かれていない項目は、初期値のままにする
			EnemyParam param_;
			param_.hp_ = data_.value("hp", param_.hp_);
			param_.searchSpeed_ = data_.value("searchSpeed", param_.searchSpeed_);
			param_.chaseSpeed_ = data_.value("chaseSpeed", param_.chaseSpeed_);
			param_.fanRange_ = data_.value("fanRange", param_.fanRange_);
			param_.fanAngleDeg_ = data_.value("fanAngleDeg", param_.fanAngleDeg_);
			param_.chaseRange_ = data_.value("chaseRange", param_.chaseRange_);
			param_.attackPower_ = data_.value("attackPower", param_.attackPower_);

			enemyMap_[entry_.key()] = param_;
		}
	}
	catch (const nlohmann::json::exception&)
	{
		ErrorLogAdd("EnemyData.jsonの値の書き方が正しくありません(数字が文字になっているなど)。\n");
	}
}

void ParamManager::LoadItem(void)
{
	itemMap_.clear();

	nlohmann::json json_;
	if (!ReadJson(ITEM_FILE, json_))
	{
		return;
	}

	try
	{
		for (auto& entry_ : json_.items())
		{
			const nlohmann::json& data_ = entry_.value();

			// 書かれていない項目は、初期値のままにする
			ItemParam param_;
			param_.attackPower_ = data_.value("attackPower", param_.attackPower_);
			param_.cooldownTime_ = data_.value("cooldownTime", param_.cooldownTime_);
			param_.cleanSpeed_ = data_.value("cleanSpeed", param_.cleanSpeed_);
			param_.reach_ = data_.value("reach", param_.reach_);
			param_.moveSpeedRate_ = data_.value("moveSpeedRate", param_.moveSpeedRate_);
			param_.durability_ = data_.value("durability", param_.durability_);
			param_.attackVolume_ = data_.value("attackVolume", param_.attackVolume_);
			param_.cleanVolume_ = data_.value("cleanVolume", param_.cleanVolume_);
			param_.price_ = data_.value("price", param_.price_);

			itemMap_[entry_.key()] = param_;
		}
	}
	catch (const nlohmann::json::exception&)
	{
		ErrorLogAdd("ItemData.jsonの値の書き方が正しくありません(数字が文字になっているなど)。\n");
	}
}