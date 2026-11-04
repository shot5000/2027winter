#pragma once
#include "SceneBase.h"

class GameClearScene : public SceneBase
{

public:

	// 獲得したお金の表示
	static constexpr const char* MONEY_TEXT = "獲得金額 %d円";
	static constexpr unsigned int MONEY_COLOR = 0xffff00;
	static constexpr int MONEY_POS_X = 400;
	static constexpr int MONEY_POS_Y = 300;

	// コンストラクタ
	GameClearScene(void);

	// デストラクタ
	~GameClearScene(void) override;

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:

};