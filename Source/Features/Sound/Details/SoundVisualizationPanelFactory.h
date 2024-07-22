#pragma once

#include <cassert>

#include <CS2/Classes/Panorama.h>
#include <CS2/Constants/ColorConstants.h>
#include <GameClasses/Panel.h>
#include <GameClasses/PanoramaUiPanel.h>

#include "SoundVisualizationPanelProperties.h"

template <typename HookContext>
class SoundVisualizationPanelFactory {
public:
    SoundVisualizationPanelFactory(HookContext& hookContext, cs2::CUIPanel& parentPanel) noexcept
        : hookContext{hookContext}
        , parentPanel{parentPanel}
    {
    }

    [[nodiscard]] auto createSoundVisualizationPanel(const SoundVisualizationPanelProperties& properties) const noexcept
    {
        const auto containerPanel{Panel::create("", &parentPanel)};
        if (!containerPanel)
            return PanoramaUiPanel{PanoramaUiPanelContext{hookContext, nullptr}};

        PanoramaUiPanel panel{PanoramaUiPanelContext{hookContext, containerPanel->uiPanel}};

        panel.setWidth(cs2::CUILength::pixels(kWidth));
        panel.setHeight(cs2::CUILength::pixels(kHeight));
        if (properties.position == SoundVisualizationPosition::AboveOrigin) {
            panel.setPosition(cs2::CUILength::pixels(-kWidth * 0.5f), cs2::CUILength::pixels(-kHeight));
            panel.setTransformOrigin(cs2::CUILength::percent(50), cs2::CUILength::percent(100));
        }
        else {
            assert(properties.position == SoundVisualizationPosition::AtOrigin);
            panel.setPosition(cs2::CUILength::pixels(-kWidth * 0.5f), cs2::CUILength::pixels(-kHeight * 0.5f));
        }

        applyStyleToImagePanel(PanoramaImagePanelFactory::create("", containerPanel->uiPanel), properties);
        return panel;
    }

private:
    void applyStyleToImagePanel(cs2::CImagePanel* imagePanel, const SoundVisualizationPanelProperties& properties) const noexcept
    {
        if (!imagePanel)
            return;

        PanoramaImagePanel{PanoramaImagePanelContext{hookContext, imagePanel}}.setImageSvg(properties.svgImagePath, properties.svgTextureHeight);
        PanoramaUiPanel panel{PanoramaUiPanelContext{hookContext, imagePanel->uiPanel}};
        panel.setAlign(cs2::k_EHorizontalAlignmentCenter, imageVerticalAlignment(properties.position));
        panel.setImageShadow(imageShadowParams());
    }

    [[nodiscard]] static PanelShadowParams imageShadowParams() noexcept
    {
        return PanelShadowParams{
            .horizontalOffset{cs2::CUILength::pixels(0)},
            .verticalOffset{cs2::CUILength::pixels(0)},
            .blurRadius{cs2::CUILength::pixels(1)},
            .strength = 3,
            .color{cs2::kColorBlack}
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

    HookContext& hookContext;
    cs2::CUIPanel& parentPanel;
};
