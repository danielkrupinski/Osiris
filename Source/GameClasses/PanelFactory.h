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
        if (parentPanel && panelDeps().create)
            return hookContext.template make<ClientPanel>(panelDeps().create(id, parentPanel));
        return hookContext.template make<ClientPanel>(nullptr);
    }

    [[nodiscard]] decltype(auto) createLabelPanel(cs2::CUIPanel* parentPanel, const char* id = "") const noexcept
    {
        if (!parentPanel || !labelPanelDeps().constructor || !labelPanelDeps().size)
            return hookContext.template make<ClientPanel>(nullptr);

        const auto memory{static_cast<cs2::CLabel*>(hookContext.template make<MemAlloc>().allocate(*labelPanelDeps().size))};
        if (memory)
            labelPanelDeps().constructor(memory, parentPanel->clientPanel, id);
        return hookContext.template make<ClientPanel>(memory);
    }

    [[nodiscard]] decltype(auto) createImagePanel(cs2::CUIPanel* parentPanel, const char* id = "") noexcept
    {
        if (!parentPanel || !imagePanelDeps().constructor || !imagePanelDeps().size)
            return hookContext.template make<ClientPanel>(nullptr).template as<PanoramaImagePanel>();

        const auto memory{static_cast<cs2::CImagePanel*>(hookContext.template make<MemAlloc>().allocate(*imagePanelDeps().size))};
        if (memory)
            imagePanelDeps().constructor(memory, parentPanel->clientPanel, id);
        return hookContext.template make<ClientPanel>(memory).template as<PanoramaImagePanel>();
    }

private:
    [[nodiscard]] const auto& panelDeps() const noexcept
    {
        return hookContext.gameDependencies().panelDeps;
    }

    [[nodiscard]] const auto& imagePanelDeps() const noexcept
    {
        return hookContext.gameDependencies().imagePanelDeps;
    }

    [[nodiscard]] const auto& labelPanelDeps() const noexcept
    {
        return hookContext.gameDependencies().panoramaLabelDeps;
    }

    HookContext& hookContext;
};
