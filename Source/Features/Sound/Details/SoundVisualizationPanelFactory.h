#pragma once

#include <cassert>

#include <CS2/Classes/Panorama.h>
#include <GameClasses/Panel.h>
#include <GameClasses/PanoramaUiPanel.h>

#include "SoundVisualizationPanelProperties.h"

class SoundVisualizationPanelFactory {
public:
    SoundVisualizationPanelFactory(cs2::CUIPanel& parentPanel, PanelConfigurator panelConfigurator) noexcept
        : parentPanel{parentPanel}
        , panelConfigurator{panelConfigurator}
    {
    }

    [[nodiscard]] PanoramaUiPanel createSoundVisualizationPanel(const SoundVisualizationPanelProperties& properties) const noexcept
    {
        const auto containerPanel{Panel::create("", &parentPanel)};
        if (!containerPanel)
            return PanoramaUiPanel{nullptr};

        if (const auto style{PanoramaUiPanel{containerPanel->uiPanel}.getStyle()}) {
            const auto styler{panelConfigurator.panelStyle(*style)};
            styler.setWidth(cs2::CUILength::pixels(kWidth));
            styler.setHeight(cs2::CUILength::pixels(kHeight));
            if (properties.position == SoundVisualizationPosition::AboveOrigin) {
                styler.setPosition(cs2::CUILength::pixels(-kWidth * 0.5f), cs2::CUILength::pixels(-kHeight));
                styler.setTransformOrigin(cs2::CUILength::percent(50), cs2::CUILength::percent(100));
            } else {
                assert(properties.position == SoundVisualizationPosition::AtOrigin);
                styler.setPosition(cs2::CUILength::pixels(-kWidth * 0.5f), cs2::CUILength::pixels(-kHeight * 0.5f));
            }
        }

        applyStyleToImagePanel(PanoramaImagePanel::create("", containerPanel->uiPanel), properties);
        return PanoramaUiPanel{containerPanel->uiPanel};
    }

private:
    void applyStyleToImagePanel(cs2::CImagePanel* imagePanel, const SoundVisualizationPanelProperties& properties) const noexcept
    {
        if (!imagePanel)
            return;

        PanoramaImagePanel{imagePanel}.setImageSvg(properties.svgImagePath, properties.svgTextureHeight);
        if (const auto style{PanoramaUiPanel{imagePanel->uiPanel}.getStyle()}) {
            const auto styler{panelConfigurator.panelStyle(*style)};
            styler.setAlign(cs2::k_EHorizontalAlignmentCenter, imageVerticalAlignment(properties.position));
            styler.setImageShadow(imageShadowParams());
        }
    }

    [[nodiscard]] static PanelShadowParams imageShadowParams() noexcept
    {
        return PanelShadowParams{
            .horizontalOffset{cs2::CUILength::pixels(0)},
            .verticalOffset{cs2::CUILength::pixels(0)},
            .blurRadius{cs2::CUILength::pixels(1)},
            .strength = 3,
            .color{0, 0, 0}
        };
    }

    [[nodiscard]] static cs2::EVerticalAlignment imageVerticalAlignment(SoundVisualizationPosition position) noexcept
    {
        if (position == SoundVisualizationPosition::AboveOrigin)
            return cs2::k_EVerticalAlignmentBottom;
        return cs2::k_EVerticalAlignmentCenter;
    }

    static constexpr auto kWidth{256};
    static constexpr auto kHeight{256};

    cs2::CUIPanel& parentPanel;
    PanelConfigurator panelConfigurator;
};
