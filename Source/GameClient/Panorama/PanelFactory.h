#pragma once

#include <CS2/Panorama/CImagePanel.h>
#include <CS2/Panorama/CLabel.h>
#include <CS2/Panorama/CUIPanel.h>
#include <GameClient/MemAlloc.h>
#include <MemoryPatterns/PatternTypes/PanelPatternTypes.h>
#include <MemoryPatterns/PatternTypes/PanoramaImagePanelPatternTypes.h>
#include <MemoryPatterns/PatternTypes/PanoramaLabelPatternTypes.h>
#include <GameClient/Panorama/ClientPanel.h>
#include <GameClient/Panorama/PanoramaImagePanel.h>

template <typename HookContext>
struct PanelFactory {
    explicit PanelFactory(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] decltype(auto) createPanel(cs2::CUIPanel* parentPanel, const char* id = "") noexcept
    {
        if (parentPanel && panelConstructor())
            return hookContext.template make<ClientPanel>(panelConstructor()(id, parentPanel->clientPanel));
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
