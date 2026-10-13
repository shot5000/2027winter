#pragma once
#include "SceneBase.h"

class GameClearScene : public SceneBase
{

public:

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