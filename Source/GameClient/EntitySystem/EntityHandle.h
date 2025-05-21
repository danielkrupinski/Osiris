#pragma once

#include <CS2/Classes/EntitySystem/CEntityHandle.h>
#include <GameClient/EntitySystem/EntitySystem.h>
#include <Utils/Lvalue.h>

template <typename HookContext>
class EntityHandle {
public:
    EntityHandle(HookContext& hookContext, cs2::CEntityHandle& handle)
        : hookContext{hookContext}
        , handle{handle}
    {
    }

    [[nodiscard]] decltype(auto) getOrInit(auto&& onGet, auto&& init)
    {
        if (auto&& entity = hookContext.template make<EntitySystem>().getEntityFromHandle2(handle)) {
            onGet();
            return utils::lvalue<decltype(entity)>(entity);
        }

        handle = init();
        auto&& entity = hookContext.template make<EntitySystem>().getEntityFromHandle2(handle);
        if (entity)
            onGet();
        return utils::lvalue<decltype(entity)>(entity);
    }

private:
    HookContext& hookContext;
    cs2::CEntityHandle& handle;
};
