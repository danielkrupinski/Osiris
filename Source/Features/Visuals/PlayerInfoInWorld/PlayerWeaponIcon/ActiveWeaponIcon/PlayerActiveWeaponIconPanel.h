#pragma once

#include <utility>

#include <Common/Visibility.h>
#include <GameClient/Panorama/ImagePanel.h>
#include <Utils/CString.h>
#include <Utils/StringBuilder.h>
#include "PlayerActiveWeaponIconPanelContext.h"

template <typename HookContext, typename Context = PlayerActiveWeaponIconPanelContext<HookContext>>
class PlayerActiveWeaponIconPanel {
public:
    template <typename... Args>
    explicit PlayerActiveWeaponIconPanel(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void update(auto&& playerPawn, Visibility bombIconVisibility) const noexcept
    {
        if (!context.config().template getVariable<player_info_vars::ActiveWeaponIconEnabled>()) {
            context.panel().setVisible(false);
            return;
        }

        const auto activeWeapon = playerPawn.getActiveWeapon();
        if (bombIconVisibility == Visibility::Visible && activeWeapon.isC4()) {
            context.panel().setVisible(false);
            return;
        }

        const auto definitionName = activeWeapon.getName();
        if (!definitionName) {
            context.panel().setVisible(false);
            return;
        }

        auto weaponName = CString{definitionName};
        if (!weaponName.skipPrefix("weapon_")) {
            context.panel().setVisible(false);
            return;
        }

        context.panel().setVisible(true);

        StringBuilderStorage<100> weaponIconPathStorage;
        auto weaponIconPathBuilder = weaponIconPathStorage.builder();
        weaponIconPathBuilder.put("s2r://panorama/images/icons/equipment/", weaponName.string, ".svg");
        const auto weaponIconPath = weaponIconPathBuilder.cstring();

        auto&& weaponIconImagePanel = context.panel().clientPanel().template as<ImagePanel>();
        if (shouldUpdateImagePanel(weaponIconImagePanel, weaponIconPath))
            weaponIconImagePanel.setImageSvg(weaponIconPath, 24);
    }

private:
    [[nodiscard]] bool shouldUpdateImagePanel(auto&& imagePanel, const char* newImagePath) const noexcept
    {
        return imagePanel.getImagePath() != newImagePath;
    }

    Context context;
};
