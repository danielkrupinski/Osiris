#pragma once

#include <CS2/Classes/Entities/C_BaseEntity.h>
#include <CS2/Classes/EntitySystem/CEntityIdentity.h>

template <typename HookContext>
class BaseEntity;

template <typename HookContext>
class EntityIdentity {
public:
    EntityIdentity(HookContext& hookContext, cs2::CEntityIdentity* identity) noexcept
        : hookContext{hookContext}
        , identity{identity}
    {
    }

    template <typename RawEntityType>
    [[nodiscard]] bool is() const noexcept
    {
        if (identity)
            return hookContext.entityClassifier().template entityIs<RawEntityType>(identity->entityClass);
        return false;
    }

    [[nodiscard]] decltype(auto) classify() const noexcept
    {
        return hookContext.entityClassifier().classifyEntity(identity ? identity->entityClass : nullptr);
    }

    [[nodiscard]] decltype(auto) entity() const noexcept
    {
        return hookContext.template make<BaseEntity>(identity ? static_cast<cs2::C_BaseEntity*>(identity->entity) : nullptr);
    }

private:
    HookContext& hookContext;
    cs2::CEntityIdentity* identity;
};
