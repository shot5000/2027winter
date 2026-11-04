#include "GameResult.h"

int GameResult::money_ = 0;

void GameResult::Reset(void)
{
	money_ = 0;
}

void GameResult::AddMoney(int money)
{
	money_ += money;
}

int GameResult::GetMoney(void)
{
	return money_;
}