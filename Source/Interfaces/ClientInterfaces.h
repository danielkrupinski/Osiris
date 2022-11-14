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
        client{ std::uintptr_t(clientInterfaceFinder("VClient018")) },
        entityList{ std::uintptr_t(clientInterfaceFinder("VClientEntityList003")) },
        gameMovement{ std::uintptr_t(clientInterfaceFinder("GameMovement001")) },
        prediction{ std::uintptr_t(clientInterfaceFinder("VClientPrediction001")) }
    {
    }

    [[nodiscard]] auto getClient() const noexcept
    {
        return Client{ retSpoofInvoker, client };
    }

    [[nodiscard]] std::uintptr_t getClientAddress() const noexcept
    {
        return client;
    }

    [[nodiscard]] auto getEntityList() const noexcept
    {
        return EntityList{ retSpoofInvoker, entityList };
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
    std::uintptr_t client;
    std::uintptr_t entityList;
    std::uintptr_t gameMovement;
    std::uintptr_t prediction;
};
