#pragma once

#include "InterfaceFinderWithLog.h"

namespace csgo { struct EnginePOD; }
namespace csgo { struct EngineTracePOD; }
namespace csgo { struct GameEventManagerPOD; }
namespace csgo { struct ModelInfoPOD; }
namespace csgo { struct ModelRenderPOD; }
namespace csgo { struct EngineSoundPOD; }

struct EngineInterfacesPODs {
    csgo::EnginePOD* engine;
    csgo::EngineTracePOD* engineTrace;
    csgo::GameEventManagerPOD* gameEventManager;
    csgo::ModelInfoPOD* modelInfo;
    csgo::ModelRenderPOD* modelRender;
    csgo::EngineSoundPOD* sound;
};

[[nodiscard]] inline EngineInterfacesPODs createEngineInterfacesPODs(InterfaceFinderWithLog engineInterfaceFinder)
{
    return EngineInterfacesPODs{
        .engine = static_cast<csgo::EnginePOD*>(engineInterfaceFinder("VEngineClient014")),
        .engineTrace = static_cast<csgo::EngineTracePOD*>(engineInterfaceFinder("EngineTraceClient004")),
        .gameEventManager = static_cast<csgo::GameEventManagerPOD*>(engineInterfaceFinder("GAMEEVENTSMANAGER002")),
        .modelInfo = static_cast<csgo::ModelInfoPOD*>(engineInterfaceFinder("VModelInfoClient004")),
        .modelRender = static_cast<csgo::ModelRenderPOD*>(engineInterfaceFinder("VEngineModel016")),
        .sound = static_cast<csgo::EngineSoundPOD*>(engineInterfaceFinder("IEngineSoundClient003"))
    };
}
