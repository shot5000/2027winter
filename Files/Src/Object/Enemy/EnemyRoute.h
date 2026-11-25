#pragma once
#include <vector>
#include <DxLib.h>

// 敵が巡回する道(地点の並び)。最後の地点まで行ったら、最初に戻る
class EnemyRoute
{

public:

	// 道のJSONの場所
	static constexpr const char* ROUTE_FILE = "Data/Json/EnemyRoute.json";

	// 地点に着いたとみなす距離(水平)
	static constexpr float ARRIVE_DIST = 30.0f;

	// 地点1つに書く数字の数(x, y, z)
	static constexpr int POS_ELEMENT_NUM = 3;

	// 確認用の表示：球の半径、分割数、床からの高さ、色(向かっている地点は色を変える)
	static constexpr float DEBUG_RADIUS = 20.0f;
	static constexpr int DEBUG_DIV_NUM = 12;
	static constexpr float DEBUG_OFFSET_Y = 30.0f;
	static constexpr unsigned int DEBUG_COLOR = 0x00ff00;
	static constexpr unsigned int DEBUG_TARGET_COLOR = 0xffff00;

	// コンストラクタ
	EnemyRoute(void);

	// デストラクタ
	~EnemyRoute(void);

	// JSONから、全員共通の道を読む(読めなければ空)
	static std::vector<VECTOR> Load(void);

	// 道を決める(posにいちばん近い地点から始める)
	void Init(const std::vector<VECTOR>& points, const VECTOR& pos);

	// 道が決まっていないか
	bool IsEmpty(void) const;

	// 今向かっている地点
	const VECTOR& GetTarget(void) const;

	// 今の地点に着いていたら、次の地点に変える
	void Update(const VECTOR& pos);

	// posにいちばん近い地点を、向かう地点にする
	void SelectNearest(const VECTOR& pos);

	// 地点の球と、地点をつなぐ線を描く(確認用)
	void DrawDebug(void) const;

private:

	// 地点の並び
	std::vector<VECTOR> points_;

	// 今向かっている地点の番号
	int index_;

	// 水平方向の距離
	static float HorizontalDist(const VECTOR& a, const VECTOR& b);

};