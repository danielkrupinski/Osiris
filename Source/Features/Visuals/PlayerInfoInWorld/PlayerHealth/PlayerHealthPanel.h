#pragma once

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
        if (!context.state().showPlayerHealth) {
            context.panel().setVisible(false);
            return;
        }

        context.panel().setVisible(true);

        auto&& healthValuePanel = context.panel().children()[1];
        healthValuePanel.setColor(getColor(playerPawn));
        healthValuePanel.clientPanel().template as<PanoramaLabel>().setText(StringBuilderStorage<10>{}.builder().put(playerPawn.health().valueOr(0)).cstring());
    }

private:
    [[nodiscard]] cs2::Color getColor(auto&& playerPawn) const noexcept
    {
        if (context.state().playerHealthTextColor == PlayerHealthTextColor::HealthBased)
            return playerPawn.healthColor().value_or(cs2::kColorWhite);
        return cs2::kColorWhite;
    }

    Context context;
};
