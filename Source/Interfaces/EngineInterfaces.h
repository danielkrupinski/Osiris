#pragma once

#include "EngineInterfacesPODs.h"
#include <Platform/RetSpoofInvoker.h>

#include <CSGO/Engine.h>
#include <CSGO/EngineTrace.h>
#include <CSGO/GameEvent.h>
#include <CSGO/ModelInfo.h>

class EngineInterfaces {
public:
    explicit EngineInterfaces(RetSpoofInvoker retSpoofInvoker, const EngineInterfacesPODs& pods)
        : retSpoofInvoker{ retSpoofInvoker }, pods{ pods }
    {
    }

    [[nodiscard]] auto getEngine() const noexcept
    {
        return csgo::Engine::from(retSpoofInvoker, pods.engine);
    }

    [[nodiscard]] auto getGameEventManager(csgo::GetEventDescriptor getEventDescriptorFn) const noexcept
    {
        return csgo::GameEventManager::from(retSpoofInvoker, pods.gameEventManager, getEventDescriptorFn);
    }

    [[nodiscard]] auto getModelInfo() const noexcept
    {
        return csgo::ModelInfo::from(retSpoofInvoker, pods.modelInfo);
    }

    [[nodiscard]] auto engineTrace() const noexcept
    {
        return csgo::EngineTrace::from(retSpoofInvoker, pods.engineTrace);
    }

    [[nodiscard]] const EngineInterfacesPODs& getPODs() const noexcept
    {
        return pods;
    }

private:
    RetSpoofInvoker retSpoofInvoker;
    const EngineInterfacesPODs& pods;
};
