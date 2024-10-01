#pragma once

#include <CS2/Classes/CCSPlayer_HostageServices.h>
#include "EntitySystem.h"

template <typename HookContext>
class HostageServices {
public:
    HostageServices(HookContext& hookContext, cs2::CCSPlayer_HostageServices* hostageServices) noexcept
        : hookContext{hookContext}
        , hostageServices{hostageServices}
    {
    }

    [[nodiscard]] bool hasCarriedHostage() const noexcept
    {
        return hookContext.template make<EntitySystem>().getEntityFromHandle(deps().offsetToCarriedHostage.of(hostageServices).valueOr(cs2::CEntityHandle{cs2::INVALID_EHANDLE_INDEX})) != nullptr;
    }

private:
    [[nodiscard]] const auto& deps() const noexcept
    {
        return hookContext.gameDependencies().hostageServicesDeps;
    }

    HookContext& hookContext;
    cs2::CCSPlayer_HostageServices* hostageServices;
};
