#pragma once

#include <CS2/Classes/Entities/C_CSWeaponBase.h>
#include <CS2/Classes/CCSWeaponBaseVData.h>

template <typename HookContext>
class BaseWeapon {
public:
    BaseWeapon(HookContext& hookContext, cs2::C_CSWeaponBase* baseWeapon) noexcept
        : hookContext{hookContext}
        , baseWeapon{baseWeapon}
    {
    }

    template <template <typename...> typename EntityType>
    [[nodiscard]] bool is() const noexcept
    {
        return baseEntity().template is<EntityType>();
    }

    [[nodiscard]] decltype(auto) baseEntity() const noexcept
    {
        return hookContext.template make<BaseEntity>(baseWeapon);
    }

    [[nodiscard]] auto getName() const noexcept
    {
        const auto vData = static_cast<cs2::CCSWeaponBaseVData*>(hookContext.template make<BaseEntity>(baseWeapon).vData().valueOr(nullptr));
        return hookContext.clientPatternSearchResults().template get<OffsetToWeaponName>().of(vData).valueOr(nullptr);
    }

    [[nodiscard]] auto clipAmmo() const noexcept
    {
        return hookContext.clientPatternSearchResults().template get<OffsetToClipAmmo>().of(baseWeapon).toOptional();
    }

private:
    HookContext& hookContext;
    cs2::C_CSWeaponBase* baseWeapon;
};
