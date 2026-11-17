#pragma once
#include <string>
#include <vector>
#include "../Collision/BodyCollision.h"
#include <DxLib.h>

class Stage
{
public:

	// モデルの拡大率(1m = 100)
	static constexpr float SCALE = 1.0f;

	// モデルの位置(床の高さがY=0)
	static constexpr VECTOR POS = { -2000.0f, 0.0f, 0.0f };

	// 当たり判定の分割数(細かいほど正確で、重くなる)
	static constexpr int COLL_DIV_X = 40;
	static constexpr int COLL_DIV_Y = 8;
	static constexpr int COLL_DIV_Z = 20;

	// マテリアルの環境光の色(1.0fで、全体の明るさがそのまま反映される)
	static constexpr float MATERIAL_AMBIENT = 1.0f;
	static constexpr float MATERIAL_ALPHA = 1.0f;

	// ステージの壁や床になるフレームの名前の頭(扉は別に扱う)
	static constexpr const char* STAGE_FRAME_PREFIX = "Stage_";

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

};