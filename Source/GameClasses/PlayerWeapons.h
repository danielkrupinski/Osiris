#pragma once

#include <CS2/Classes/CUtlVector.h>
#include <CS2/Classes/EntitySystem/CEntityHandle.h>

#include "BaseEntity.h"
#include "EntitySystem.h"

class EntityFromHandleFinder;

template <typename HookContext>
class PlayerWeapons {
public:
    PlayerWeapons(HookContext& hookContext, cs2::CUtlVector<cs2::CEntityHandle>* weaponHandles) noexcept
        : hookContext{hookContext}
        , weaponHandles{weaponHandles}
    {
    }

    template <typename WeaponType>
    [[nodiscard]] bool has() const noexcept
    {
        if (!weaponHandles)
            return false;

        for (int i = 0; i < weaponHandles->size; ++i) {
            auto&& weapon = hookContext.template make<BaseEntity>(static_cast<cs2::C_BaseEntity*>(hookContext.template make<EntitySystem>().getEntityFromHandle(weaponHandles->memory[i]))).template cast<WeaponType>();
            if (weapon)
                return true;
        }
        return false;
    }


    template <typename F>
    void forEach(F f) const noexcept
    {
        if (!weaponHandles)
            return;

        for (int i = 0; i < weaponHandles->size; ++i) {
            auto&& weaponEntity = hookContext.template make<BaseEntity>(static_cast<cs2::C_BaseEntity*>(hookContext.template make<EntitySystem>().getEntityFromHandle(weaponHandles->memory[i])));
            if (weaponEntity)
                f(weaponEntity);
        }
    }

private:
    HookContext& hookContext;
    cs2::CUtlVector<cs2::CEntityHandle>* weaponHandles;
};
