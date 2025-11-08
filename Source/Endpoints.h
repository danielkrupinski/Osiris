#pragma once

#include "GlobalContext/GlobalContext.h"
#include "Hooks/PeepEventsHook.h"
#include "Utils/ReturnAddress.h"

int SDLHook_PeepEvents(void* events, int numevents, int action, unsigned minType, unsigned maxType) noexcept
{
    return GlobalContext::instance().peepEventsHook(events, numevents, action, minType, maxType);
}

void ViewRenderHook_onRenderStart(cs2::CViewRender* thisptr) noexcept
{
    const auto unloadFlag = GlobalContext::instance().onRenderStartHook(thisptr);
    if (unloadFlag)
        GlobalContext::destroyInstance();
}

LINUX_ONLY([[gnu::aligned(8)]]) std::uint64_t PlayerPawn_sceneObjectUpdater(cs2::C_CSPlayerPawn* playerPawn, void* unknown, bool unknownBool) noexcept
{
    return GlobalContext::instance().playerPawnSceneObjectUpdater(playerPawn, unknown, unknownBool);
}

LINUX_ONLY([[gnu::aligned(8)]]) std::uint64_t Weapon_sceneObjectUpdater(cs2::C_CSWeaponBase* weapon, void* unknown, bool unknownBool) noexcept
{
    return GlobalContext::instance().weaponSceneObjectUpdater(weapon, unknown, unknownBool);
}

float ClientModeHook_getViewmodelFov(cs2::ClientModeCSNormal* clientMode) noexcept
{
    return GlobalContext::instance().getViewmodelFovHook(clientMode);
}
