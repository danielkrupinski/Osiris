#pragma once

#include <utility>

template <typename Context>
class PlayerOutlineGlowCondition {
public:
    template <typename... Args>
    explicit PlayerOutlineGlowCondition(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] bool shouldRun() const noexcept
    {
        return context.state().enabled;
    }

    [[nodiscard]] bool shouldGlowPlayer(auto&& playerPawn) const noexcept
    {
        return context.isLocalPlayerAlive()
            && playerPawn.isAlive().value_or(true)
            && playerPawn.health().greaterThan(0).valueOr(true)
            && !playerPawn.isControlledByLocalPlayer()
            && playerPawn.isTTorCT()
            && (!context.state().showOnlyEnemies || playerPawn.isEnemy().value_or(true));
    }

private:
    Context context;
};

template <typename HookContext>
class PlayerOutlineGlowContext;

template <typename HookContext>
PlayerOutlineGlowCondition(HookContext&) -> PlayerOutlineGlowCondition<PlayerOutlineGlowContext<HookContext>>;
