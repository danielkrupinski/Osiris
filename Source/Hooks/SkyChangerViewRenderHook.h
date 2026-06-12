#pragma once

#include <HookContext/HookContext.h>
#include <Features/Visuals/SkyChanger/SkyChanger.h>
#include <Features/Visuals/SkyChanger/SkyTextureMapping.h>

// Integration point for SkyChanger into the render hook
template <typename HookContext>
void integrateSkyChanterIntoViewRender(HookContext& hookContext) noexcept
{
    // Check if sky changer is enabled
    const auto& skyChangerState = hookContext.featuresStates().visualFeaturesStates.skyChangerState;
    
    if (skyChangerState.enabled) {
        // Create sky changer instance and apply current sky
        auto&& skyChanger = hookContext.template make<SkyChanger<HookContext>>();
        skyChanger.applySkyTexture();
    }
}