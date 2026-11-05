#pragma once
#include <map>
#include <string>
#include "../Object/Enemy/EnemyParam.h"
#include "../Object/Item/ItemParam.h"

class ParamManager
{

public:

	// JSONファイルの場所
	static constexpr const char* ENEMY_FILE = "Data/Json/EnemyData.json";
	static constexpr const char* ITEM_FILE = "Data/Json/ItemData.json";

	// 明示的にインスタンスを生成する
	static void CreateInstance(void);

	// 静的インスタンスの取得
	static ParamManager& GetInstance(void);

	// 静的インスタンスの破棄
	static void DeleteInstance(void);

	// 読み込み(読み直すときも、これを呼ぶ)
	void Load(void);

	// 敵の性能の取得(名前が見つからないときは、初期値を返す)
	const EnemyParam& GetEnemy(const std::string& id) const;

	// アイテムの性能の取得(名前が見つからないときは、初期値を返す)
	const ItemParam& GetItem(const std::string& id) const;

private:

	// 静的インスタンス
	static ParamManager* instance_;

	// 名前ごとの性能
	std::map<std::string, EnemyParam> enemyMap_;
	std::map<std::string, ItemParam> itemMap_;

	// 見つからなかったときに返す、初期値の性能
	EnemyParam defaultEnemy_;
	ItemParam defaultItem_;

	// 外から生成・コピー・破棄できないようにする
	ParamManager(void);
	ParamManager(const ParamManager& manager) = delete;
	~ParamManager(void);

	// ファイルごとの読み込み
	void LoadEnemy(void);
	void LoadItem(void);

};