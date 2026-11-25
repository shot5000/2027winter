#include <cmath>
#include <fstream>
#include "../../External/json/json.hpp"
#include "EnemyRoute.h"

EnemyRoute::EnemyRoute(void)
{
	index_ = 0;
}

EnemyRoute::~EnemyRoute(void)
{
}

std::vector<VECTOR> EnemyRoute::Load(void)
{
	std::vector<VECTOR> points_;

	std::ifstream file_(ROUTE_FILE);
	if (!file_.is_open())
	{
		ErrorLogAdd("道のJSONを開けませんでした：");
		ErrorLogAdd(ROUTE_FILE);
		ErrorLogAdd("\n");
		return points_;
	}

	nlohmann::json json_ = nlohmann::json::parse(file_, nullptr, false);
	if (json_.is_discarded() || !json_.contains("route"))
	{
		ErrorLogAdd("道のJSONの書き方が正しくありません(routeがありません)。\n");
		return points_;
	}

	// 地点を順番に読む(数字が足りない地点は飛ばす)
	for (const auto& posJson_ : json_["route"])
	{
		if (!posJson_.is_array() || posJson_.size() != POS_ELEMENT_NUM)
		{
			continue;
		}
		points_.push_back(VGet(
			posJson_[0].get<float>(),
			posJson_[1].get<float>(),
			posJson_[2].get<float>()));
	}
	return points_;
}

void EnemyRoute::Init(const std::vector<VECTOR>& points, const VECTOR& pos)
{
	points_ = points;
	index_ = 0;
	SelectNearest(pos);
}

bool EnemyRoute::IsEmpty(void) const
{
	return points_.empty();
}

const VECTOR& EnemyRoute::GetTarget(void) const
{
	return points_[index_];
}

void EnemyRoute::Update(const VECTOR& pos)
{
	if (IsEmpty())
	{
		return;
	}

	// 着いたら、次の地点へ(最後まで行ったら最初に戻る)
	if (HorizontalDist(pos, points_[index_]) <= ARRIVE_DIST)
	{
		index_ = (index_ + 1) % static_cast<int>(points_.size());
	}
}

void EnemyRoute::SelectNearest(const VECTOR& pos)
{
	if (IsEmpty())
	{
		return;
	}

	float nearDist_ = HorizontalDist(pos, points_[0]);
	index_ = 0;
	for (size_t i_ = 1; i_ < points_.size(); i_++)
	{
		float dist_ = HorizontalDist(pos, points_[i_]);
		if (dist_ < nearDist_)
		{
			nearDist_ = dist_;
			index_ = static_cast<int>(i_);
		}
	}
}

void EnemyRoute::DrawDebug(void) const
{
	if (IsEmpty())
	{
		return;
	}

	// ライトを切って、色がそのまま出るようにする
	SetUseLighting(FALSE);

	int pointNum_ = static_cast<int>(points_.size());
	for (int i_ = 0; i_ < pointNum_; i_++)
	{
		// 次の地点とのあいだに線を引く(最後は最初へ)
		VECTOR from_ = points_[i_];
		VECTOR to_ = points_[(i_ + 1) % pointNum_];
		from_.y += DEBUG_OFFSET_Y;
		to_.y += DEBUG_OFFSET_Y;
		DrawLine3D(from_, to_, DEBUG_COLOR);

		// 地点の球(向かっている地点は、色を変える)
		unsigned int color_ = (i_ == index_) ? DEBUG_TARGET_COLOR : DEBUG_COLOR;
		DrawSphere3D(from_, DEBUG_RADIUS, DEBUG_DIV_NUM, color_, color_, TRUE);
	}

	SetUseLighting(TRUE);
}

float EnemyRoute::HorizontalDist(const VECTOR& a, const VECTOR& b)
{
	float dx_ = a.x - b.x;
	float dz_ = a.z - b.z;
	return sqrtf(dx_ * dx_ + dz_ * dz_);
}