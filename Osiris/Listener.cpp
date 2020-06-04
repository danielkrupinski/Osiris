#include "Listener.h"
#include <deque>
#include "SDK/ModelInfo.h"
#include "Hacks/Backtrack.h"

typedef float quaternion_t[4];

HookListener hookListener;

/*
void BuildTransformations(Entity* player, uintptr_t /*edx*/, StudioHdr* hdr, Vector* pos, quaternion_t* q, const matrix3x4& cameraTransform, int boneMask, byte* boneComputed) {
	auto orig = hookListener.m_players[player->index()].m_vmt.getOriginal<void, StudioHdr*, Vector*, quaternion_t*, const matrix3x4&, int, byte*>(187, hdr, pos, q, cameraTransform, boneMask, boneComputed);
	//auto backup = hdr->flags;
	//for (int i = 0; i < hdr->numBones;i++)
	//{
	//	hdr->flags = 0x04;
	//}
	orig(player, hdr, pos, q, cameraTransform, boneMask, boneComputed);
	//hdr->flags = backup;
}
*/
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

	hookListener.m_players[e->index()].m_vmt.getOriginal<void>(197,player, hdr, pos, q, matrix, bone_list, context);

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



/*
void __fastcall AccumulateLayers(Entity* player, uintptr_t /*edx*/, void** bone_setup, Vector* pos, float time, quaternion_t* q)
{
	std::array<size_t, 13> layers;
	layers.fill(layers.size());
	std::array<AnimationLayer, 13> anims;
	std::memcpy(&anims, player->get_anim_overlays(), sizeof(AnimationLayer) * localPlayer.get()->getAnimationLayerCount());

	for (auto i = 0u; i < anims.size(); i++)
	{
		const auto& layer = (*player->get_anim_overlay(i));

		if (layer.weight > 0 && layer.order >= 0 && layer.order < layers.size())
			layers[layer.order] = i;
	}

	for (auto i = 0u; i < layers.size(); i++)
	{
		const auto layer_num = layers[i];

		if (layer_num < 0 || layer_num >= anims.size())
			continue;

		const auto& layer = (*player->get_anim_overlay(layer_num));

		if (layer.weight <= 1)
			continue;

		//(*setup)->accumulate_pose(pos, q, layer.sequence, layer.cycle, layer.weight, memory->globalVars->currenttime, player->get_ik_context());
		//piece of shit, no answers on uc about get_ik_context and accumulate_pose i dont fucking have ida pro, WHAT IAM SUPPOSED TO DO????????
	}
}
*/

/*
void __fastcall UpdateClientSideAnimation(Entity* player_, uintptr_t /*edx*/)
{
	auto e = reinterpret_cast<Entity*>(player_);
	
	if (e->index() == localPlayer->index())
	{
		auto state = reinterpret_cast<CCSGOPlayerAnimState*> (e->getAnimstate2());

		if (state)
		{
			state->m_fDuckAmount = e->duckAmmount();
			e->setAbsAngle(Vector{ 0.f, state->m_flGoalFeetYaw, 0.f });
		}

		hookListener.m_players[e->index()].m_vmt.getOriginal<void>(223, player_);

		if (state)
			e->setAbsAngle(Vector{ 0.f, state->m_flGoalFeetYaw, 0.f });
	}
	
	if (e->isEnemy())
	{
		const auto old_cur_time = memory->globalVars->currenttime;
		const auto old_real_time = memory->globalVars->realtime;
		const auto old_frame_time = memory->globalVars->frametime;
		const auto old_abs_frame_time = memory->globalVars->absoluteFrameTime;
		const auto old_interp_amount = memory->globalVars->interpolationAmount;
		const auto old_tick_count = memory->globalVars->tickCount;
		memory->globalVars->currenttime = e->simulationTime();
		memory->globalVars->realtime = e->simulationTime();
		memory->globalVars->interpolationAmount = 0;
		memory->globalVars->tickCount = Backtrack::timeToTicks(e->simulationTime());
		memory->setAbsOrigin(e,e->origin());


		hookListener.m_players[e->index()].m_vmt.getOriginal<void>(223, player_);


		memory->globalVars->currenttime = old_cur_time;
		memory->globalVars->realtime = old_real_time;
		memory->globalVars->frametime = old_frame_time;
		memory->globalVars->absoluteFrameTime = old_abs_frame_time;
		memory->globalVars->tickCount = old_tick_count;
		memory->globalVars->interpolationAmount = old_interp_amount;
	}
	else
		hookListener.m_players[e->index()].m_vmt.getOriginal<void>(223, player_);
}
*/
void HookListener::hook_entity(Entity* ent)
{
	auto i = ent->index();
	m_players[i].m_vmt.init(ent);
	//hook
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
