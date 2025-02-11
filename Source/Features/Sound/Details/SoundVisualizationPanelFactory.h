#pragma once

#include <cassert>

#include <CS2/Constants/ColorConstants.h>
#include <CS2/Panorama/CUILength.h>
#include <CS2/Panorama/CUIPanel.h>
#include <CS2/Panorama/StyleEnums.h>
#include <GameClient/Panorama/PanelAlignmentParams.h>
#include <GameClient/Panorama/PanelShadowParams.h>
#include <Utils/Lvalue.h>

#include "SoundVisualizationPanelProperties.h"

template <typename HookContext>
class SoundVisualizationPanelFactory {
public:
    SoundVisualizationPanelFactory(HookContext& hookContext, cs2::CUIPanel& parentPanel) noexcept
        : hookContext{hookContext}
        , parentPanel{parentPanel}
    {
    }

    [[nodiscard]] decltype(auto) createSoundVisualizationPanel(const SoundVisualizationPanelProperties& properties) const noexcept
    {
        auto&& panelFactory = hookContext.panelFactory();
        auto&& containerPanel = panelFactory.createPanel(&parentPanel).uiPanel();

        containerPanel.setWidth(cs2::CUILength::pixels(kWidth));
        containerPanel.setHeight(cs2::CUILength::pixels(kHeight));
        if (properties.position == SoundVisualizationPosition::AboveOrigin) {
            containerPanel.setPosition(cs2::CUILength::pixels(-kWidth * 0.5f), cs2::CUILength::pixels(-kHeight));
            containerPanel.setTransformOrigin(cs2::CUILength::percent(50), cs2::CUILength::percent(100));
        } else {
            assert(properties.position == SoundVisualizationPosition::AtOrigin);
            containerPanel.setPosition(cs2::CUILength::pixels(-kWidth * 0.5f), cs2::CUILength::pixels(-kHeight * 0.5f));
        }

        applyStyleToImagePanel(panelFactory.createImagePanel(containerPanel), properties);
        return utils::lvalue<decltype(containerPanel)>(containerPanel);
    }

private:
    void applyStyleToImagePanel(auto&& imagePanel, const SoundVisualizationPanelProperties& properties) const noexcept
    {
        imagePanel.setImageSvg(properties.svgImagePath, properties.svgTextureHeight);
        auto&& uiPanel = imagePanel.uiPanel();
        uiPanel.setAlign(PanelAlignmentParams{cs2::k_EHorizontalAlignmentCenter, imageVerticalAlignment(properties.position)});
        uiPanel.setImageShadow(imageShadowParams());
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
