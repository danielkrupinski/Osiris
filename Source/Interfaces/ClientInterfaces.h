#pragma once

#include "ClientInterfacesPODs.h"

#include <Platform/RetSpoofInvoker.h>
#include <CSGO/Client.h>
#include <CSGO/EntityList.h>
#include <CSGO/GameMovement.h>
#include <CSGO/Prediction.h>

class ClientInterfaces {
public:
    explicit ClientInterfaces(RetSpoofInvoker retSpoofInvoker, const ClientInterfacesPODs& pods)
        : retSpoofInvoker{ retSpoofInvoker }, pods{ pods }
    {
    }

    [[nodiscard]] auto getClient() const noexcept
    {
        return csgo::Client::from(retSpoofInvoker, pods.client);
    }

    [[nodiscard]] auto getEntityList() const noexcept
    {
        return csgo::EntityList::from(retSpoofInvoker, pods.entityList);
    }

    [[nodiscard]] auto getGameMovement() const noexcept
    {
        return csgo::GameMovement::from(retSpoofInvoker, pods.gameMovement);
    }

    [[nodiscard]] auto getPrediction() const noexcept
    {
        return csgo::Prediction::from(retSpoofInvoker, pods.prediction);
    }

private:
    RetSpoofInvoker retSpoofInvoker;
    const ClientInterfacesPODs& pods;
};
