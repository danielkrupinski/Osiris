#pragma once

#include <tuple>

#include <CSGO/Constants/InterfaceNames.h>
#include "InterfaceFinderWithLog.h"

namespace csgo
{
    struct ClientPOD;
    struct EntityListPOD;
    struct GameMovementPOD;
    struct PredictionPOD;
}

using ClientInterfacesPODs = std::tuple<
    csgo::ClientPOD*,
    csgo::EntityListPOD*,
    csgo::GameMovementPOD*,
    csgo::PredictionPOD*
>;

[[nodiscard]] inline ClientInterfacesPODs createClientInterfacesPODs(InterfaceFinderWithLog clientInterfaceFinder)
{
    using namespace csgo::i;
    return ClientInterfacesPODs{
        static_cast<csgo::ClientPOD*>(clientInterfaceFinder(VClient)),
        static_cast<csgo::EntityListPOD*>(clientInterfaceFinder(VClientEntityList)),
        static_cast<csgo::GameMovementPOD*>(clientInterfaceFinder(GameMovement)),
        static_cast<csgo::PredictionPOD*>(clientInterfaceFinder(VClientPrediction))
    };
}
