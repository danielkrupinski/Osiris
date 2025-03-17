#pragma once

#include <utility>

#include <CS2/Classes/Color.h>
#include <CS2/Constants/ColorConstants.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoInWorldState.h>
#include <GameClient/Panorama/PanoramaLabel.h>
#include <Utils/StringBuilder.h>

#include "PlayerHealthPanelContext.h"

template <typename HookContext, typename Context = PlayerHealthPanelContext<HookContext>>
class PlayerHealthPanel {
public:
    template <typename... Args>
    explicit PlayerHealthPanel(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void update(auto&& playerPawn) const noexcept
    {
        if (!context.config().template getVariable<PlayerInfoInWorldPlayerHealthEnabled>()) {
            context.panel().setVisible(false);
            return;
        }

        context.panel().setVisible(true);

        auto&& healthValuePanel = context.panel().children()[1];

        if (const auto healthTextColor = getColor(playerPawn); context.cache().playerHealthTextColor(healthTextColor))
            healthValuePanel.setColor(healthTextColor);

        if (const auto playerHealth = playerPawn.health().valueOr(0); context.cache().playerHealth(playerHealth))
            healthValuePanel.clientPanel().template as<PanoramaLabel>().setText(StringBuilderStorage<10>{}.builder().put(playerHealth).cstring());
    }

private:
    [[nodiscard]] cs2::Color getColor(auto&& playerPawn) const noexcept
    {
        if (context.config().template getVariable<PlayerInfoInWorldPlayerHealthColorMode>() == PlayerHealthTextColor::HealthBased)
            return playerPawn.healthColor().value_or(cs2::kColorWhite);
        return cs2::kColorWhite;
    }

    Context context;
};
