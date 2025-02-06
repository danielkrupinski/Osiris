#pragma once

#include <bit>
#include <cstddef>
#include <optional>
#include <string_view>
#include <utility>

#include <CS2/Classes/Color.h>
#include <CS2/Panorama/CImagePanel.h>
#include <MemoryPatterns/PatternTypes/PanoramaImagePanelPatternTypes.h>

#include "PanoramaImagePanelContext.h"

struct SvgImageParams {
    const char* imageUrl;
    int textureHeight{-1};
    std::optional<cs2::Color> fillColor{};
};

template <typename HookContext, typename Context = PanoramaImagePanelContext<HookContext>>
struct PanoramaImagePanel {
    using RawType = cs2::CImagePanel;

    template <typename... Args>
        requires std::is_constructible_v<Context, Args...>
    PanoramaImagePanel(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] decltype(auto) uiPanel() const noexcept
    {
        return context.uiPanel();
    }

    [[nodiscard]] cs2::ImageProperties* getImageProperties() const noexcept
    {
        return context.hookContext.clientPatternSearchResults().template get<ImagePropertiesOffset>().of(context.panel).get();
    }

    [[nodiscard]] std::string_view getImagePath() const noexcept
    {
        if (auto&& imagePath = context.getImagePath())
            return imagePath;
        return {};
    }

    void setImageSvg(const char* imageUrl, int textureHeight = -1) const noexcept
    {
        setImageSvg(SvgImageParams{.imageUrl = imageUrl, .textureHeight = textureHeight});
    }

    void setImageSvg(const SvgImageParams& params) const noexcept
    {
        if (context.panel == nullptr)
            return;

        const auto properties{getImageProperties()};
        if (!properties)
            return;

        properties->scale = context.uiPanel().getUiScaleFactor().valueOr(1.0f);
        properties->textureHeight = params.textureHeight;

        if (params.fillColor.has_value()) {
            properties->svgAttributes[static_cast<std::size_t>(cs2::SvgAttributeType::FillColor)] = std::bit_cast<cs2::SvgAttribute>(*params.fillColor);
            properties->presentSvgAttributes |= 1 << static_cast<std::size_t>(cs2::SvgAttributeType::FillColor);
        }

        if (context.hookContext.clientPatternSearchResults().template get<SetImageFunctionPointer>())
            context.hookContext.clientPatternSearchResults().template get<SetImageFunctionPointer>()(context.panel, params.imageUrl, nullptr, properties);
    }

private:
    Context context;
};
