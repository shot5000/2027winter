#pragma once
#include "Collision/BodyCollision.h"
#include <DxLib.h>

class Stage
{
public:

	Stage(void);
	~Stage(void);

	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	// モデルのハンドルIDの取得
	int GetModelId(void) const;

private:

	// ステージモデルのハンドルID
	int modelId_;

	// ステージモデルの位置
	VECTOR pos_;

};
