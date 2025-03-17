#pragma once

#include <utility>

template <typename Context>
class WeaponOutlineGlowCondition {
public:
    template <typename... Args>
    explicit WeaponOutlineGlowCondition(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] bool shouldRun() const noexcept
    {
        return context.config().template getVariable<WeaponOutlineGlowEnabled>();
    }

    [[nodiscard]] bool shouldGlowWeapon(auto&& weapon) const noexcept
    {
        return !weapon.hasOwner().valueOr(true);
    }

private:
    Context context;
};
