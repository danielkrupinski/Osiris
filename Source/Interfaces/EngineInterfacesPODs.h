#pragma once

#include <tuple>

#include <CSGO/Constants/InterfaceNames.h>
#include "InterfaceFinderWithLog.h"

namespace csgo
{
    struct EnginePOD;
    struct EngineSoundPOD;
    struct EngineTracePOD;
    struct GameEventManagerPOD;
    struct ModelInfoPOD;
    struct ModelRenderPOD;
}

using EngineInterfacesPODs = std::tuple<
    csgo::EnginePOD*,
    csgo::EngineTracePOD*,
    csgo::GameEventManagerPOD*,
    csgo::ModelInfoPOD*,
    csgo::ModelRenderPOD*,
    csgo::EngineSoundPOD*
>;

[[nodiscard]] inline EngineInterfacesPODs createEngineInterfacesPODs(InterfaceFinderWithLog engineInterfaceFinder)
{
    using namespace csgo::i;
    return EngineInterfacesPODs{
        static_cast<csgo::EnginePOD*>(engineInterfaceFinder(VEngineClient)),
        static_cast<csgo::EngineTracePOD*>(engineInterfaceFinder(EngineTraceClient)),
        static_cast<csgo::GameEventManagerPOD*>(engineInterfaceFinder(GAMEEVENTSMANAGER)),
        static_cast<csgo::ModelInfoPOD*>(engineInterfaceFinder(VModelInfoClient)),
        static_cast<csgo::ModelRenderPOD*>(engineInterfaceFinder(VEngineModel)),
        static_cast<csgo::EngineSoundPOD*>(engineInterfaceFinder(IEngineSoundClient))
    };
}
