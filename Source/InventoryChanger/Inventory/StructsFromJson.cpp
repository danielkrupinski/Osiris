#include <nlohmann/json.hpp>

#include "Structs.h"
#include "StructsFromJson.h"

namespace inventory
{

Glove gloveFromJson(const json& j)
{
    Glove glove;

    if (j.contains("Wear")) {
        if (const auto& wear = j["Wear"]; wear.is_number_float())
            glove.wear = wear;
    }

    if (j.contains("Seed")) {
        if (const auto& seed = j["Seed"]; seed.is_number_integer())
            glove.seed = seed;
    }

    return glove;
}

SouvenirPackage souvenirPackageFromJson(const json& j)
{
    SouvenirPackage souvenirPackage;

    if (j.contains("Tournament Stage")) {
        if (const auto& tournamentStage = j["Tournament Stage"]; tournamentStage.is_number_unsigned())
            souvenirPackage.tournamentStage = tournamentStage;
    }

    if (j.contains("Tournament Team 1")) {
        if (const auto& tournamentTeam1 = j["Tournament Team 1"]; tournamentTeam1.is_number_unsigned())
            souvenirPackage.tournamentTeam1 = tournamentTeam1;
    }

    if (j.contains("Tournament Team 2")) {
        if (const auto& tournamentTeam2 = j["Tournament Team 2"]; tournamentTeam2.is_number_unsigned())
            souvenirPackage.tournamentTeam2 = tournamentTeam2;
    }

    if (j.contains("Tournament Player")) {
        if (const auto& tournamentPlayer = j["Tournament Player"]; tournamentPlayer.is_number_unsigned())
            souvenirPackage.proPlayer = tournamentPlayer;
    }

    return souvenirPackage;
}

}
