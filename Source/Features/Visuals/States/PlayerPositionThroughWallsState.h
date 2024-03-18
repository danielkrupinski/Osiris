#pragma once

#include <CS2/Classes/Panorama.h>
#include <FeatureHelpers/HudInWorldPanelIndex.h>
#include <HookDependencies/HookDependenciesMask.h>
#include <Utils/DynamicArray.h>

struct PlayerPositionThroughWallsState {
    bool enabled{false};
    cs2::PanelHandle containerPanelHandle;
    DynamicArray<HudInWorldPanelIndex> panelIndices;

    [[nodiscard]] HookDependenciesMask getRenderingHookDependencies() const noexcept
    {
        if (enabled)
            return kCrucialDependencies | kExtraDependencies;
        return {};
    }

    static constexpr auto kCrucialDependencies{
        HookDependenciesMask{}
            .set<OffsetToGameSceneNode>()
            .set<OffsetToAbsOrigin>()
            .set<WorldToClipSpaceConverter>()
            .set<HudInWorldPanelContainer>()
            .set<HudProvider>()
            .set<PanelConfigurator>()
            .set<PanoramaTransformFactory>()};

    static constexpr auto kExtraDependencies{HookDependenciesMask{}.set<OffsetToTeamNumber>()};
};
