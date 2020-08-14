#pragma once
#include "../SDK/GlobalVars.h"
#include "../SDK/EngineTrace.h"

struct FireBulletData
{
	FireBulletData(const Vector& eyePos, Entity* entity) : src(eyePos), filter(entity)
	{
	}

	Vector          src;
	Trace           enter_trace;
	Vector          direction;
	TraceFilter     filter;
	float           trace_length;
	float           trace_length_remaining;
	float           current_damage;
	int             penetrate_count;
};


class AutoWall
{
public:
	float Damage(const Vector& point)noexcept;
	bool CanHitFloatingPoint(const Vector& point, const Vector& source)noexcept;
	bool CanWallbang(float& dmg)noexcept;
	bool PenetrateWall(Entity* pBaseEntity, Vector& vecPoint, int weaponIndex) noexcept;
};
extern AutoWall* Autowall;