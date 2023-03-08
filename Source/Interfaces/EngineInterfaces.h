#pragma once

#include "EngineInterfacesPODs.h"
#include <RetSpoof/RetSpoofInvoker.h>

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
        return csgo::Engine::from(retSpoofInvoker, std::get<csgo::EnginePOD*>(pods));
    }

    [[nodiscard]] auto getGameEventManager(csgo::GetEventDescriptor getEventDescriptorFn) const noexcept
    {
        return csgo::GameEventManager::from(retSpoofInvoker, std::get<csgo::GameEventManagerPOD*>(pods), getEventDescriptorFn);
    }

    [[nodiscard]] auto getModelInfo() const noexcept
    {
        return csgo::ModelInfo::from(retSpoofInvoker, std::get<csgo::ModelInfoPOD*>(pods));
    }

    [[nodiscard]] auto engineTrace() const noexcept
    {
        return csgo::EngineTrace::from(retSpoofInvoker, std::get<csgo::EngineTracePOD*>(pods));
    }

    [[nodiscard]] const EngineInterfacesPODs& getPODs() const noexcept
    {
        return pods;
    }

private:
    RetSpoofInvoker retSpoofInvoker;
    EngineInterfacesPODs pods;
};
