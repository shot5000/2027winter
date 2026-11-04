#pragma once

class GameResult
{

public:

	// Š‹à‚ğ0‚É–ß‚·(ƒQ[ƒ€‚ğn‚ß‚é‚Æ‚«‚ÉŒÄ‚Ô)
	static void Reset(void);

	// Š‹à‚ğ‘‚â‚·
	static void AddMoney(int money);

	// Š‹à‚Ìæ“¾
	static int GetMoney(void);

private:

	// Š‹à
	static int money_;

	// ¶¬‚³‚¹‚È‚¢(ŠÖ”‚¾‚¯‚ğg‚¤)
	GameResult(void) = delete;

};