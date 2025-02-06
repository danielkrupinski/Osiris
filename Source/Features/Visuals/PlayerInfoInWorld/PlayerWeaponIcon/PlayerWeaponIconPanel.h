#pragma once

#include <utility>

#include "PlayerWeaponIconPanelContext.h"

template <typename HookContext, typename Context = PlayerWeaponIconPanelContext<HookContext>>
class PlayerWeaponIconPanel {
public:
    template <typename... Args>
    explicit PlayerWeaponIconPanel(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void update(auto&& playerPawn) const noexcept
    {
        const auto bombIconPanelVisibility = context.bombIconPanel().update(playerPawn);
        context.activeWeaponIconPanel().update(playerPawn, bombIconPanelVisibility);
    }

private:
    Context context;
};
