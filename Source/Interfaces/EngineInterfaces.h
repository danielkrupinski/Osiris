#pragma once

#include "InterfaceFinderWithLog.h"
#include <Platform/RetSpoofInvoker.h>

#include <SDK/Engine.h>
#include <SDK/GameEvent.h>
#include <SDK/ModelInfo.h>

namespace csgo::pod { struct ModelRender; }
namespace csgo::pod { struct EngineSound; }

class EngineInterfaces {
public:
    explicit EngineInterfaces(InterfaceFinderWithLog engineInterfaceFinder, RetSpoofInvoker retSpoofInvoker)
        : retSpoofInvoker{ retSpoofInvoker },
        engine{ static_cast<csgo::pod::Engine*>(engineInterfaceFinder("VEngineClient014")) },
        engineTrace_{ static_cast<csgo::pod::EngineTrace*>(engineInterfaceFinder("EngineTraceClient004")) },
        gameEventManager{ static_cast<csgo::pod::GameEventManager*>(engineInterfaceFinder("GAMEEVENTSMANAGER002")) },
        modelInfo{ static_cast<csgo::pod::ModelInfo*>(engineInterfaceFinder("VModelInfoClient004")) },
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
        return ModelInfo::from(retSpoofInvoker, modelInfo);
    }

    [[nodiscard]] auto engineTrace() const noexcept
    {
        return EngineTrace::from(retSpoofInvoker, engineTrace_);
    }

private:
    RetSpoofInvoker retSpoofInvoker;
    csgo::pod::Engine* engine;
    csgo::pod::GameEventManager* gameEventManager;
    csgo::pod::ModelInfo* modelInfo;
    csgo::pod::EngineTrace* engineTrace_;
public:
    csgo::pod::ModelRender* modelRender;
    csgo::pod::EngineSound* sound;
};
