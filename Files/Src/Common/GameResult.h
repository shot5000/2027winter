#pragma once
#include <vector>

class GameResult
{

public:

	// 一番最初の所持金
	static constexpr int START_MONEY = 1000;

	// 最初の状態に戻す(所持金を最初の金額にして、購入したアイテムも消す)
	// 新しく遊び始めるときに呼ぶ
	static void Reset(void);

	// 今回のゲームで稼いだ額を0に戻す(ゲームを始めるときに呼ぶ)
	static void ResetEarned(void);

	// 所持金を増やす(稼いだ額も増える)
	static void AddMoney(int money);

	// 所持金で払えるか
	static bool CanAfford(int money);

	// お金を使う(足りなければ使わずに false)
	static bool SpendMoney(int money);

	// 所持金の取得
	static int GetMoney(void);

	// 今回のゲームで稼いだ額の取得
	static int GetEarned(void);

	// 購入したアイテムを、一覧に足す(アイテムの番号を渡す)
	static void AddPurchased(int itemId);

	// 購入したアイテムの一覧の取得
	static const std::vector<int>& GetPurchased(void);

	// 購入したアイテムの一覧を空にする(ゲームが終わったときに呼ぶ。持ち越せないため)
	static void ClearPurchased(void);

private:

	// 所持金(ゲームをまたいで持ち越す)
	static int money_;

	// 今回のゲームで稼いだ額
	static int earned_;

	// 購入したアイテム(次のゲームの昇降口に置く)
	static std::vector<int> purchased_;

	// 生成させない(関数だけを使う)
	GameResult(void) = delete;

};