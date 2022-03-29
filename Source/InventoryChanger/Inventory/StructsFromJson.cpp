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

Music musicFromJson(const json& j)
{
    Music music;

    if (j.contains("StatTrak")) {
        if (const auto& statTrak = j["StatTrak"]; statTrak.is_number_integer() && statTrak > -1)
            music.statTrak = statTrak;
    }

    return music;
}

ServiceMedal serviceMedalFromJson(const json& j)
{
    ServiceMedal serviceMedal;

    if (j.contains("Issue Date Timestamp")) {
        if (const auto& issueDateTimestamp = j["Issue Date Timestamp"]; issueDateTimestamp.is_number_unsigned())
            serviceMedal.issueDateTimestamp = issueDateTimestamp;
    }

    return serviceMedal;
}

Graffiti graffitiFromJson(const json& j)
{
    Graffiti graffiti;

    if (j.contains("Uses Left")) {
        if (const auto& usesLeft = j["Uses Left"]; usesLeft.is_number_integer())
            graffiti.usesLeft = usesLeft;
    }

    return graffiti;
}

std::array<Skin::Sticker, 5> skinStickersFromJson(const json& j)
{
    std::array<Skin::Sticker, 5> skinStickers;

    if (!j.contains("Stickers"))
        return skinStickers;

    const auto& stickers = j["Stickers"];
    if (!stickers.is_array())
        return skinStickers;

    for (const auto& sticker : stickers) {
        if (!sticker.is_object())
            continue;

        if (!sticker.contains("Sticker ID") || !sticker["Sticker ID"].is_number_integer())
            continue;

        if (!sticker.contains("Slot") || !sticker["Slot"].is_number_integer())
            continue;

        const int stickerID = sticker["Sticker ID"];
        if (stickerID == 0)
            continue;

        const std::size_t slot = sticker["Slot"];
        if (slot >= skinStickers.size())
            continue;

        skinStickers[slot].stickerID = stickerID;

        if (sticker.contains("Wear") && sticker["Wear"].is_number_float())
            skinStickers[slot].wear = sticker["Wear"];
    }

    return skinStickers;
}

}
