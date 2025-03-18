#pragma once

#include <utility>

template <typename Context>
class TickingBombOutlineGlowCondition {
public:
    template <typename... Args>
    TickingBombOutlineGlowCondition(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] bool shouldRun() const noexcept
    {
        return context.config().template getVariable<TickingBombModelGlowEnabled>();
    }

    [[nodiscard]] bool shouldGlowPlantedBomb(auto&& plantedBomb) const noexcept
    {
        return plantedBomb.isTicking().valueOr(true);
    }

private:
    Context context;
};
