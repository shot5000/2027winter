#pragma once
#include <map>
#include <memory>
#include "Resource.h"

class ResourceManager
{

public:

	// リソース名(増やすときはここに足して、Init に登録を足す)
	enum class SRC
	{
		STAGE,

		ENEMY_GHOST,
	};

	// 明示的にインスタンスを生成する
	static void CreateInstance(void);

	// 静的インスタンスの取得
	static ResourceManager& GetInstance(void);

	// 静的インスタンスの破棄
	static void DeleteInstance(void);

	// 初期化(リソースの登録。読み込みはLoadが呼ばれたときに行う)
	void Init(void);

	// 解放(読み込んだものを全部消す。登録は残る)
	void Release(void);

	// リソースの読み込み(読み込み済みならそのまま返す)
	const Resource& Load(SRC src);

	// リソースの再読み込み
	const Resource& Reload(SRC src);

	// 読み込み済みのリソースを解放して、読み込み済みの一覧から外す
	void Unload(SRC src);

	// モデルの複製を作る(同じモデルを複数置くとき用)
	int LoadModelDuplicate(SRC src);

	// 複製したモデルを1つだけ消す(敵が倒れたときなど)
	void DeleteModelDuplicate(SRC src, int duplicateId);

private:

	// 静的インスタンス
	static ResourceManager* instance_;

	// 登録されているリソース
	std::map<SRC, std::unique_ptr<Resource>> resourcesMap_;

	// 読み込み済みのリソース
	std::map<SRC, Resource&> loadedMap_;

	// 読み込めなかったときに返すダミー
	Resource dummy_;

	// 外から生成・コピー・破棄できないようにする
	ResourceManager(void);
	ResourceManager(const ResourceManager& manager) = delete;
	~ResourceManager(void);

	// 内部の読み込み
	Resource& LoadInternal(SRC src);

};