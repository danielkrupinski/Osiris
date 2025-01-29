#pragma once

#include <Panorama/PanoramaImagePanel.h>

template <typename HookContext>
struct PanelFactory {
    explicit PanelFactory(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] decltype(auto) createPanel(cs2::CUIPanel* parentPanel, const char* id = "") noexcept
    {
        if (parentPanel && panelConstructor())
            return hookContext.template make<ClientPanel>(panelConstructor()(id, parentPanel));
        return hookContext.template make<ClientPanel>(nullptr);
    }

    [[nodiscard]] decltype(auto) createLabelPanel(cs2::CUIPanel* parentPanel, const char* id = "") const noexcept
    {
        if (!parentPanel || !labelPanelConstructor() || !labelPanelSize())
            return hookContext.template make<ClientPanel>(nullptr);

        const auto memory{static_cast<cs2::CLabel*>(hookContext.template make<MemAlloc>().allocate(labelPanelSize()))};
        if (memory)
            labelPanelConstructor()(memory, parentPanel->clientPanel, id);
        return hookContext.template make<ClientPanel>(memory);
    }

    [[nodiscard]] decltype(auto) createImagePanel(cs2::CUIPanel* parentPanel, const char* id = "") noexcept
    {
        if (!parentPanel || !imagePanelConstructor() || !imagePanelSize())
            return hookContext.template make<ClientPanel>(nullptr).template as<PanoramaImagePanel>();

        const auto memory{static_cast<cs2::CImagePanel*>(hookContext.template make<MemAlloc>().allocate(imagePanelSize()))};
        if (memory)
            imagePanelConstructor()(memory, parentPanel->clientPanel, id);
        return hookContext.template make<ClientPanel>(memory).template as<PanoramaImagePanel>();
    }

private:
    [[nodiscard]] auto panelConstructor() const noexcept
    {
        return hookContext.clientPatternSearchResults().template get<PanelConstructorPointer>();
    }

    [[nodiscard]] auto imagePanelConstructor() const noexcept
    {
        return hookContext.clientPatternSearchResults().template get<ImagePanelConstructorPointer>();
    }

    [[nodiscard]] auto imagePanelSize() const noexcept
    {
        return hookContext.clientPatternSearchResults().template get<ImagePanelClassSize>();
    }

    [[nodiscard]] auto labelPanelConstructor() const noexcept
    {
        return hookContext.clientPatternSearchResults().template get<LabelPanelConstructorPointer>();
    }

    [[nodiscard]] auto labelPanelSize() const noexcept
    {
        return hookContext.clientPatternSearchResults().template get<LabelPanelObjectSize>();
    }

    HookContext& hookContext;
};
