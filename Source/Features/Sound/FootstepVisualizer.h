#pragma once

#include <algorithm>
#include <array>

#include <CS2/Classes/Panorama.h>
#include <CS2/Constants/SoundNames.h>
#include <FeatureHelpers/HudInWorldPanels.h>
#include <FeatureHelpers/HudInWorldPanelFactory.h>
#include <FeatureHelpers/PanoramaTransformations.h>
#include "Details/FootstepSound.h"
#include "Details/SoundVisualizationFeature.h"
#include <FeatureHelpers/Sound/SoundWatcher.h>
#include <FeatureHelpers/TogglableFeature.h>
#include <FeatureHelpers/WorldToClipSpaceConverter.h>
#include <GameClasses/PanoramaUiEngine.h>
#include <Helpers/HudProvider.h>
#include <Helpers/PanoramaTransformFactory.h>
#include <Hooks/ViewRenderHook.h>

struct FootstepPanels {
    static constexpr auto kMaxNumberOfPanels = 100;

    [[nodiscard]] static cs2::CPanel2D* createContainerPanel(const HudInWorldPanelFactory& inWorldFactory) noexcept
    {
        return inWorldFactory.createPanel("FootstepContainer", HudInWorldPanelZOrder::Footstep);
    }

    static void createContentPanels(cs2::CUIPanel& containerPanel, PanelConfigurator panelConfigurator) noexcept
    {
        for (std::size_t i = 0; i < kMaxNumberOfPanels; ++i) {
            const auto panel = Panel::create("", &containerPanel);
            if (!panel)
                continue;

            if (const auto style{PanoramaUiPanel{panel->uiPanel}.getStyle()}) {
                const auto styleConfigurator{panelConfigurator.panelStyle(*style)};
                styleConfigurator.setWidth(cs2::CUILength::pixels(50));
                styleConfigurator.setHeight(cs2::CUILength::pixels(50));
                styleConfigurator.setPosition(cs2::CUILength::pixels(-25), cs2::CUILength::pixels(-50));
                styleConfigurator.setTransformOrigin(cs2::CUILength::percent(50), cs2::CUILength::percent(100));
            }

            const auto imagePanel{PanoramaImagePanel::create("", panel->uiPanel)};
            if (!imagePanel)
                continue;

            PanoramaImagePanel{imagePanel}.setImageSvg("s2r://panorama/images/icons/equipment/stomp_damage.svg");
            if (const auto style{PanoramaUiPanel{imagePanel->uiPanel}.getStyle()}) {
                const auto styleSetter{panelConfigurator.panelStyle(*style)};
                styleSetter.setAlign(cs2::k_EHorizontalAlignmentCenter, cs2::k_EVerticalAlignmentBottom);
                styleSetter.setImageShadow(ImageShadowParams{
                    .horizontalOffset{cs2::CUILength::pixels(0)},
                    .verticalOffset{cs2::CUILength::pixels(0)},
                    .blurRadius{cs2::CUILength::pixels(1)},
                    .strength = 3,
                    .color{0, 0, 0}
                });
            }
        }
    }
};

using FootstepVisualizerState = SoundVisualizationFeatureState<FootstepPanels>;
using FootstepVisualizer = SoundVisualizationFeature<FootstepPanels, FootstepSound>;
