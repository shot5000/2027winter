#pragma once
#include <string>
#include <vector>

class Resource
{

public:

	// リソースの種類
	enum class TYPE
	{
		NONE,
		MODEL,
	};

	// 読み込めていないときのハンドルID(DxLibの読み込み失敗と同じ値)
	static constexpr int INVALID_HANDLE = -1;

	// コンストラクタ
	Resource(void) = default;

	// コンストラクタ(種類と、置き場所からの相対パスを受け取る)
	Resource(TYPE type, const std::string& path);

	// デストラクタ
	~Resource(void);

	// 読み込み
	void Load(void);

	// 解放
	void Release(void);

	// 種類
	TYPE type_ = TYPE::NONE;

	// 読み込み先のパス
	std::string path_ = "";

	// ハンドルID
	int handleId_ = INVALID_HANDLE;

	// 複製したモデルのハンドルID
	std::vector<int> duplicateModelIds_ = {};

};