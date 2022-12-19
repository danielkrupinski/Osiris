#pragma once

#include <CSGO/Constants/InterfaceNames.h>
#include "InterfaceFinderWithLog.h"

namespace csgo { struct ClientPOD; }
namespace csgo { struct EntityListPOD; }
namespace csgo { struct GameMovementPOD; }
namespace csgo { struct PredictionPOD; }

struct ClientInterfacesPODs {
    csgo::ClientPOD* client;
    csgo::EntityListPOD* entityList;
    csgo::GameMovementPOD* gameMovement;
    csgo::PredictionPOD* prediction;
};

[[nodiscard]] inline ClientInterfacesPODs createClientInterfacesPODs(InterfaceFinderWithLog clientInterfaceFinder)
{
    using namespace csgo::i;
    return ClientInterfacesPODs{
        .client = static_cast<csgo::ClientPOD*>(clientInterfaceFinder(VClient)),
        .entityList = static_cast<csgo::EntityListPOD*>(clientInterfaceFinder(VClientEntityList)),
        .gameMovement = static_cast<csgo::GameMovementPOD*>(clientInterfaceFinder(GameMovement)),
        .prediction = static_cast<csgo::PredictionPOD*>(clientInterfaceFinder(VClientPrediction))
    };
}
