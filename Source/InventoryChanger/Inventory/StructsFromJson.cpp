#include <nlohmann/json.hpp>

#include "Structs.h"
#include "StructsFromJson.h"

namespace inventory_changer::inventory
{

Gloves glovesFromJson(const json& j)
{
    Gloves glove;

    if (const auto wear = j.find("Wear"); wear != j.end() && wear->is_number_float())
        glove.wear = wear->get<float>();

    if (const auto seed = j.find("Seed"); seed != j.end() && seed->is_number_integer())
        glove.seed = seed->get<int>();

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

        skinStickers[slot].stickerID = static_cast<csgo::StickerId>(stickerID);

        if (sticker.contains("Wear") && sticker["Wear"].is_number_float())
            skinStickers[slot].wear = sticker["Wear"];
    }

    return skinStickers;
}

Skin skinFromJson(const json& j)
{
    Skin skin;

    if (j.contains("Tournament ID")) {
        if (const auto& tournamentID = j["Tournament ID"]; tournamentID.is_number_unsigned())
            skin.tournamentID = tournamentID;
    }

    if (j.contains("Wear")) {
        if (const auto& wear = j["Wear"]; wear.is_number_float())
            skin.wear = wear;
    }

    if (j.contains("Seed")) {
        if (const auto& seed = j["Seed"]; seed.is_number_integer())
            skin.seed = seed;
    }

    if (j.contains("StatTrak")) {
        if (const auto& statTrak = j["StatTrak"]; statTrak.is_number_integer())
            skin.statTrak = statTrak;
    }

    if (j.contains("Name Tag")) {
        if (const auto& nameTag = j["Name Tag"]; nameTag.is_string())
            skin.nameTag = nameTag;
    }

    if (j.contains("Tournament Stage")) {
        if (const auto& tournamentStage = j["Tournament Stage"]; tournamentStage.is_number_unsigned())
            skin.tournamentStage = tournamentStage;
    }

    if (j.contains("Tournament Team 1")) {
        if (const auto& tournamentTeam1 = j["Tournament Team 1"]; tournamentTeam1.is_number_unsigned())
            skin.tournamentTeam1 = tournamentTeam1;
    }

    if (j.contains("Tournament Team 2")) {
        if (const auto& tournamentTeam2 = j["Tournament Team 2"]; tournamentTeam2.is_number_unsigned())
            skin.tournamentTeam2 = tournamentTeam2;
    }

    if (j.contains("Tournament Player")) {
        if (const auto& tournamentPlayer = j["Tournament Player"]; tournamentPlayer.is_number_unsigned())
            skin.proPlayer = tournamentPlayer;
    }

    skin.stickers = inventory::skinStickersFromJson(j);
    return skin;
}

std::array<Agent::Patch, 5> agentPatchesFromJson(const json& j)
{
    std::array<Agent::Patch, 5> agentPatches;

    if (!j.contains("Patches"))
        return agentPatches;

    const auto& patches = j["Patches"];
    if (!patches.is_array())
        return agentPatches;

    for (const auto& patch : patches) {
        if (!patch.is_object())
            continue;

        if (!patch.contains("Patch ID") || !patch["Patch ID"].is_number_integer())
            continue;

        if (!patch.contains("Slot") || !patch["Slot"].is_number_integer())
            continue;

        const int patchID = patch["Patch ID"];
        const std::size_t slot = patch["Slot"];

        if (patchID != 0 && slot < agentPatches.size())
            agentPatches[slot].patchID = patchID;
    }

    return agentPatches;
}

Agent agentFromJson(const json& j)
{
    Agent dynamicData;
    dynamicData.patches = agentPatchesFromJson(j);
    return dynamicData;
}

StorageUnit storageUnitFromJson(const json& j)
{
    StorageUnit storageUnit;

    if (const auto modificationDateTimestamp = j.find("Modification Date Timestamp"); modificationDateTimestamp != j.end() && modificationDateTimestamp->is_number_integer())
        storageUnit.modificationDateTimestamp = modificationDateTimestamp->get<std::uint32_t>();
    
    if (const auto name = j.find("Name"); name != j.end() && name->is_string())
        storageUnit.name = name->get<std::string>();
    
    return storageUnit;
}

}
