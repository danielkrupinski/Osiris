#pragma once

#include <GameClasses/PanoramaImagePanel.h>
#include <GameDependencies/PanoramaImagePanelDeps.h>
#include <GameDependencies/PanoramaLabelDeps.h>

template <typename HookContext>
struct PanelFactory {
    explicit PanelFactory(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] decltype(auto) createPanel(cs2::CUIPanel* parentPanel, const char* id = "") noexcept
    {
        if (parentPanel && PanelDeps::instance().create)
            return hookContext.template make<ClientPanel>(PanelDeps::instance().create(id, parentPanel));
        return hookContext.template make<ClientPanel>(nullptr);
    }

    [[nodiscard]] decltype(auto) createLabelPanel(cs2::CUIPanel* parentPanel, const char* id = "") const noexcept
    {
        if (!parentPanel || !PanoramaLabelDeps::instance().constructor || !PanoramaLabelDeps::instance().size)
            return hookContext.template make<ClientPanel>(nullptr);

        const auto memory{static_cast<cs2::CLabel*>(MemAlloc::allocate(*PanoramaLabelDeps::instance().size))};
        if (memory)
            PanoramaLabelDeps::instance().constructor(memory, parentPanel->clientPanel, id);
        return hookContext.template make<ClientPanel>(memory);
    }

    [[nodiscard]] decltype(auto) createImagePanel(cs2::CUIPanel* parentPanel, const char* id = "") noexcept
    {
        if (!parentPanel || !PanoramaImagePanelDeps::instance().constructor || !PanoramaImagePanelDeps::instance().size)
            return hookContext.template make<ClientPanel>(nullptr).template as<PanoramaImagePanel>();

        const auto memory{static_cast<cs2::CImagePanel*>(MemAlloc::allocate(*PanoramaImagePanelDeps::instance().size))};
        if (memory)
            PanoramaImagePanelDeps::instance().constructor(memory, parentPanel->clientPanel, id);
        return hookContext.template make<ClientPanel>(memory).template as<PanoramaImagePanel>();
    }

private:
    HookContext& hookContext;
};
