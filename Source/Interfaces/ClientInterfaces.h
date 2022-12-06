#pragma once

#include "ClientInterfacesPODs.h"

#include <Platform/RetSpoofInvoker.h>
#include <SDK/Client.h>
#include <SDK/EntityList.h>
#include <SDK/GameMovement.h>
#include <SDK/Prediction.h>

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
