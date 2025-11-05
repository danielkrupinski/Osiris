#pragma once

#include <bit>
#include <cstddef>
#include <optional>
#include <string_view>
#include <utility>

#include <CS2/Classes/Color.h>
#include <CS2/Panorama/CImagePanel.h>
#include <MemoryPatterns/PatternTypes/PanoramaImagePanelPatternTypes.h>

struct SvgImageParams {
    const char* imageUrl;
    int textureHeight{-1};
    std::optional<cs2::Color> fillColor{};
};

template <typename HookContext>
class ImagePanel {
public:
    using RawType = cs2::CImagePanel;

    ImagePanel(HookContext& hookContext, cs2::CImagePanel* panel) noexcept
        : hookContext{hookContext}
        , panel{panel}
    {
    }

    [[nodiscard]] decltype(auto) uiPanel() const noexcept
    {
        return hookContext.uiPanel(panel ? panel->uiPanel : nullptr);
    }

    [[nodiscard]] cs2::ImageProperties* getImageProperties() const noexcept
    {
        return hookContext.patternSearchResults().template get<ImagePropertiesOffset>().of(panel).get();
    }

    [[nodiscard]] std::string_view getImagePath() const noexcept
    {
        if (auto&& imagePath = hookContext.patternSearchResults().template get<OffsetToImagePath>().of(panel).get(); imagePath && imagePath->m_pString)
            return imagePath->m_pString;
        return {};
    }

    void setImageSvg(const char* imageUrl, int textureHeight = -1) const noexcept
    {
        setImageSvg(SvgImageParams{.imageUrl = imageUrl, .textureHeight = textureHeight});
    }

    void setImageSvg(const SvgImageParams& params) const noexcept
    {
        const auto properties{getImageProperties()};
        if (!properties)
            return;

        properties->scale = uiScaleFactor();
        properties->textureHeight = params.textureHeight;

        if (params.fillColor.has_value()) {
            properties->svgAttributes[static_cast<std::size_t>(cs2::SvgAttributeType::FillColor)] = std::bit_cast<cs2::SvgAttribute>(*params.fillColor);
            properties->presentSvgAttributes |= 1 << static_cast<std::size_t>(cs2::SvgAttributeType::FillColor);
        }

        if (hookContext.patternSearchResults().template get<SetImageFunctionPointer>())
            hookContext.patternSearchResults().template get<SetImageFunctionPointer>()(panel, params.imageUrl, nullptr, properties);
    }

private:
    [[nodiscard]] decltype(auto) uiScaleFactor() const
    {
        const auto scale = uiPanel().getUiScaleFactor().valueOr(1.0f);
        assert(scale >= 0.1f && scale <= 10.0f && "Invalid UI scale factor");
        return scale;
    }

    HookContext& hookContext;
    cs2::CImagePanel* panel;
};
