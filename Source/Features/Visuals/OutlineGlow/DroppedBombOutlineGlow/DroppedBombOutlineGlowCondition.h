#pragma once

#include <utility>

template <typename Context>
class DroppedBombOutlineGlowCondition {
public:
    template <typename... Args>
    DroppedBombOutlineGlowCondition(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] bool shouldRun() const noexcept
    {
        return context.config().template getVariable<DroppedBombModelGlowEnabled>();
    }

    [[nodiscard]] bool shouldGlowBomb(auto&& bomb) const noexcept
    {
        return !bomb.hasOwner().valueOr(true);
    }

private:
    Context context;
};
