#pragma once

#include <CS2/Classes/CCSPlayer_WeaponServices.h>
#include <CS2/Classes/Entities/C_CSWeaponBase.h>
#include "BaseWeapon.h"
#include "PlayerWeapons.h"

template <typename HookContext>
class WeaponServices {
public:
    WeaponServices(HookContext& hookContext, cs2::CCSPlayer_WeaponServices* weaponServices) noexcept
        : hookContext{hookContext}
        , weaponServices{weaponServices}
    {
    }

    [[nodiscard]] decltype(auto) weapons() const noexcept
    {
        return hookContext.template make<PlayerWeapons>(deps().offsetToWeapons.of(weaponServices).get());
    }

    [[nodiscard]] auto getActiveWeapon() const noexcept
    {
        return hookContext.template make<BaseWeapon>(static_cast<cs2::C_CSWeaponBase*>(hookContext.template make<EntitySystem>().getEntityFromHandle(deps().offsetToActiveWeapon.of(weaponServices).valueOr(cs2::CEntityHandle{cs2::INVALID_EHANDLE_INDEX}))));
    }

private:
    [[nodiscard]] const auto& deps() const noexcept
    {
        return hookContext.gameDependencies().weaponServicesDeps;
    }

    HookContext& hookContext;
    cs2::CCSPlayer_WeaponServices* weaponServices;
};
