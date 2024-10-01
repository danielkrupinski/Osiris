#pragma once

#include "PlayerActiveWeaponIconPanelContext.h"

template <typename HookContext, typename Context = PlayerActiveWeaponIconPanelContext<HookContext>>
class PlayerActiveWeaponIconPanel {
public:
    template <typename... Args>
    explicit PlayerActiveWeaponIconPanel(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void update(auto&& playerPawn) const noexcept
    {
        if (!context.state().showPlayerActiveWeapon) {
            context.panel().setVisible(false);
            return;
        }

        auto weaponName = CString{playerPawn.getActiveWeapon().getName()};
        if (!weaponName.string)
            return;
        weaponName.skipPrefix("weapon_");

        context.panel().setVisible(true);

        StringBuilderStorage<100> weaponIconPathStorage;
        auto weaponIconPathBuilder = weaponIconPathStorage.builder();
        weaponIconPathBuilder.put("s2r://panorama/images/icons/equipment/", weaponName.string, ".svg");
        const auto weaponIconPath = weaponIconPathBuilder.cstring();

        auto&& weaponIconImagePanel = context.panel().clientPanel().template as<PanoramaImagePanel>();
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
