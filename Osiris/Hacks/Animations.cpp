#include "Animations.h"

#include "../Memory.h"
#include "../Interfaces.h"

#include "../SDK/Engine.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/LocalPlayer.h"
#include "../SDK/ModelInfo.h"

std::array<PlayerHook, 65> playerHooks;
std::array<AnimData, 65> animData;

static Vector* __FASTCALL getEyeAngles(Entity* entity, uint32_t edx) noexcept
{
	static auto oGetEyeAngles = playerHooks[entity->index()].vmt.getOriginal<Vector*, 170>();

	if (entity->index() != localPlayer->index() || RETURN_ADDRESS() != memory->setEyePositionAndVectors)
		return oGetEyeAngles(entity);

	return &animData[entity->index()].sentViewAngles;
}

static void __FASTCALL buildTransformations(Entity* entity, uint32_t edx, StudioHdr* hdr, Vector* pos, void* q, matrix3x4* cameraTransform, int boneMask, void* boneComputed) noexcept
{
	static auto oBuildTransformations = playerHooks[entity->index()].vmt.getOriginal<void, 190, StudioHdr*, Vector*, void*, matrix3x4*, int, void*>(hdr, pos, q, cameraTransform, boneMask, boneComputed);

	std::array<int, 256> backupFlags;
	for (int i = 0; i < 256; i++) {
		if (auto bone = hdr->getBone(i); bone) {
			backupFlags[i] = bone->flags;
			bone->flags &= ~0x04;
		}
	}

	oBuildTransformations(entity, hdr, pos, q, cameraTransform, boneMask, boneComputed);

	for (int i = 0; i < 256; i++) {
		if (auto bone = hdr->getBone(i); bone) {
			bone->flags = backupFlags[i];
		}
	}
}

static void __FASTCALL doExtraBoneProcessing(Entity* entity, uint32_t edx, StudioHdr* hdr, Vector* pos, void* q, matrix3x4* matrix, void* boneComputed, void* context) noexcept
{
	static auto oDoExtraBoneProcessing = playerHooks[entity->index()].vmt.getOriginal<void, 198, StudioHdr*, Vector*, void*, matrix3x4*, void*, void*>(hdr, pos, q, matrix, boneComputed, context);
	const auto backupOnGround = entity->getAnimstate()->onGround;

	entity->getAnimstate()->onGround = false;
	oDoExtraBoneProcessing(entity, hdr, pos, q, matrix, boneComputed, context);
	entity->getAnimstate()->onGround = backupOnGround;
}

static void __FASTCALL standardBlendingRules(Entity* entity, uint32_t edx, StudioHdr* hdr, Vector* pos, void* q, float currentTime, int boneMask) noexcept
{
	static auto oStandardBlendingRules = playerHooks[entity->index()].vmt.getOriginal<void, 206, StudioHdr*, Vector*, void*, float, int>(hdr, pos, q, currentTime, boneMask);

	entity->getEffects() |= 0x8;
	oStandardBlendingRules(entity, hdr, pos, q, currentTime, boneMask);
	entity->getEffects() &= ~0x8;
}

static void __FASTCALL updateClientSideAnimation(Entity* entity, uint32_t edx) noexcept
{
	static auto oUpdateClientSideAnimation = playerHooks[entity->index()].vmt.getOriginal<void, 224>();

	if (animData[entity->index()].canUpdateAnimations || entity->index() != localPlayer->index()) {
		return oUpdateClientSideAnimation(entity);
	}
}

void Animations::hookPlayers() noexcept
{
	if (!localPlayer || !localPlayer->isAlive())
		return;

	if (!playerHooks[localPlayer->index()].isHooked || playerHooks[localPlayer->index()].entity != localPlayer.get()) {
		playerHooks[localPlayer->index()].entity = localPlayer.get();

		playerHooks[localPlayer->index()].vmt.init(localPlayer.get());
		playerHooks[localPlayer->index()].vmt.hookAt(170, getEyeAngles);
		playerHooks[localPlayer->index()].vmt.hookAt(190, buildTransformations);
		playerHooks[localPlayer->index()].vmt.hookAt(198, doExtraBoneProcessing);
		playerHooks[localPlayer->index()].vmt.hookAt(206, standardBlendingRules);
		playerHooks[localPlayer->index()].vmt.hookAt(224, updateClientSideAnimation);

		playerHooks[localPlayer->index()].isHooked = true;
	}
}

void Animations::restorePlayers() noexcept
{
	if (localPlayer) {
		playerHooks[localPlayer->index()].reset();
	}
}

void Animations::updateData(UserCmd* cmd, bool& sendPacket) noexcept
{
	if (!localPlayer || !localPlayer->isAlive())
		return;

	AnimData* data = &animData[localPlayer->index()];

	data->sendPacket = sendPacket;
	data->viewAngles = cmd->viewangles;
	if (sendPacket)
		data->sentViewAngles = cmd->viewangles;
}

void Animations::updateLocal() noexcept
{
	if (!localPlayer || !localPlayer->isAlive())
		return;

	AnimData* data = &animData[localPlayer->index()];

	if (!data->once) {
		memcpy(&data->layers, localPlayer->getAnimOverlays(), sizeof(AnimLayer) * 15);
		memcpy(&data->poses, &localPlayer->getPoseParameters(), sizeof(float) * 24);
		data->once = true;
	}

	if (localPlayer->getAnimstate()->lastClientSideAnimationUpdateFramecount == memory->globalVars->framecount)
		localPlayer->getAnimstate()->lastClientSideAnimationUpdateFramecount -= 1;

	if (localPlayer->getAnimstate()->lastClientSideAnimationUpdateTime == memory->globalVars->currenttime)
		localPlayer->getAnimstate()->lastClientSideAnimationUpdateTime += 1 * memory->globalVars->intervalPerTick;

	if (memory->globalVars->newTick()) {
		memcpy(&data->layers, localPlayer->getAnimOverlays(), sizeof(AnimLayer) * 15);

		data->layers[3].weight = 0.f;
		data->layers[3].cycle = 0.f;
		data->layers[12].weight = 0.f;

		data->canUpdateAnimations = localPlayer->clientSideAnimation() = true;
		memory->updateState(localPlayer->getAnimstate(), data->viewAngles);
		data->canUpdateAnimations = localPlayer->clientSideAnimation() = false;

		if (data->sendPacket) {
			memcpy(&data->poses, &localPlayer->getPoseParameters(), sizeof(float) * 24);
			data->absAngle = Vector{ 0.f, localPlayer->getAnimstate()->goalFeetYaw, 0.f };
		}
	}

	localPlayer->getAnimstate()->feetYawRate = 0.f;
	memory->setAbsAngle(localPlayer.get(), data->absAngle);

	memcpy(localPlayer->getAnimOverlays(), &data->layers, sizeof(AnimLayer) * 15);
	memcpy(&localPlayer->getPoseParameters(), &data->poses, sizeof(float) * 24);

}