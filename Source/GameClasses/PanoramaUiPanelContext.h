#pragma once

#include <CS2/Panorama/Transform3D.h>
#include <GameDependencies/PanelStyleDeps.h>

#include "ClientPanel.h"
#include "PanoramaUiPanelChildPanels.h"
#include "PanoramaUiPanelClasses.h"
#include "PanoramaUiPanelMethodInvoker.h"

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

    template <template <typename> typename T>
    [[nodiscard]] decltype(auto) as() const noexcept
    {
        return hookContext.template make<T>(panel);
    }

    [[nodiscard]] decltype(auto) getHandle() const noexcept
    {
        return hookContext.panels().getPanelHandle(panel);
    }

    void setSimpleForegroundColor(cs2::Color color) const noexcept
    {
        if (const auto style = getStyle()) {
            // FIXME: hardcoded virtual method index
            reinterpret_cast<void(*)(cs2::CPanelStyle* thisptr, const cs2::Color* color)>((*reinterpret_cast<void(***)()>(style))[54])(style, &color);
        }
    }

    void setTransform3D(const cs2::CUtlVector<cs2::CTransform3D*>& transforms) const noexcept
    {
        if (const auto style = getStyle()) {
            // FIXME: hardcoded virtual method index
            reinterpret_cast<void(*)(cs2::CPanelStyle* thisptr, const cs2::CUtlVector<cs2::CTransform3D*>* transforms)>((*reinterpret_cast<void(***)()>(style))[17])(style, &transforms);
        }
    }

    void setProperty(cs2::CStyleProperty* styleProperty) const noexcept
    {
        if (!styleProperty)
            return;

        const auto style = getStyle();
        if (!style)
            return;

        if (const auto setPropertyFn{PanelStyleDeps::instance().setProperty})
            setPropertyFn(style, styleProperty, true);
    }

    [[nodiscard]] PanoramaUiPanelClasses classes() const noexcept
    {
        return PanoramaUiPanelClasses{impl().classes.of(panel).get()};
    }

    [[nodiscard]] auto childPanels() const noexcept
    {
        return PanoramaUiPanelChildPanels{hookContext, impl().childPanels.of(panel).get()};
    }

    [[nodiscard]] auto getParentWindow() const noexcept
    {
        return TopLevelWindow{impl().parentWindowOffset.of(panel).valueOr(nullptr)};
    }

    [[nodiscard]] const char* getId() const noexcept
    {
        if (const auto id = impl().offsetToPanelId.of(panel).get())
            return id->m_pString;
        return nullptr;
    }

    [[nodiscard]] Optional<bool> hasFlag(cs2::EPanelFlag flag) const noexcept
    {
        return (impl().offsetToPanelFlags.of(panel).toOptional() & flag) != 0;
    }

    [[nodiscard]] auto setParent() const noexcept
    {
        return PanoramaUiPanelMethodInvoker{panel, impl().setParent};
    }

    [[nodiscard]] auto setVisible() const noexcept
    {
        return PanoramaUiPanelMethodInvoker{panel, impl().setVisible};
    }

    [[nodiscard]] auto getAttributeString() const noexcept
    {
        return PanoramaUiPanelMethodInvoker{panel, impl().getAttributeString};
    }

    [[nodiscard]] auto setAttributeString() const noexcept
    {
        return PanoramaUiPanelMethodInvoker{panel, impl().setAttributeString};
    }

    [[nodiscard]] auto propertyFactory() const noexcept
    {
        return PanelStylePropertyFactory{hookContext.gameDependencies().stylePropertiesSymbolsAndVMTs};
    }

    [[nodiscard]] decltype(auto) nullPanel() const noexcept
    {
        return hookContext.template make<PanoramaUiPanel>(nullptr);
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
        return impl().panelStyle.of(panel).get();
    }

    [[nodiscard]] static const PanoramaUiPanelDeps& impl() noexcept
    {
        return PanoramaUiPanelDeps::instance();
    }
    
    HookContext& hookContext;
    cs2::CUIPanel* panel;
};
