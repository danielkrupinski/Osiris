#pragma once

#include <CS2/Classes/Panorama.h>
#include <CS2/Constants/PanelIDs.h>
#include <GameClasses/Panel.h>
#include <Helpers/HudProvider.h>

class HudInWorldPanelFactory {
public:
    explicit HudInWorldPanelFactory(HudProvider hudProvider) noexcept
        : hudProvider{ hudProvider }
    {
    }

    [[nodiscard]] cs2::CPanel2D* createPanel(const char* id) const noexcept
    {
        if (const auto hudReticle = getHudReticle())
            return createChild(id, hudReticle);
        return nullptr;
    }

private:
    [[nodiscard]] cs2::CPanel2D* createChild(const char* id, cs2::CUIPanel* hudReticle) const noexcept
    {
        if (const auto panel = Panel::create(id, hudReticle)) {
            fitParent(PanoramaUiPanel{ panel->uiPanel });
            return panel;
        }
        return nullptr;
    }

    [[nodiscard]] cs2::CUIPanel* getHudReticle() const noexcept
    {
        return hudProvider.findChildInLayoutFile(cs2::HudReticle);
    }

    static void fitParent(PanoramaUiPanel panel) noexcept
    {
        if (const auto style = panel.getStyle()) {
            style.setWidth(cs2::CUILength{ 100.0f, cs2::CUILength::k_EUILengthPercent });
            style.setHeight(cs2::CUILength{ 100.0f, cs2::CUILength::k_EUILengthPercent });
        }
    }

    HudProvider hudProvider;
};
