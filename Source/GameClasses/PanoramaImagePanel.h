#pragma once

#include <CS2/Panorama/CImagePanel.h>
#include <MemoryPatterns/PatternTypes/PanoramaImagePanelPatternTypes.h>

#include "PanoramaImagePanelContext.h"

template <typename Context>
struct PanoramaImagePanel {
    explicit PanoramaImagePanel(Context context) noexcept
        : context{context}
    {
    }

    template <typename HookContext>
    PanoramaImagePanel(HookContext& hookContext, cs2::CPanel2D* panel) noexcept
        :  context{hookContext, static_cast<cs2::CImagePanel*>(panel)}
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
        if (context.panel == nullptr)
            return;

        const auto properties{getImageProperties()};
        if (!properties)
            return;

        properties->scale = context.uiPanel().getUiScaleFactor().valueOr(1.0f);
        properties->textureHeight = textureHeight;
        if (context.hookContext.clientPatternSearchResults().template get<SetImageFunctionPointer>())
            context.hookContext.clientPatternSearchResults().template get<SetImageFunctionPointer>()(context.panel, imageUrl, nullptr, properties);
    }

private:
    Context context;
};

template <typename HookContext>
PanoramaImagePanel(HookContext&, cs2::CPanel2D*) -> PanoramaImagePanel<PanoramaImagePanelContext<HookContext>>;
