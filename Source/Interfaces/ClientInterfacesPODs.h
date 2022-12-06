#pragma once

#include "InterfaceFinderWithLog.h"

namespace csgo::pod { struct Client; }
namespace csgo::pod { struct EntityList; }
namespace csgo::pod { struct GameMovement; }
namespace csgo::pod { struct Prediction; }

struct ClientInterfacesPODs {
    csgo::pod::Client* client;
    csgo::pod::EntityList* entityList;
    csgo::pod::GameMovement* gameMovement;
    csgo::pod::Prediction* prediction;
};

[[nodiscard]] inline ClientInterfacesPODs createClientInterfacesPODs(InterfaceFinderWithLog clientInterfaceFinder)
{
    return ClientInterfacesPODs{
        .client = static_cast<csgo::pod::Client*>(clientInterfaceFinder("VClient018")),
        .entityList = static_cast<csgo::pod::EntityList*>(clientInterfaceFinder("VClientEntityList003")),
        .gameMovement = static_cast<csgo::pod::GameMovement*>(clientInterfaceFinder("GameMovement001")),
        .prediction = static_cast<csgo::pod::Prediction*>(clientInterfaceFinder("VClientPrediction001"))
    };
}
