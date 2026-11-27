#include <algorithm>
#include <cmath>
#include "NoiseManager.h"

std::vector<NoiseManager::NOISE> NoiseManager::noises_;
int NoiseManager::nextId_ = NoiseManager::NO_ID + 1;

void NoiseManager::Emit(const VECTOR& pos, float radius)
{
	// 音量が0のものは、音を出さない
	if (radius <= 0.0f)
	{
		return;
	}

	NOISE noise_ = { pos, radius, nextId_, 0.0f };
	nextId_++;
	noises_.push_back(noise_);
}

void NoiseManager::Update(float deltaTime)
{
	for (auto& noise_ : noises_)
	{
		noise_.age_ += deltaTime;
	}

	// 覚えておく時間を過ぎた音を消す
	noises_.erase(
		std::remove_if(
			noises_.begin(), noises_.end(),
			[](const NOISE& noise)
			{
				return noise.age_ > LIFE_TIME;
			}),
		noises_.end());
}

void NoiseManager::Reset(void)
{
	noises_.clear();
}

const std::vector<NoiseManager::NOISE>& NoiseManager::GetNoises(void)
{
	return noises_;
}

void NoiseManager::DrawDebug(void)
{
	float step_ = CIRCLE_ANGLE / DRAW_DIV_NUM;

	for (const NOISE& noise_ : noises_)
	{
		VECTOR center_ = noise_.pos_;
		center_.y += DRAW_OFFSET_Y;

		// 輪を、細かい線に分けて描く
		for (int i_ = 0; i_ < DRAW_DIV_NUM; i_++)
		{
			VECTOR p0_ = VAdd(center_, VGet(
				sinf(step_ * i_) * noise_.radius_, 0.0f,
				cosf(step_ * i_) * noise_.radius_));
			VECTOR p1_ = VAdd(center_, VGet(
				sinf(step_ * (i_ + 1)) * noise_.radius_, 0.0f,
				cosf(step_ * (i_ + 1)) * noise_.radius_));
			DrawLine3D(p0_, p1_, DRAW_COLOR);
		}
	}
}