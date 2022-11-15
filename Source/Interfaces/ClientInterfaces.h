#pragma once

#include "InterfaceFinderWithLog.h"
#include <SDK/Client.h>
#include <SDK/EntityList.h>
#include <SDK/GameMovement.h>
#include <SDK/Prediction.h>

class ClientInterfaces {
public:
    explicit ClientInterfaces(InterfaceFinderWithLog clientInterfaceFinder, RetSpoofInvoker retSpoofInvoker)
        : retSpoofInvoker{ retSpoofInvoker },
        client{ static_cast<csgo::pod::Client*>(clientInterfaceFinder("VClient018")) },
        entityList{ static_cast<csgo::pod::EntityList*>(clientInterfaceFinder("VClientEntityList003")) },
        gameMovement{ std::uintptr_t(clientInterfaceFinder("GameMovement001")) },
        prediction{ std::uintptr_t(clientInterfaceFinder("VClientPrediction001")) }
    {
    }

    [[nodiscard]] auto getClient() const noexcept
    {
        return Client::from(retSpoofInvoker, client);
    }

    [[nodiscard]] auto getEntityList() const noexcept
    {
        return EntityList::from(retSpoofInvoker, entityList);
    }

    [[nodiscard]] auto getGameMovement() const noexcept
    {
        return GameMovement{ retSpoofInvoker, gameMovement };
    }

    [[nodiscard]] auto getPrediction() const noexcept
    {
        return Prediction{ retSpoofInvoker, prediction };
    }

private:
    RetSpoofInvoker retSpoofInvoker;
    csgo::pod::Client* client;
    csgo::pod::EntityList* entityList;
    std::uintptr_t gameMovement;
    std::uintptr_t prediction;
};
