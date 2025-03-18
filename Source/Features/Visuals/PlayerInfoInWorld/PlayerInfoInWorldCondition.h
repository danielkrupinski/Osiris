#pragma once

template <typename Context>
class PlayerInfoInWorldCondition {
public:
    explicit PlayerInfoInWorldCondition(auto& otherContext) noexcept
        : context{otherContext}
    {
    }

    [[nodiscard]] bool shouldRun() const noexcept
    {
        return context.config().template getVariable<PlayerInfoInWorldEnabled>();
    }

    [[nodiscard]] bool shouldDrawInfoOnPawn(auto&& playerPawn) const noexcept
    {
        return playerPawn.isAlive().value_or(true)
            && playerPawn.health().greaterThan(0).valueOr(true)
            && !playerPawn.isControlledByLocalPlayer()
            && playerPawn.isTTorCT()
            && (!context.config().template getVariable<PlayerInfoInWorldOnlyEnemies>() || playerPawn.isEnemy().value_or(true));
    }

private:
    Context context;
};
