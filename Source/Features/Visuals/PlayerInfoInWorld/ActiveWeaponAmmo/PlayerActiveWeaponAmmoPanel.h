#pragma once

#include "PlayerActiveWeaponAmmoPanelContext.h"
#include <GameClasses/PanoramaLabel.h>

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
        context.panel().children()[0].clientPanel().template as<PanoramaLabel>().setText(StringBuilderStorage<10>{}.builder().put(ammo).cstring());
    }

private:
    Context context;
};
