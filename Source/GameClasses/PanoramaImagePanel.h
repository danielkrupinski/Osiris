#pragma once

#include <CS2/Classes/Panorama.h>
#include <GameDependencies/PanoramaImagePanelDeps.h>

template <typename HookContext>
struct PanoramaImagePanelContext {
    PanoramaImagePanelContext(HookContext& hookContext, cs2::CImagePanel* panel) noexcept
        : hookContext{hookContext}
        , panel{panel}
    {
    }

    [[nodiscard]] decltype(auto) uiPanel() const noexcept
    {
        return hookContext.template make<PanoramaUiPanel>(panel->uiPanel);
    }

    [[nodiscard]] const char* getImagePath() const noexcept
    {
        if (auto&& imagePath = PanoramaImagePanelDeps::instance().offsetToImagePath.of(panel).get())
            return imagePath->m_pString;
        return nullptr;
    }

    HookContext& hookContext;
    cs2::CImagePanel* panel;
};

struct PanoramaImagePanelFactory {
    [[nodiscard]] static cs2::CImagePanel* create(const char* id, cs2::CUIPanel* parent) noexcept
    {
        if (!PanoramaImagePanelDeps::instance().constructor || !PanoramaImagePanelDeps::instance().size)
            return nullptr;

        const auto memory{static_cast<cs2::CImagePanel*>(MemAlloc::allocate(*PanoramaImagePanelDeps::instance().size))};
        if (memory)
            PanoramaImagePanelDeps::instance().constructor(memory, parent->clientPanel, id);
        return memory;
    }
};

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
        return PanoramaImagePanelDeps::instance().imagePropertiesOffset.of(context.panel).get();
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

        properties->scale = context.uiPanel().getUiScaleFactor();
        properties->textureHeight = textureHeight;
        if (PanoramaImagePanelDeps::instance().setImage)
            PanoramaImagePanelDeps::instance().setImage(context.panel, imageUrl, nullptr, properties);
    }

private:
    Context context;
};

template <typename HookContext>
PanoramaImagePanel(HookContext&, cs2::CPanel2D*) -> PanoramaImagePanel<PanoramaImagePanelContext<HookContext>>;
