#pragma once

#include "EngineInterfacesPODs.h"
#include <Platform/RetSpoofInvoker.h>

#include <SDK/Engine.h>
#include <SDK/EngineTrace.h>
#include <SDK/GameEvent.h>
#include <SDK/ModelInfo.h>

class EngineInterfaces {
public:
    explicit EngineInterfaces(RetSpoofInvoker retSpoofInvoker, const EngineInterfacesPODs& pods)
        : retSpoofInvoker{ retSpoofInvoker }, pods{ pods }
    {
    }

    [[nodiscard]] auto getEngine() const noexcept
    {
        return Engine::from(retSpoofInvoker, pods.engine);
    }

    [[nodiscard]] auto getGameEventManager(std::uintptr_t getEventDescriptorFn) const noexcept
    {
        return GameEventManager::from(retSpoofInvoker, pods.gameEventManager, getEventDescriptorFn);
    }

    [[nodiscard]] auto getModelInfo() const noexcept
    {
        return ModelInfo::from(retSpoofInvoker, pods.modelInfo);
    }

    [[nodiscard]] auto engineTrace() const noexcept
    {
        return EngineTrace::from(retSpoofInvoker, pods.engineTrace);
    }

private:
    RetSpoofInvoker retSpoofInvoker;
    const EngineInterfacesPODs& pods;
};
