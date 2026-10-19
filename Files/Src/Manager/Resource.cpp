#include <DxLib.h>
#include "../Application.h"
#include "Resource.h"

Resource::Resource(TYPE type, const std::string& path)
{
	type_ = type;

	// 種類ごとの置き場所を、パスの前につなげる
	switch (type_)
	{
	case Resource::TYPE::MODEL:
		path_ = Application::PATH_MODEL + path;
		break;
	}
}

Resource::~Resource(void)
{
}

void Resource::Load(void)
{
	switch (type_)
	{
	case Resource::TYPE::MODEL:
		handleId_ = MV1LoadModel(path_.c_str());
		break;
	}
}

void Resource::Release(void)
{
	switch (type_)
	{
	case Resource::TYPE::MODEL:
		// 複製したモデルを消してから、元のモデルを消す
		for (int duplicateId_ : duplicateModelIds_)
		{
			MV1DeleteModel(duplicateId_);
		}
		duplicateModelIds_.clear();
		MV1DeleteModel(handleId_);
		break;
	}

	// 読み込み直したときに、古いIDが残らないようにする
	handleId_ = INVALID_HANDLE;
}