#pragma once

#include <CS2/Classes/CCSPlayer_HostageServices.h>
#include <CS2/Classes/EntitySystem/CEntityHandle.h>
#include <CS2/Constants/EntityHandle.h>
#include <GameClient/EntitySystem/EntitySystem.h>
#include <MemoryPatterns/PatternTypes/HostageServicesPatternTypes.h>

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
        return hookContext.template make<EntitySystem>().getEntityFromHandle(hookContext.clientPatternSearchResults().template get<OffsetToCarriedHostage>().of(hostageServices).valueOr(cs2::CEntityHandle{cs2::INVALID_EHANDLE_INDEX})) != nullptr;
    }

private:
    HookContext& hookContext;
    cs2::CCSPlayer_HostageServices* hostageServices;
};
