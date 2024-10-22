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

void ViewRenderHook_onRenderStart_cpp(cs2::CViewRender* thisptr) noexcept
{
    auto& fullContext = GlobalContext::instance().fullContext();

    fullContext.hooks.viewRenderHook.getOriginalOnRenderStart()(thisptr);

    HookDependencies dependencies{fullContext};
    SoundWatcher<decltype(dependencies)> soundWatcher{fullContext.featureHelpers.soundWatcherState, dependencies};
    soundWatcher.update();
    fullContext.features(dependencies).soundFeatures().runOnViewMatrixUpdate();

    auto&& playerInfoInWorld = dependencies.make<PlayerInfoInWorld>();
    RenderingHookEntityLoop{dependencies, playerInfoInWorld}.run();
    playerInfoInWorld.hideUnusedPanels();
    dependencies.make<GlowSceneObjects>().removeUnreferencedObjects();
}

}
