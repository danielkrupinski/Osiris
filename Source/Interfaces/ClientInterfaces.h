#pragma once

#include "InterfaceFinderWithLog.h"
#include <SDK/Client.h>
#include <SDK/EntityList.h>
#include <SDK/GameMovement.h>
#include <SDK/Prediction.h>

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

class ClientInterfaces {
public:
    explicit ClientInterfaces(RetSpoofInvoker retSpoofInvoker, const ClientInterfacesPODs& pods)
        : retSpoofInvoker{ retSpoofInvoker }, pods{ pods }
    {
    }

    [[nodiscard]] auto getClient() const noexcept
    {
        return Client::from(retSpoofInvoker, pods.client);
    }

    [[nodiscard]] auto getEntityList() const noexcept
    {
        return EntityList::from(retSpoofInvoker, pods.entityList);
    }

    [[nodiscard]] auto getGameMovement() const noexcept
    {
        return GameMovement::from(retSpoofInvoker, pods.gameMovement);
    }

    [[nodiscard]] auto getPrediction() const noexcept
    {
        return Prediction::from(retSpoofInvoker, pods.prediction);
    }

private:
    RetSpoofInvoker retSpoofInvoker;
    const ClientInterfacesPODs& pods;
};
