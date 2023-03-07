#pragma once

#include "ClientInterfacesPODs.h"

#include <RetSpoof/RetSpoofInvoker.h>
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
        return csgo::Client::from(retSpoofInvoker, std::get<csgo::ClientPOD*>(pods));
    }

    [[nodiscard]] auto getEntityList() const noexcept
    {
        return csgo::EntityList::from(retSpoofInvoker, std::get<csgo::EntityListPOD*>(pods));
    }

    [[nodiscard]] auto getGameMovement() const noexcept
    {
        return csgo::GameMovement::from(retSpoofInvoker, std::get<csgo::GameMovementPOD*>(pods));
    }

    [[nodiscard]] auto getPrediction() const noexcept
    {
        return csgo::Prediction::from(retSpoofInvoker, std::get<csgo::PredictionPOD*>(pods));
    }

private:
    RetSpoofInvoker retSpoofInvoker;
    ClientInterfacesPODs pods;
};
