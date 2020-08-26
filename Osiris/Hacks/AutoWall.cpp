#include <functional>
#include <intrin.h>
#include <string>

#include "Autowall.h"

#include "../SDK/Entity.h"
#include "../SDK/Engine.h"
#include "../SDK/Surface.h"
#include "../SDK/PhysicsSurfaceProps.h"
#include "../SDK/Utils.h"
#include "../SDK/EngineTrace.h"

#include "../Interfaces.h"
#include "../Config.h"

#include <algorithm>
#include "../SDK/Math.h"

//five
AutoWall* Autowall;

Vector aim;
void TraceLine(Vector& vecAbsStart, Vector& vecAbsEnd, unsigned int mask, Entity* ignore, Trace* ptr)noexcept
{
	TraceFilter filter(ignore);
	interfaces->engineTrace->traceRay(Ray(vecAbsStart, vecAbsEnd), mask, filter, *ptr);
}

bool VectortoVectorVisible(Vector src, Vector point)noexcept
{
	Trace Trace;
	TraceLine(src, point, mask_solid, localPlayer.get(), &Trace);

	if (Trace.fraction == 1.0f)
	{
		return true;
	}

	return false;
}

bool HandleBulletPenetration(WeaponInfo* wpn_data, FireBulletData& data, bool extracheck, Vector& wallbangPos) noexcept;

float GetHitgroupDamageMult(int iHitGroup)noexcept
{
	switch (iHitGroup)
	{
	case HitGroup::Head:
		return 4.f;
	case HitGroup::Stomach:
		return 1.25f;
	case HitGroup::LeftLeg:
	case HitGroup::RightLeg:
		return 0.75f;
	}

	return 1.0f;
}

bool IsArmored(Entity* enemy, int ArmorValue, int Hitgroup)
{
	bool result = false;
 
	if (ArmorValue > 0)
	{
		switch (Hitgroup)
		{
		case HITGROUP_GENERIC:
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			result = true;
			break;
		case HITGROUP_HEAD:
			result = enemy->hasHelmet();
			break;
		}
	}
 
	return result;
}

void ScaleDamage(int hitgroup, Entity* enemy, float weapon_armor_ratio, float& current_damage) noexcept
{
	current_damage *= GetHitgroupDamageMult(hitgroup);
	int ArmorValue = enemy->armor();
	
	if (IsArmored(enemy, ArmorValue, hitgroup))
	{
		float Damage = current_damage;
		float v47 = 1.f, ArmorBonusRatio = 0.5f, ArmorRatio = weapon_armor_ratio * 0.5f;
		auto NewDamage = Damage * ArmorRatio;

		if (((Damage - (Damage * ArmorRatio)) * (v47 * ArmorBonusRatio)) > ArmorValue)
		{
			NewDamage = Damage - (ArmorValue / ArmorBonusRatio);	
		}

		current_damage = Damage;
	}
}

DWORD FindPattern(const char* module_name, const BYTE* mask, const char* mask_string)noexcept
{
	/// Get module address
	const unsigned int module_address = reinterpret_cast<unsigned int>(GetModuleHandle(module_name));

	/// Get module information to the size
	MODULEINFO module_info;
	GetModuleInformation(GetCurrentProcess(), reinterpret_cast<HMODULE>(module_address), &module_info, sizeof(MODULEINFO));

	auto IsCorrectMask = [](const unsigned char* data, const unsigned char* mask, const char* mask_string) -> bool
	{
		for (; *mask_string; ++mask_string, ++mask, ++data)
			if (*mask_string == 'x' && *mask != *data)
				return false;

		return (*mask_string) == 0;
	};

	/// Iterate until we find a matching mask
	for (unsigned int c = 0; c < module_info.SizeOfImage; c += 1)
	{
		/// does it match?
		if (IsCorrectMask(reinterpret_cast<unsigned char*>(module_address + c), mask, mask_string))
			return (module_address + c);
	}

	return 0;
}

void UTIL_ClipTraceToPlayers(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, TraceFilter* filter, Trace* tr)noexcept
{
	static DWORD dwAddress = FindPattern("client.dll", (BYTE*)"\x53\x8B\xDC\x83\xEC\x08\x83\xE4\xF0\x83\xC4\x04\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x81\xEC\x00\x00\x00\x00\x8B\x43\x10", "xxxxxxxxxxxxxxxxxxxxxxxx????xxx");

	if (!dwAddress)
		return;

	_asm
	{
		MOV		EAX, filter
		LEA		ECX, tr
		PUSH	ECX
		PUSH	EAX
		PUSH	mask
		LEA		EDX, vecAbsEnd
		LEA		ECX, vecAbsStart
		CALL	dwAddress
		ADD		ESP, 0xC
	}
}

