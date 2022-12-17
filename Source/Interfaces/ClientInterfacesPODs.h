#pragma once

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
    return ClientInterfacesPODs{
        .client = static_cast<csgo::ClientPOD*>(clientInterfaceFinder("VClient018")),
        .entityList = static_cast<csgo::EntityListPOD*>(clientInterfaceFinder("VClientEntityList003")),
        .gameMovement = static_cast<csgo::GameMovementPOD*>(clientInterfaceFinder("GameMovement001")),
        .prediction = static_cast<csgo::PredictionPOD*>(clientInterfaceFinder("VClientPrediction001"))
    };
}
