#pragma once

#include <utility>

#include <CS2/Classes/CUtlVector.h>
#include <CS2/Classes/Color.h>
#include <CS2/Panorama/CPanel2D.h>
#include <CS2/Panorama/CPanelStyle.h>
#include <CS2/Panorama/CUIPanel.h>
#include <CS2/Panorama/StyleProperties.h>
#include <CS2/Panorama/Transform3D.h>
#include <MemoryPatterns/PatternTypes/PanelStylePatternTypes.h>
#include <MemoryPatterns/PatternTypes/UiPanelPatternTypes.h>
#include <Utils/Optional.h>

#include "ClientPanel.h"
#include "PanelStylePropertyFactory.h"
#include "PanoramaUiEngine.h"
#include "PanoramaUiPanelChildPanels.h"
#include "PanoramaUiPanelClasses.h"
#include "PanoramaUiPanelMethodInvoker.h"
#include "TopLevelWindow.h"

template <typename HookContext>
struct PanoramaUiPanelContext {
    PanoramaUiPanelContext(HookContext& hookContext, cs2::CUIPanel* panel) noexcept
        : hookContext{hookContext}
        , panel{panel}
    {
    }

    [[nodiscard]] decltype(auto) clientPanel() const noexcept
    {
        return hookContext.template make<ClientPanel>(clientPanelPointer());
    }

    template <template <typename...> typename T, typename... Args>
    [[nodiscard]] decltype(auto) as(Args&&... args) const noexcept
    {
        return hookContext.template make<T>(panel, std::forward<Args>(args)...);
    }

    [[nodiscard]] decltype(auto) getHandle() const noexcept
    {
        return hookContext.patternSearchResults().template get<OffsetToPanelHandle>().of(panel).valueOr(cs2::PanelHandle{});
    }

    void setProperty(cs2::CStyleProperty* styleProperty) const noexcept
    {
        if (!styleProperty)
            return;

        const auto style = getStyle();
        if (!style)
            return;

        if (const auto setPropertyFn{hookContext.patternSearchResults().template get<SetPanelStylePropertyFunctionPointer>()})
            setPropertyFn(style, styleProperty, true);
    }

    [[nodiscard]] PanoramaUiPanelClasses classes() const noexcept
    {
        return PanoramaUiPanelClasses{hookContext.patternSearchResults().template get<PanelClassesVectorOffset>().of(panel).get()};
    }

    [[nodiscard]] auto childPanels() const noexcept
    {
        return PanoramaUiPanelChildPanels{hookContext, hookContext.patternSearchResults().template get<ChildPanelsVectorOffset>().of(panel).get()};
    }

    [[nodiscard]] decltype(auto) getParentWindow() const noexcept
    {
        return hookContext.template make<TopLevelWindow>(hookContext.patternSearchResults().template get<ParentWindowOffset>().of(panel).valueOr(nullptr));
    }

    [[nodiscard]] const char* getId() const noexcept
    {
        if (const auto id = hookContext.patternSearchResults().template get<OffsetToPanelId>().of(panel).get())
            return id->m_pString;
        return nullptr;
    }

    [[nodiscard]] Optional<bool> hasFlag(cs2::EPanelFlag flag) const noexcept
    {
        return (hookContext.patternSearchResults().template get<OffsetToPanelFlags>().of(panel).toOptional() & flag) != 0;
    }

    [[nodiscard]] auto setParent() const noexcept
    {
        return PanoramaUiPanelMethodInvoker{panel, hookContext.patternSearchResults().template get<SetParentFunctionOffset>()};
    }

    [[nodiscard]] auto setVisible() const noexcept
    {
        return PanoramaUiPanelMethodInvoker{panel, hookContext.patternSearchResults().template get<SetVisibleFunctionOffset>()};
    }

    [[nodiscard]] auto getAttributeString() const noexcept
    {
        return PanoramaUiPanelMethodInvoker{panel, hookContext.patternSearchResults().template get<GetAttributeStringFunctionOffset>()};
    }

    [[nodiscard]] auto setAttributeString() const noexcept
    {
        return PanoramaUiPanelMethodInvoker{panel, hookContext.patternSearchResults().template get<SetAttributeStringFunctionOffset>()};
    }

    [[nodiscard]] auto propertyFactory() const noexcept
    {
        return hookContext.template make<PanelStylePropertyFactory>(hookContext.stylePropertySymbolsAndVMTs());
    }

    [[nodiscard]] decltype(auto) nullPanel() const noexcept
    {
        return hookContext.uiPanel(nullptr);
    }

    [[nodiscard]] cs2::CUIPanel* getRawPointer() const noexcept
    {
        return panel;
    }

private:
    [[nodiscard]] cs2::CPanel2D* clientPanelPointer() const noexcept
    {
        if (panel)
            return panel->clientPanel;
        return nullptr;
    }

    [[nodiscard]] cs2::CPanelStyle* getStyle() const noexcept
    {
        return hookContext.patternSearchResults().template get<PanelStyleOffset>().of(panel).get();
    }

    HookContext& hookContext;
    cs2::CUIPanel* panel;
};
