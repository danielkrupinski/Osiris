#pragma once

#include <utility>

#include <Common/Visibility.h>
#include <GameClient/Entities/C4.h>
#include <GameClient/Panorama/ImagePanel.h>
#include <Utils/CString.h>
#include <Utils/StringBuilder.h>

template <typename HookContext>
class PlayerActiveWeaponIconPanel {
public:
    explicit PlayerActiveWeaponIconPanel(HookContext& hookContext, cs2::CUIPanel* uiPanel) noexcept
        : hookContext{hookContext}
        , uiPanel{uiPanel}
    {
    }

    void update(auto&& playerPawn, Visibility bombIconVisibility) const noexcept
    {
        if (!GET_CONFIG_VAR(player_info_vars::ActiveWeaponIconEnabled) || (bombIconVisibility == Visibility::Visible && playerPawn.getActiveWeapon().template is<C4>())) {
            panel().setVisible(false);
            return;
        }

        auto weaponName = CString{playerPawn.getActiveWeapon().getName()};
        if (!weaponName.string)
            return;
        weaponName.skipPrefix("weapon_");

        panel().setVisible(true);

        StringBuilderStorage<100> weaponIconPathStorage;
        auto weaponIconPathBuilder = weaponIconPathStorage.builder();
        weaponIconPathBuilder.put("s2r://panorama/images/icons/equipment/", weaponName.string, ".svg");
        const auto weaponIconPath = weaponIconPathBuilder.cstring();

        auto&& weaponIconImagePanel = panel().clientPanel().template as<ImagePanel>();
        if (shouldUpdateImagePanel(weaponIconImagePanel, weaponIconPath))
            weaponIconImagePanel.setImageSvg(weaponIconPath, 24);
    }

private:
    [[nodiscard]] bool shouldUpdateImagePanel(auto&& imagePanel, const char* newImagePath) const noexcept
    {
        return imagePanel.getImagePath() != newImagePath;
    }

    [[nodiscard]] decltype(auto) panel() const noexcept
    {
        return hookContext.template make<PanoramaUiPanel>(uiPanel);
    }

    HookContext& hookContext;
    cs2::CUIPanel* uiPanel;
};
