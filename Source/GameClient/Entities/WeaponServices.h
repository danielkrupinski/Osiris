#pragma once

#include <CS2/Classes/CCSPlayer_WeaponServices.h>
#include <CS2/Classes/Entities/C_CSWeaponBase.h>
#include <CS2/Classes/EntitySystem/CEntityHandle.h>
#include <CS2/Constants/EntityHandle.h>
#include <GameClient/EntitySystem/EntitySystem.h>
#include <MemoryPatterns/PatternTypes/WeaponServicesPatternTypes.h>
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
        return hookContext.template make<PlayerWeapons>(hookContext.clientPatternSearchResults().template get<OffsetToWeapons>().of(weaponServices).get());
    }

    [[nodiscard]] auto getActiveWeapon() const noexcept
    {
        return hookContext.template make<BaseWeapon>(static_cast<cs2::C_CSWeaponBase*>(hookContext.template make<EntitySystem>().getEntityFromHandle(hookContext.clientPatternSearchResults().template get<OffsetToActiveWeapon>().of(weaponServices).valueOr(cs2::CEntityHandle{cs2::INVALID_EHANDLE_INDEX}))));
    }

private:
    HookContext& hookContext;
    cs2::CCSPlayer_WeaponServices* weaponServices;
};
