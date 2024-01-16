#pragma once

#include <Platform/Macros/IsPlatform.h>

#include "GlobalContext/GlobalContext.h"
#include "Hooks/PeepEventsHook.h"
#include "Platform/Macros/CallStack.h"

extern "C"
{

#if IS_LINUX()
// TODO: remove when we have asm implementation on Linux

int SDLHook_PeepEvents_asm(void* events, int numevents,
    int action,
    unsigned minType, unsigned maxType) noexcept
{
    const auto [original, shouldUnload]{GlobalContext::instance().peepEventsHook()};
    if (shouldUnload)
        GlobalContext::destroyInstance();
    return original(events, numevents, action, minType, maxType);
}

void* LoopModeGameHook_getWorldSession_asm(cs2::CLoopModeGame* thisptr) noexcept
{
    return GlobalContext::instance().fullContext().getWorldSessionHook(RETURN_ADDRESS())(thisptr);
}

void ViewRenderHook_onRenderStart_asm(cs2::CViewRender* thisptr) noexcept
{
    GlobalContext::instance().fullContext().onRenderStart(thisptr);
}

#else

cs2::SDL_PeepEvents SDLHook_PeepEvents_cpp() noexcept
{
    const auto [original, shouldUnload] {GlobalContext::instance().peepEventsHook()};
    if (shouldUnload)
        GlobalContext::destroyInstance();
    return original;
}

cs2::CLoopModeGame::getWorldSession LoopModeGameHook_getWorldSession_cpp(cs2::CLoopModeGame* /* thisptr */, const void* returnAddress) noexcept
{
    return GlobalContext::instance().fullContext().getWorldSessionHook(ReturnAddress{returnAddress});
}

void ViewRenderHook_onRenderStart_cpp(cs2::CViewRender* thisptr) noexcept
{
    GlobalContext::instance().fullContext().onRenderStart(thisptr);
}

#endif

}
