#pragma once

#include <CS2/Classes/Entities/C_BaseEntity.h>

#include "RenderComponent.h"

template <typename HookContext>
class BaseEntity {
public:
    BaseEntity(HookContext& hookContext, cs2::C_BaseEntity* entity) noexcept
        : hookContext{hookContext}
        , entity{entity}
    {
    }

    [[nodiscard]] decltype(auto) renderComponent() const noexcept
    {
        return hookContext.template make<RenderComponent>(hookContext.gameDependencies().entityDeps.offsetToRenderComponent.of(entity).valueOr(nullptr));
    }

    [[nodiscard]] cs2::CEntityHandle handle() const noexcept
    {
        if (entity)
            return entity->identity->handle;
        return cs2::CEntityHandle{cs2::INVALID_EHANDLE_INDEX};
    }

    operator cs2::C_BaseEntity*() const noexcept
    {
        return entity;
    }

private:
    HookContext& hookContext;
    cs2::C_BaseEntity* entity;
};
