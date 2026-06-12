#pragma once

#include <HookContext/HookContext.h>

template <typename HookContext>
void applySkyChangerOnViewRender(HookContext& hookContext) noexcept
{
    // This is called from ViewRenderHook_onRenderStart
    // Apply sky texture changes every frame if enabled
    if (hookContext.featuresStates().visualFeaturesStates.skyChangerState.enabled) {
        auto&& skyChanger = hookContext.template make<SkyChanger<HookContext>>();
        skyChanger.applySkyTexture();
    }
}