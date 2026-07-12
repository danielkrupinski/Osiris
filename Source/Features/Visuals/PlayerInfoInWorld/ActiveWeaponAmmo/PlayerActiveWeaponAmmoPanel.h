#pragma once

#include <utility>

#include "PlayerActiveWeaponAmmoPanelContext.h"
#include <GameClient/Panorama/PanoramaLabel.h>
#include <Utils/StringBuilder.h>

template <typename HookContext, typename Context = PlayerActiveWeaponAmmoPanelContext<HookContext>>
class PlayerActiveWeaponAmmoPanel {
public:
    template <typename... Args>
    explicit PlayerActiveWeaponAmmoPanel(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void update(auto&& playerPawn) const noexcept
    {
        if (!context.shouldShowOn(playerPawn)) {
            context.panel().setVisible(false);
            return;
        }

        context.panel().setVisible(true);
        const auto ammo = playerPawn.getActiveWeapon().clipAmmo().valueOr(-1);
        if (context.cache().activeWeaponAmmo(ammo))
            context.panel().children()[0].clientPanel().template as<PanoramaLabel>().setText(StringBuilder{std::span{context.cache().activeWeaponAmmoText, 10}}.put(ammo).cstring());
    }

private:
    Context context;
};