bool SimulateFireBullet(Entity* local, Entity* weapon, FireBulletData& data, Vector& wallbangVector)noexcept
{
	if (!localPlayer  || !local || !weapon)
	{
		return false;
	}
	
	data.penetrate_count = 4; // Max Amount Of Penetration
	data.trace_length = 0.0f; 
	auto wpn_data = weapon->getWeaponData(); // Get Weapon Info

	if (!wpn_data)
	{
		return false;	
	}
	
	data.current_damage = static_cast<float>(wpn_data->damage);
	while ((data.penetrate_count > 0) && (data.current_damage >= 1.0f))
	{
		data.trace_length_remaining = wpn_data->range - data.trace_length;
		Vector End_Point = data.src + data.direction * data.trace_length_remaining;
		TraceLine(data.src, End_Point, 0x4600400B, local, &data.enter_trace);
		UTIL_ClipTraceToPlayers(data.src, End_Point * 40.f, 0x4600400B, &data.filter, &data.enter_trace);
		if (data.enter_trace.fraction == 1.0f)
			break;
		if ((data.enter_trace.hitgroup <= 7) && (data.enter_trace.hitgroup > 0) && (local->team() != data.enter_trace.entity->team()))
		{
			data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
			data.current_damage *= pow(wpn_data->rangeModifier, data.trace_length * 0.002f);
			ScaleDamage(data.enter_trace.hitgroup, data.enter_trace.entity, wpn_data->armorRatio, data.current_damage);
			return true;
		}
		if (!HandleBulletPenetration(wpn_data, data, false, wallbangVector))
			break;
	}
	return false;
}

bool TraceToExitalt(Vector& end, Trace& tr, Vector start, Vector vEnd, Trace* trace)noexcept
{
	typedef bool(__fastcall* TraceToExitFn)(Vector&, Trace&, float, float, float, float, float, float, Trace*);
	static TraceToExitFn TraceToExit = (TraceToExitFn)FindSignature("client.dll", "55 8B EC 83 EC 30 F3 0F 10 75");
	if (!TraceToExit)
		return false;

	//return TraceToExit(end, tr, start.x, start.y, start.z, vEnd.x, vEnd.y, vEnd.z, trace);
	__asm
	{
		push trace
		push vEnd.z
		push vEnd.y
		push vEnd.x
		push start.z
		push start.y
		push start.x
		mov edx, tr
		mov ecx, end
		call TraceToExit
		add esp, 0x1C
	}
}

bool HandleBulletPenetration(WeaponInfo* wpn_data, FireBulletData& data, bool extracheck, Vector& wallbangPos)noexcept
{
	SurfaceData* enter_surface_data = interfaces->physicsSurfaceProps->getSurfaceData(data.enter_trace.surface.surfaceProps);
	int enter_material = enter_surface_data->material;
	float enter_surf_penetration_mod = enter_surface_data->penetrationmodifier;
	data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
	data.current_damage *= pow(wpn_data->rangeModifier, (data.trace_length * 0.002f));
	if ((data.trace_length > 3000.f) || (enter_surf_penetration_mod < 0.1f))
		data.penetrate_count = 0;
	if (data.penetrate_count <= 0)
		return false;
	static Vector dummy;
	static Trace trace_exit;
	if (!TraceToExitalt(dummy, data.enter_trace, data.enter_trace.endpos, data.direction, &trace_exit))
		return false;
	SurfaceData* exit_surface_data = interfaces->physicsSurfaceProps->getSurfaceData(trace_exit.surface.surfaceProps);
	int exit_material = exit_surface_data->material;
	float exit_surf_penetration_mod = exit_surface_data->penetrationmodifier;
	float final_damage_modifier = 0.16f;
	float combined_penetration_modifier = ( enter_surface_data->penetrationmodifier + exit_surface_data->penetrationmodifier ) * 0.5f;

	bool a5 = data.enter_trace.contents >> 3 & CONTENTS_SOLID;
	bool v19 = data.enter_trace.surface.flags >> 7 & SURF_LIGHT;
	
	if (((data.enter_trace.contents & contents_grate) != 0) || (enter_material == CHAR_TEX_GLASS) || (enter_material == CHAR_TEX_GRATE))
	{
		combined_penetration_modifier = 3.0f;
		final_damage_modifier = 0.05f;
	}
	else if( a5 || v19 ) 
	{
		final_damage_modifier = 0.16f;
		combined_penetration_modifier = 1.0f;
	}


	if (enter_material == exit_material)
	{
		if (exit_material == CHAR_TEX_WOOD || exit_material == CHAR_TEX_CARDBOARD)
		{
			combined_penetration_modifier = 3.0f;
		}
		else if (exit_material == CHAR_TEX_PLASTIC) 
		{
			combined_penetration_modifier = 2.0f;
		}
	}
	float v34 = fmaxf(0.f, 1.0f / combined_penetration_modifier);
	float v35 = (data.current_damage * final_damage_modifier) + v34 * 3.0f * fmaxf(0.0f, (3.0f / wpn_data->penetration) * 1.25f);

		
	Vector tem{ trace_exit.endpos - data.enter_trace.endpos };
	float thickness = tem.length();
	if (extracheck)
		if (!VectortoVectorVisible(trace_exit.endpos, aim))
			return false;
	thickness *= thickness;
	thickness *= v34;
	thickness /= 24.0f;
	float lost_damage = fmaxf(0.0f, v35 + thickness); // in karma they substract 1.6f here for some reason
	if (lost_damage > data.current_damage)
		return false;
	if (lost_damage >= 0.0f)
		data.current_damage -= lost_damage;
	if (data.current_damage < 1.0f)
		return false;

	// only updated on first wallbang
	if (data.penetrate_count >= 4)
	{
		wallbangPos.x = trace_exit.endpos.x;
		wallbangPos.y = trace_exit.endpos.y;
		wallbangPos.z = trace_exit.endpos.z;	
	}
	
	data.src = trace_exit.endpos;
	data.penetrate_count--;

	return true;
}


