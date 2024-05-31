#pragma once

#include <CS2/Classes/Panorama.h>
#include <GameClasses/Implementation/PanoramaUiPanelImpl.h>
#include <GameClasses/TopLevelWindow.h>

struct PanoramaUiPanel {
    explicit PanoramaUiPanel(cs2::CUIPanel* thisptr) noexcept
        : thisptr{ thisptr }
    {
    }

    [[nodiscard]] cs2::CPanel2D* getClientPanel() const noexcept
    {
        return thisptr->clientPanel;
    }
    
    void setParent(cs2::CUIPanel* parent) const noexcept
    {
        if (const auto fn = impl().setParent.of(thisptr->vmt).get())
            (*fn)(thisptr, parent);
    }

    void setVisible(bool visible) const noexcept
    {
        if (const auto fn = impl().setVisible.of(thisptr->vmt).get())
            (*fn)(thisptr, visible);
    }

    [[nodiscard]] PanoramaUiPanel findChildInLayoutFile(const char* childId) const noexcept
    {
        if (!impl().offsetToPanelFlags || !impl().offsetToPanelId)
            return PanoramaUiPanel{nullptr};
        return PanoramaUiPanel{findChildInLayoutFileInternal(thisptr, childId)};
    }

    [[nodiscard]] const char* getAttributeString(cs2::CPanoramaSymbol attributeName, const char* defaultValue) const noexcept
    {
        if (const auto fn = impl().getAttributeString.of(thisptr->vmt).get())
            return (*fn)(thisptr, attributeName, defaultValue);
        return nullptr;
    }

    void setAttributeString(cs2::CPanoramaSymbol attributeName, const char* value) const noexcept
    {
        if (const auto fn = impl().setAttributeString.of(thisptr->vmt).get())
            (*fn)(thisptr, attributeName, value);
    }

    [[nodiscard]] cs2::CUIPanel::childrenVector* children() const noexcept
    {
        return impl().childPanels.of(thisptr).get();
    }

    [[nodiscard]] bool hasClass(cs2::CPanoramaSymbol className) const noexcept
    {
        const cs2::CUIPanel::classesVector* classes = impl().classes.of(thisptr).get();
        if (!classes)
            return false;

        for (int i = 0; i < classes->size; ++i) {
            if (classes->memory[i] == className)
                return true;
        }
        return false;
    }

    [[nodiscard]] cs2::CPanelStyle* getStyle() const noexcept
    {
        return impl().panelStyle.of(thisptr).get();
    }

    [[nodiscard]] auto getParentWindow() const noexcept
    {
        return TopLevelWindow{impl().parentWindowOffset.of(thisptr).valueOr(nullptr)};
    }

    explicit(false) operator cs2::CUIPanel*() const noexcept
    {
        return thisptr;
    }

    explicit operator bool() const noexcept
    {
        return thisptr != nullptr;
    }

private:
    [[nodiscard]] cs2::CUIPanel* findChildInLayoutFileInternal(cs2::CUIPanel* parentPanel, const char* idToFind) const noexcept
    {
        const auto children = impl().childPanels.of(parentPanel).get();
        if (!children)
            return nullptr;

        for (int i = 0; i < children->size; ++i) {
            const auto panel = children->memory[i];
            const auto panelId = impl().offsetToPanelId.of(panel).get()->m_pString;
            if (panelId && std::strcmp(panelId, idToFind) == 0)
                return panel;
        }

        for (int i = 0; i < children->size; ++i) {
            const auto panel = children->memory[i];
            const auto panelFlags = *impl().offsetToPanelFlags.of(panel).get();
            if ((panelFlags & cs2::k_EPanelFlag_HasOwnLayoutFile) == 0) {
                if (const auto found = findChildInLayoutFileInternal(panel, idToFind))
                    return found;
            }
        }

        return nullptr;
    }

    [[nodiscard]] static const PanoramaUiPanelImpl& impl() noexcept
    {
        return PanoramaUiPanelImpl::instance();
    }
    
    cs2::CUIPanel* thisptr;
};
