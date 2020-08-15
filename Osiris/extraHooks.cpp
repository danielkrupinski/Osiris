#include "SDK/ModelInfo.h"
#include "extraHooks.h"

typedef float quaternion[4];

extraHooks extraHook;

void __fastcall DoExtraBoneProcessing(Entity* player, uint32_t, StudioHdr* hdr, Vector* pos, quaternion* q, matrix3x4* matrix, void* bone_list, void* context)
{
	const auto entity = reinterpret_cast<Entity*> (player);;
	auto state = entity->getAnimstate();
	const auto val = reinterpret_cast<float*> (reinterpret_cast<uintptr_t> (state) + 292);
	const auto backup = *val;
	auto backup_onground = false;

	if (state)
	{
		backup_onground = state->OnGround;
		state->OnGround = false;

		if (entity->velocity().length2D() < 0.1f)
			*val = 0.f;
	}

	extraHook.player.vmt.getOriginal<void>(197, player, hdr, pos, q, matrix, bone_list, context);

	if (state)
	{
		*val = backup;
		state->OnGround = backup_onground;
	}
}

void __fastcall StandardBlendingRules(Entity* player, uint32_t edx, StudioHdr* hdr, Vector* pos, quaternion* q, float curTime, int boneMask)
{
	auto orig = extraHook.player.vmt.getOriginal<void, StudioHdr*, Vector*, quaternion*, float, int>(205, hdr, pos, q, curTime, boneMask);
	uint32_t* effects = (uint32_t*)((uintptr_t)player + 0xF0);
	*effects |= 8;
	orig(player, hdr, pos, q, curTime, boneMask);
	*effects &= ~8;
}

void extraHooks::hookEntity(Entity* ent)
{
	player.vmt.init(ent);
	player.vmt.hookAt(197, DoExtraBoneProcessing);
	player.vmt.hookAt(205, StandardBlendingRules);
	player.isHooked = true;
}

bool extraHooks::init()
{
	if (interfaces->engine->isInGame())
	{
		if (!localPlayer || !localPlayer.get())
			return false;
		static Entity* oldLocalPlayer = nullptr;
		if (!player.isHooked || oldLocalPlayer != localPlayer.get())
		{
			oldLocalPlayer = localPlayer.get();
			hookEntity(localPlayer.get());
		}
	}
	return true;
}

void extraHooks::restore()
{
	if (player.isHooked)
	{
		player.vmt.restore();
		player.isHooked = false;
	}
} 