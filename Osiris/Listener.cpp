#include "Listener.h"
#include <deque>
#include "SDK/ModelInfo.h"
#include "Hacks/Backtrack.h"

typedef float quaternion_t[4];

HookListener hookListener;

void __fastcall DoExtraBoneProcessing(Entity* player, uint32_t, StudioHdr* hdr, Vector* pos, quaternion_t* q, matrix3x4* matrix,
	void* bone_list, void* context)
{
	const auto e = reinterpret_cast<Entity*> (player);;
	auto state = e->getAnimstate2();
	const auto val = reinterpret_cast<float*> (reinterpret_cast<uintptr_t> (state) + 292);
	const auto backup = *val;
	auto backup_onground = false;

	if (state)
	{
		backup_onground = state->m_bOnGround;
		state->m_bOnGround = false;

		if (localPlayer->velocity().length2D() < 0.1f)
			*val = 0.f;
	}

	hookListener.m_players[e->index()].m_vmt.getOriginal<void>(197, player, hdr, pos, q, matrix, bone_list, context);

	if (state)
	{
		*val = backup;
		state->m_bOnGround = backup_onground;
	}
}

void __fastcall StandardBlendingRules(Entity* player, uintptr_t edx, StudioHdr* hdr, Vector* pos, quaternion_t* q, float curTime, int boneMask)
{
	auto orig = hookListener.m_players[player->index()].m_vmt.getOriginal<void, StudioHdr*, Vector*, quaternion_t*, float, int>(205, hdr, pos, q, curTime, boneMask);

	*reinterpret_cast<int*>(player + 0xF0) |= 8;
	orig(player, hdr, pos, q, curTime, boneMask);
	*reinterpret_cast<int*>(player + 0xF0) &= ~8;
}

void HookListener::hook_entity(Entity* ent)
{
	auto i = ent->index();
	m_players[i].m_vmt.init(ent);
	//hook Entity functions
	//m_players[i].m_vmt.hookAt(187, BuildTransformations); //doubt index b1g doubt so no hooking
	m_players[i].m_vmt.hookAt(197, DoExtraBoneProcessing); //correct index and working
	m_players[i].m_vmt.hookAt(205, StandardBlendingRules); //correct index and working
	//m_players[i].m_vmt.hookAt(206, AccumulateLayers); //correct index
		//m_players[i].m_vmt.hookAt(223, UpdateClientSideAnimation); //correct index
	m_players[i].m_hooked = true;
}

bool HookListener::init()
{
	if (interfaces->engine->isInGame())
	{
		for (auto i = 1; i <= interfaces->engine->getMaxClients(); i++)
		{
			auto entity = interfaces->entityList->getEntity(i);

			if (!entity)
				continue;

			hook_entity(entity);
		}
	}
	return true;
}

void HookListener::remove()
{
	for (auto i = 1; i <= 65; i++)
	{
		if (m_players[i].m_hooked)
		{
			m_players[i].m_vmt.restore();
			m_players[i].m_hooked = false;
		}
	}
}