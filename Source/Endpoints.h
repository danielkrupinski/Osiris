#pragma once

#include "GlobalContext/GlobalContext.h"
#include "Hooks/PeepEventsHook.h"
#include "Utils/ReturnAddress.h"

extern "C"
{

sdl3::SDL_PeepEvents* SDLHook_PeepEvents_cpp() noexcept
{
    return GlobalContext::instance().peepEventsHook().original;
}

void ViewRenderHook_onRenderStart_cpp(cs2::CViewRender* thisptr) noexcept
{
    const auto unloadFlag = GlobalContext::instance().onRenderStartHook(thisptr);
    if (unloadFlag)
        GlobalContext::destroyInstance();
}

std::uint64_t PlayerPawn_sceneObjectUpdater_cpp(cs2::C_CSPlayerPawn* playerPawn, void* unknown, bool unknownBool) noexcept
{
    return GlobalContext::instance().playerPawnSceneObjectUpdater(playerPawn, unknown, unknownBool);
}

std::uint64_t Weapon_sceneObjectUpdater_cpp(cs2::C_CSWeaponBase* weapon, void* unknown, bool unknownBool) noexcept
{
    return GlobalContext::instance().weaponSceneObjectUpdater(weapon, unknown, unknownBool);
}

}
