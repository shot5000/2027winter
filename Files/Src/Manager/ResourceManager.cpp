#include <DxLib.h>
#include "ResourceManager.h"

ResourceManager* ResourceManager::instance_ = nullptr;

ResourceManager::ResourceManager(void)
{
}

ResourceManager::~ResourceManager(void)
{
}

void ResourceManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new ResourceManager();
	}
}

ResourceManager& ResourceManager::GetInstance(void)
{
	return *instance_;
}

void ResourceManager::DeleteInstance(void)
{
	if (instance_ == nullptr)
	{
		return;
	}

	instance_->Release();
	instance_->resourcesMap_.clear();

	delete instance_;
	instance_ = nullptr;
}

void ResourceManager::Init(void)
{
	// ステージ
	resourcesMap_.emplace(SRC::STAGE,
		std::make_unique<Resource>(Resource::TYPE::MODEL, "Stage.mv1"));

	// エネミー
	resourcesMap_.emplace(SRC::ENEMY_GHOST,
		std::make_unique<Resource>(Resource::TYPE::MODEL, "Enemy/Rat.mv1"));
}

void ResourceManager::Release(void)
{
	// 読み込み済みのものだけ解放する
	for (auto& pair_ : loadedMap_)
	{
		pair_.second.Release();
	}
	loadedMap_.clear();
}

const Resource& ResourceManager::Load(SRC src)
{
	Resource& res_ = LoadInternal(src);

	// 登録されていないときは、ダミーを返す
	if (res_.type_ == Resource::TYPE::NONE)
	{
		return dummy_;
	}
	return res_;
}

const Resource& ResourceManager::Reload(SRC src)
{
	// 読み込んでいないものは、再読み込みの対象外
	auto loadedPair_ = loadedMap_.find(src);
	if (loadedPair_ == loadedMap_.end())
	{
		return dummy_;
	}

	loadedPair_->second.Release();
	loadedMap_.erase(loadedPair_);

	return Load(src);
}

void ResourceManager::Unload(SRC src)
{
	// 読み込んでいないものは、何もしない
	auto loadedPair_ = loadedMap_.find(src);
	if (loadedPair_ == loadedMap_.end())
	{
		return;
	}

	loadedPair_->second.Release();
	loadedMap_.erase(loadedPair_);
}

int ResourceManager::LoadModelDuplicate(SRC src)
{
	Resource& res_ = LoadInternal(src);
	if (res_.type_ != Resource::TYPE::MODEL
		|| res_.handleId_ == Resource::INVALID_HANDLE)
	{
		return Resource::INVALID_HANDLE;
	}

	// 複製したIDは、解放のときにまとめて消すために覚えておく
	int duplicateId_ = MV1DuplicateModel(res_.handleId_);
	res_.duplicateModelIds_.push_back(duplicateId_);

	return duplicateId_;
}

Resource& ResourceManager::LoadInternal(SRC src)
{
	// 読み込み済みなら、そのまま返す
	auto loadedPair_ = loadedMap_.find(src);
	if (loadedPair_ != loadedMap_.end())
	{
		return loadedPair_->second;
	}

	// 登録されているか
	auto registerPair_ = resourcesMap_.find(src);
	if (registerPair_ == resourcesMap_.end())
	{
		return dummy_;
	}

	// 読み込んで、読み込み済みの一覧に入れる
	Resource& res_ = *registerPair_->second;
	res_.Load();
	loadedMap_.emplace(src, res_);

	return res_;
}