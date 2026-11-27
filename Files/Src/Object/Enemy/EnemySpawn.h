#pragma once
#include <map>
#include <random>
#include <string>
#include <vector>
#include <DxLib.h>

// 敵の出現位置を、JSONの候補からランダムに選ぶ
class EnemySpawn
{

public:

	// 出現位置のJSONの場所
	static constexpr const char* SPAWN_FILE = "Data/Json/EnemySpawn.json";

	// 座標の要素の数(x, y, z)
	static constexpr size_t POS_ELEMENT_NUM = 3;

	// コンストラクタ
	EnemySpawn(void);

	// デストラクタ
	~EnemySpawn(void);

	// JSONを読んで、候補をシャッフルする(読めなければ、候補は空)
	void Load(void);

	// 種類の名前(PARAM_ID)の数だけ、残りの候補から選んで取り出す
	std::vector<VECTOR> Take(const std::string& id);

	// 最初の向き(Y軸の角度)を、ランダムに決める
	float GetRandomAngle(void);

private:

	// 残りの出現位置の候補
	std::vector<VECTOR> positions_;

	// 種類ごとの数
	std::map<std::string, int> counts_;

	// 乱数
	std::mt19937 engine_;

};