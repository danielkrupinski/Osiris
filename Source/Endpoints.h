#pragma once

#include "GlobalContext.h"
#include "Hooks/PeepEventsHook.h"
#include "Platform/Macros/CallStack.h"

[[nodiscard]] inline auto& features() noexcept
{
    return *GlobalContext::instance().features;
}

[[nodiscard]] inline auto& hudFeatures() noexcept
{
    return features().hudFeatures;
}

[[nodiscard]] inline auto& visuals() noexcept
{
    return features().visuals;
}

inline int PeepEventsHook::SDL_PeepEvents(void* events, int numevents,
    int action,
    unsigned minType, unsigned maxType) noexcept
{
    GlobalContext::instance().initializeFromGameThread();

    hudFeatures().bombTimer.run(GlobalContext::instance().featureHelpers->getBombTimerHelpers());
    hudFeatures().defusingAlert.run(GlobalContext::instance().featureHelpers->getDefusingAlertHelpers());
    hudFeatures().killfeedPreserver.run(GlobalContext::instance().featureHelpers->getKillfeedPreserverHelpers());

    GlobalContext::instance().panoramaGUI->run();
    GlobalContext::instance().hooks->loopModeGameHook.update();
    GlobalContext::instance().hooks->viewRenderHook.update();
    visuals().scopeOverlayRemover.updatePanelVisibility(GlobalContext::instance().featureHelpers->hudProvider);

    if (GlobalContext::instance().unloadFlag) {
        const auto originalPeepEvents = GlobalContext::instance().peepEventsHook.original;
        GlobalContext::instance().peepEventsHook.disable();
        GlobalContext::instance().hooks->loopModeGameHook.forceUninstall();
        GlobalContext::instance().hooks->viewRenderHook.forceUninstall();
        GlobalContext::destroyInstance();
        return originalPeepEvents(events, numevents, action, minType, maxType);
    }
    return GlobalContext::instance().peepEventsHook.original(events, numevents, action, minType, maxType);
}

inline void* LoopModeGameHook::getWorldSession(cs2::CLoopModeGame* thisptr) noexcept
{
    visuals().scopeOverlayRemover.getWorldSessionHook(RETURN_ADDRESS());
    return GlobalContext::instance().hooks->loopModeGameHook.originalGetWorldSession(thisptr);
}

inline void ViewRenderHook::onRenderStart(cs2::CViewRender* thisptr) noexcept
{
    GlobalContext::instance().onRenderStart(thisptr);
}