float AutoWall::Damage(const Vector& point, Vector& wallbangVector)noexcept
{
	auto data = FireBulletData(localPlayer->getEyePosition(), localPlayer.get());

	Vector angles;
	angles = Math::CalcAngle(data.src, point);
	Math::AngleVectors(angles, &data.direction);
	Math::VectorNormalize(data.direction);

	if (SimulateFireBullet(localPlayer.get(), localPlayer->getActiveWeapon(), data, wallbangVector))
		return data.current_damage;

	return 0.f;
}

bool AutoWall::CanHitFloatingPoint(const Vector& point, const Vector& source, Vector& wallbangPos)noexcept
{
	aim = point;

	if (!localPlayer.get())
		return false;

	FireBulletData data = FireBulletData(source, localPlayer.get());

	Vector angles = Math::CalcAngle(data.src, point);
	Math::AngleVectors(angles, &data.direction);
	Math::VectorNormalize(data.direction);

	Entity* pWeapon = (Entity*)localPlayer->getActiveWeapon();

	if (!pWeapon)
		return false;

	data.penetrate_count = 1;
	data.trace_length = 0.0f;

	WeaponInfo* weaponData = pWeapon->getWeaponData();

	if (!weaponData)
		return false;

	data.current_damage = (float)weaponData->damage;

	data.trace_length_remaining = weaponData->range - data.trace_length;

	Vector end = data.src + data.direction * data.trace_length_remaining;

	//void TraceLine(Vector& vecAbsStart, Vector& vecAbsEnd, unsigned int mask, Entity* ignore, Trace* ptr)
	TraceLine(data.src, end, mask_shot | contents_hitbox, localPlayer.get(), &data.enter_trace);

	if (VectortoVectorVisible(data.src, point))
	{
		return true;
	}
	static bool extra_check = true;
	if (HandleBulletPenetration(weaponData, data, extra_check, wallbangPos))
	{
		return true;
	}

	return false;
}
void CalcAngle34(Vector src, Vector dst, Vector& angles)noexcept
{
	Vector delta = src - dst;
	auto hyp = delta.length2D(); //delta.Length
	angles.y = (atan(delta.y / delta.x) * 57.29577f);
	angles.x = (atan(delta.z / hyp) * 57.29577f);
	angles.z = 0.00;

	if (delta.x >= 0.0)
		angles.y += 180.0f;
}

bool AutoWall::CanWallbang(float& dmg, Vector& wallbangPos)noexcept
{

	if (!localPlayer)
		return false;

	if (!localPlayer->isAlive())
		return false;

	FireBulletData data = FireBulletData(localPlayer->getEyePosition(), localPlayer.get());
	//	data.filter = trace;
	data.filter.skip = localPlayer.get();

	Vector EyeAng;
	interfaces->engine->getViewAngles(EyeAng);

	Vector dst, forward;

	Math::AngleVectors(EyeAng, &forward);
	dst = data.src + (forward * 8196.f);

	Vector angles;
	CalcAngle34(data.src, dst, angles);
	Math::AngleVectors(angles, &data.direction);
	Math::VectorNormalize(data.direction);

	auto weapon = localPlayer->getActiveWeapon();

	if (!weapon)
		return false;

	data.penetrate_count = 1;
	data.trace_length = 0.0f;

	auto weaponData = weapon->getWeaponData();

	if (!weaponData)
		return false;

	data.current_damage = (float)weaponData->damage;

	data.trace_length_remaining = weaponData->range - data.trace_length;

	Vector end = data.src + data.direction * data.trace_length_remaining;

	TraceLine(data.src, end, mask_shot | contents_grate, localPlayer.get(), &data.enter_trace);

	if (data.enter_trace.fraction == 1.0f)
		return false;

	if (HandleBulletPenetration(weaponData, data, false, wallbangPos))
	{
		dmg = data.current_damage;
		return true;
	}

	return false;
}

bool AutoWall::PenetrateWall(Entity* pBaseEntity, Vector& vecPoint, int weaponIndex, Vector& wallbangVector)noexcept
{
	float min_damage = 5.0f; //mindamage
	if (pBaseEntity->health() < min_damage)
		min_damage = pBaseEntity->health();

	if (Damage(vecPoint, wallbangVector) >= min_damage)
		return true;

	return false;
}