#pragma once

#include "InterfaceFinderWithLog.h"
#include <Platform/RetSpoofInvoker.h>

#include <SDK/Engine.h>
#include <SDK/GameEvent.h>
#include <SDK/ModelInfo.h>

namespace csgo::pod { struct ModelRender; }
namespace csgo::pod { struct EngineSound; }

struct EngineInterfacesPODs {
    csgo::pod::Engine* engine;
    csgo::pod::EngineTrace* engineTrace;
    csgo::pod::GameEventManager* gameEventManager;
    csgo::pod::ModelInfo* modelInfo;
    csgo::pod::ModelRender* modelRender;
    csgo::pod::EngineSound* sound;
};

[[nodiscard]] inline EngineInterfacesPODs createEngineInterfacesPODs(InterfaceFinderWithLog engineInterfaceFinder)
{
    return EngineInterfacesPODs{
        .engine = static_cast<csgo::pod::Engine*>(engineInterfaceFinder("VEngineClient014")),
        .engineTrace = static_cast<csgo::pod::EngineTrace*>(engineInterfaceFinder("EngineTraceClient004")),
        .gameEventManager = static_cast<csgo::pod::GameEventManager*>(engineInterfaceFinder("GAMEEVENTSMANAGER002")),
        .modelInfo = static_cast<csgo::pod::ModelInfo*>(engineInterfaceFinder("VModelInfoClient004")),
        .modelRender = static_cast<csgo::pod::ModelRender*>(engineInterfaceFinder("VEngineModel016")),
        .sound = static_cast<csgo::pod::EngineSound*>(engineInterfaceFinder("IEngineSoundClient003"))
    };
}

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
