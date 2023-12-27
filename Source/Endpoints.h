#pragma once

#include "GlobalContext/GlobalContext.h"
#include "Hooks/PeepEventsHook.h"
#include "Platform/Macros/CallStack.h"

inline int PeepEventsHook::SDL_PeepEvents(void* events, int numevents,
    int action,
    unsigned minType, unsigned maxType) noexcept
{
    const auto [original, shouldUnload]{GlobalContext::instance().peepEventsHook()};
    if (shouldUnload)
        GlobalContext::destroyInstance();
    return original(events, numevents, action, minType, maxType);
}

inline void* LoopModeGameHook::getWorldSession(cs2::CLoopModeGame* thisptr) noexcept
{
    return GlobalContext::instance().fullContext().getWorldSessionHook(RETURN_ADDRESS())(thisptr);
}

inline void ViewRenderHook::onRenderStart(cs2::CViewRender* thisptr) noexcept
{
    GlobalContext::instance().fullContext().onRenderStart(thisptr);
}
