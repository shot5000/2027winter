#include "GameResult.h"

int GameResult::money_ = GameResult::START_MONEY;
int GameResult::earned_ = 0;
std::vector<int> GameResult::purchased_;

void GameResult::Reset(void)
{
	money_ = START_MONEY;
	earned_ = 0;
	purchased_.clear();
}

void GameResult::ResetEarned(void)
{
	earned_ = 0;
}

void GameResult::AddMoney(int money)
{
	money_ += money;
	earned_ += money;
}

bool GameResult::CanAfford(int money)
{
	return money_ >= money;
}

bool GameResult::SpendMoney(int money)
{
	// ‘«‚è‚È‚¢‚Æ‚«‚ÍAg‚í‚È‚¢
	if (!CanAfford(money))
	{
		return false;
	}

	money_ -= money;
	return true;
}

int GameResult::GetMoney(void)
{
	return money_;
}

int GameResult::GetEarned(void)
{
	return earned_;
}

void GameResult::AddPurchased(int itemId)
{
	purchased_.push_back(itemId);
}

const std::vector<int>& GameResult::GetPurchased(void)
{
	return purchased_;
}

void GameResult::ClearPurchased(void)
{
	purchased_.clear();
}