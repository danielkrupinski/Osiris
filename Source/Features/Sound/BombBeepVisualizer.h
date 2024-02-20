#pragma once

#include <algorithm>

#include <CS2/Classes/Panorama.h>
#include <CS2/Constants/SoundNames.h>
#include <FeatureHelpers/HudInWorldPanels.h>
#include <FeatureHelpers/HudInWorldPanelFactory.h>
#include <FeatureHelpers/HudInWorldPanelZOrder.h>
#include <FeatureHelpers/PanoramaTransformations.h>
#include "Details/SoundVisualizationFeature.h"
#include <FeatureHelpers/Sound/SoundWatcher.h>
#include <FeatureHelpers/TogglableFeature.h>
#include <FeatureHelpers/WorldToClipSpaceConverter.h>
#include <GameClasses/Panel.h>
#include <GameClasses/PanoramaUiEngine.h>
#include <Helpers/HudProvider.h>
#include <Helpers/PanoramaPanelPointer.h>
#include <Helpers/PanoramaTransformFactory.h>
#include <Hooks/ViewRenderHook.h>

struct BombBeepPanels {
    [[nodiscard]] static cs2::CPanel2D* createContainerPanel(const HudInWorldPanelFactory& inWorldFactory) noexcept
    {
        return inWorldFactory.createPanel("BombBeepContainer", HudInWorldPanelZOrder::BombBeep);
    }

    static void createContentPanels(cs2::CUIPanel& containerPanel, PanelConfigurator panelConfigurator) noexcept
    {
        for (std::size_t i = 0; i < kMaxNumberOfPanels; ++i) {
            const auto panel = Panel::create("", &containerPanel);
            if (!panel)
                continue;

            if (const auto style{PanoramaUiPanel{panel->uiPanel}.getStyle()}) {
                const auto styleConfigurator{panelConfigurator.panelStyle(*style)};
                styleConfigurator.setWidth(cs2::CUILength::pixels(100));
                styleConfigurator.setHeight(cs2::CUILength::pixels(100));
                styleConfigurator.setPosition(cs2::CUILength::pixels(-50), cs2::CUILength::pixels(-50));
            }

            const auto imagePanel{PanoramaImagePanel::create("", panel->uiPanel)};
            if (!imagePanel)
                continue;

            PanoramaImagePanel{imagePanel}.setImageSvg("s2r://panorama/images/icons/ui/bomb_c4.svg", 40);
            if (const auto style{PanoramaUiPanel{imagePanel->uiPanel}.getStyle()}) {
                const auto styleSetter{panelConfigurator.panelStyle(*style)};
                styleSetter.setAlign(cs2::k_EHorizontalAlignmentCenter, cs2::k_EVerticalAlignmentCenter);
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

    static constexpr auto kMaxNumberOfPanels = 5;
};

using BombBeepVisualizerState = SoundVisualizationFeatureState<BombBeepPanels>;
using BombBeepVisualizer = SoundVisualizationFeature<BombBeepPanels, BombBeepSound>;
