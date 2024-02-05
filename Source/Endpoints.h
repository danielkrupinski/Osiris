#pragma once

#include "GlobalContext/GlobalContext.h"
#include "Hooks/PeepEventsHook.h"
#include "Utils/ReturnAddress.h"

extern "C"
{

sdl3::SDL_PeepEvents* SDLHook_PeepEvents_cpp() noexcept
{
    const auto [original, shouldUnload] {GlobalContext::instance().peepEventsHook()};
    if (shouldUnload)
        GlobalContext::destroyInstance();
    return original;
}

cs2::CLoopModeGame::getWorldSession LoopModeGameHook_getWorldSession_cpp(const void* returnAddress) noexcept
{
    return GlobalContext::instance().fullContext().getWorldSessionHook(ReturnAddress{returnAddress});
}

void ViewRenderHook_onRenderStart_cpp(cs2::CViewRender* thisptr) noexcept
{
    GlobalContext::instance().fullContext().onRenderStart(thisptr);
}

}
