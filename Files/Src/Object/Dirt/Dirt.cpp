#include "Dirt.h"

Dirt::Dirt(const VECTOR& pos)
{
	pos_ = pos;
	radius_ = DEFAULT_RADIUS;
}

Dirt::~Dirt(void)
{
}

void Dirt::Draw(void)
{
	// ç°ÇÕãÖëÃÇ≈ë„óp
	DrawSphere3D(pos_, radius_, SPHERE_DIV_NUM, DIFFUSE_COLOR, SPECULAR_COLOR, TRUE);
}

bool Dirt::IsHitLine(const VECTOR& start, const VECTOR& end) const
{
	return HitCheck_Line_Sphere(start, end, pos_, radius_) == TRUE;
}

float Dirt::GetDistance(const VECTOR& from) const
{
	return VSize(VSub(pos_, from));
}

const VECTOR& Dirt::GetPos(void) const
{
	return pos_;
}