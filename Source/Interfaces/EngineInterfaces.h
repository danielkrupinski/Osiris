#pragma once

#include "InterfaceFinderWithLog.h"
#include <Platform/RetSpoofInvoker.h>

#include <SDK/Engine.h>
#include <SDK/GameEvent.h>

namespace csgo::pod { struct ModelRender; }
namespace csgo::pod { struct EngineSound; }

class EngineInterfaces {
public:
    explicit EngineInterfaces(InterfaceFinderWithLog engineInterfaceFinder, RetSpoofInvoker retSpoofInvoker)
        : retSpoofInvoker{ retSpoofInvoker },
        engine{ static_cast<csgo::pod::Engine*>(engineInterfaceFinder("VEngineClient014")) },
        engineTrace{ retSpoofInvoker, std::uintptr_t(engineInterfaceFinder("EngineTraceClient004")) },
        gameEventManager{ static_cast<csgo::pod::GameEventManager*>(engineInterfaceFinder("GAMEEVENTSMANAGER002")) },
        modelInfo{ std::uintptr_t(engineInterfaceFinder("VModelInfoClient004")) },
        modelRender{ static_cast<csgo::pod::ModelRender*>(engineInterfaceFinder("VEngineModel016")) },
        sound{ static_cast<csgo::pod::EngineSound*>(engineInterfaceFinder("IEngineSoundClient003")) }
    {
    }

    [[nodiscard]] auto getEngine() const noexcept
    {
        return Engine::from(retSpoofInvoker, engine);
    }

    [[nodiscard]] auto getGameEventManager(std::uintptr_t getEventDescriptorFn) const noexcept
    {
        return GameEventManager::from(retSpoofInvoker, gameEventManager, getEventDescriptorFn);
    }

    [[nodiscard]] auto getModelInfo() const noexcept
    {
        return ModelInfo{ retSpoofInvoker, modelInfo };
    }

private:
    RetSpoofInvoker retSpoofInvoker;
    csgo::pod::Engine* engine;
    csgo::pod::GameEventManager* gameEventManager;
    std::uintptr_t modelInfo;
public:
    EngineTrace engineTrace;
    csgo::pod::ModelRender* modelRender;
    csgo::pod::EngineSound* sound;
};
