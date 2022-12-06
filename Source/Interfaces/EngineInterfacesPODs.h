#pragma once

#include "InterfaceFinderWithLog.h"

namespace csgo::pod { struct Engine; }
namespace csgo::pod { struct EngineTrace; }
namespace csgo::pod { struct GameEventManager; }
namespace csgo::pod { struct ModelInfo; }
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
