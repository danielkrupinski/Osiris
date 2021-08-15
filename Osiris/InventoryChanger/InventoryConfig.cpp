#include "Inventory.h"
#include "InventoryConfig.h"

constexpr auto CONFIG_VERSION = 3;

json InventoryChanger::toJson() noexcept
{
    json j;

    j["Version"] = CONFIG_VERSION;

    for (auto& items = j["Items"]; const auto & item : Inventory::get()) {
        if (item.isDeleted())
            continue;

        json itemConfig;

        const auto& gameItem = item.get();
        itemConfig["Weapon ID"] = gameItem.weaponID;

        switch (gameItem.type) {
        case StaticData::Type::Sticker: {
            const auto& staticData = StaticData::paintKits()[gameItem.dataIndex];
            itemConfig["Sticker ID"] = staticData.id;
            break;
        }
        case StaticData::Type::Glove: {
            const auto& staticData = StaticData::paintKits()[gameItem.dataIndex];
            itemConfig["Paint Kit"] = staticData.id;

            const auto& dynamicData = Inventory::dynamicGloveData(item.getDynamicDataIndex());

            itemConfig["Wear"] = dynamicData.wear;
            itemConfig["Seed"] = dynamicData.seed;
            break;
        }
        case StaticData::Type::Skin: {
            const auto& staticData = StaticData::paintKits()[gameItem.dataIndex];
            itemConfig["Paint Kit"] = staticData.id;

            const auto& dynamicData = Inventory::dynamicSkinData(item.getDynamicDataIndex());

            if (dynamicData.tournamentID != 0)
                itemConfig["Tournament ID"] = dynamicData.tournamentID;
            itemConfig["Wear"] = dynamicData.wear;
            itemConfig["Seed"] = dynamicData.seed;
            if (dynamicData.statTrak > -1)
                itemConfig["StatTrak"] = dynamicData.statTrak;

            if (!dynamicData.nameTag.empty())
                itemConfig["Name Tag"] = dynamicData.nameTag;

            auto& stickers = itemConfig["Stickers"];
            for (std::size_t i = 0; i < dynamicData.stickers.size(); ++i) {
                const auto& sticker = dynamicData.stickers[i];
                if (sticker.stickerID == 0)
                    continue;

                json stickerConfig;
                stickerConfig["Sticker ID"] = sticker.stickerID;
                stickerConfig["Wear"] = sticker.wear;
                stickerConfig["Slot"] = i;
                stickers.push_back(std::move(stickerConfig));
            }

            if (dynamicData.tournamentStage != TournamentStage{}) {
                itemConfig["Tournament Stage"] = dynamicData.tournamentStage;
                itemConfig["Tournament Team 1"] = dynamicData.tournamentTeam1;
                itemConfig["Tournament Team 2"] = dynamicData.tournamentTeam2;
                itemConfig["Tournament Player"] = dynamicData.proPlayer;
            }
            break;
        }
        case StaticData::Type::Music: {
            const auto& staticData = StaticData::paintKits()[gameItem.dataIndex];
            itemConfig["Music ID"] = staticData.id;
            if (const auto& dynamicData = Inventory::dynamicMusicData(item.getDynamicDataIndex()); dynamicData.statTrak > -1)
                itemConfig["StatTrak"] = dynamicData.statTrak;
            break;
        }
        case StaticData::Type::Patch: {
            const auto& staticData = StaticData::paintKits()[gameItem.dataIndex];
            itemConfig["Patch ID"] = staticData.id;
            break;
        }
        case StaticData::Type::Graffiti: {
            const auto& staticData = StaticData::paintKits()[gameItem.dataIndex];
            itemConfig["Graffiti ID"] = staticData.id;
            break;
        }
        case StaticData::Type::SealedGraffiti: {
            const auto& staticData = StaticData::paintKits()[gameItem.dataIndex];
            itemConfig["Graffiti ID"] = staticData.id;
            break;
        }
        case StaticData::Type::Agent: {
            const auto& dynamicData = Inventory::dynamicAgentData(item.getDynamicDataIndex());
            auto& stickers = itemConfig["Patches"];
            for (std::size_t i = 0; i < dynamicData.patches.size(); ++i) {
                const auto& patch = dynamicData.patches[i];
                if (patch.patchID == 0)
                    continue;

                json patchConfig;
                patchConfig["Patch ID"] = patch.patchID;
                patchConfig["Slot"] = i;
                stickers.push_back(std::move(patchConfig));
            }
            break;
        }
        case StaticData::Type::ServiceMedal: {
            if (const auto& dynamicData = Inventory::dynamicServiceMedalData(item.getDynamicDataIndex()); dynamicData.issueDateTimestamp != 0)
                itemConfig["Issue Date Timestamp"] = dynamicData.issueDateTimestamp;
            break;
        }
        case StaticData::Type::Case: {
            if (StaticData::cases()[gameItem.dataIndex].isSouvenirPackage()) {
                if (const auto& dynamicData = Inventory::dynamicSouvenirPackageData(item.getDynamicDataIndex()); dynamicData.tournamentStage != TournamentStage{}) {
                    itemConfig["Tournament Stage"] = dynamicData.tournamentStage;
                    itemConfig["Tournament Team 1"] = dynamicData.tournamentTeam1;
                    itemConfig["Tournament Team 2"] = dynamicData.tournamentTeam2;
                    itemConfig["Tournament Player"] = dynamicData.proPlayer;
                }
            }
        }
        default:
            break;
        }

        items.push_back(std::move(itemConfig));
    }

    if (const auto localInventory = memory->inventoryManager->getLocalInventory()) {
        auto& equipment = j["Equipment"];
        for (std::size_t i = 0; i < 57; ++i) {
            json slot;

            if (const auto itemCT = localInventory->getItemInLoadout(Team::CT, static_cast<int>(i))) {
                if (const auto soc = memory->getSOCData(itemCT); soc && Inventory::getItem(soc->itemID))
                    slot["CT"] = Inventory::getItemIndex(soc->itemID);
            }

            if (const auto itemTT = localInventory->getItemInLoadout(Team::TT, static_cast<int>(i))) {
                if (const auto soc = memory->getSOCData(itemTT); soc && Inventory::getItem(soc->itemID))
                    slot["TT"] = Inventory::getItemIndex(soc->itemID);
            }

            if (const auto itemNOTEAM = localInventory->getItemInLoadout(Team::None, static_cast<int>(i))) {
                if (const auto soc = memory->getSOCData(itemNOTEAM); soc && Inventory::getItem(soc->itemID))
                    slot["NOTEAM"] = Inventory::getItemIndex(soc->itemID);
            }

            if (!slot.empty()) {
                slot["Slot"] = i;
                equipment.push_back(std::move(slot));
            }
        }
    }

    return j;
}

[[nodiscard]] auto loadSkinStickersFromJson(const json& j) noexcept
{
    std::array<StickerConfig, 5> skinStickers;

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

[[nodiscard]] std::size_t loadDynamicSkinDataFromJson(const json& j) noexcept
{
    DynamicSkinData dynamicData;

    if (j.contains("Tournament ID")) {
        if (const auto& tournamentID = j["Tournament ID"]; tournamentID.is_number_unsigned())
            dynamicData.tournamentID = tournamentID;
    }

    if (j.contains("Wear")) {
        if (const auto& wear = j["Wear"]; wear.is_number_float())
            dynamicData.wear = wear;
    }

    if (j.contains("Seed")) {
        if (const auto& seed = j["Seed"]; seed.is_number_integer())
            dynamicData.seed = seed;
    }

    if (j.contains("StatTrak")) {
        if (const auto& statTrak = j["StatTrak"]; statTrak.is_number_integer())
            dynamicData.statTrak = statTrak;
    }

    if (j.contains("Name Tag")) {
        if (const auto& nameTag = j["Name Tag"]; nameTag.is_string())
            dynamicData.nameTag = nameTag;
    }

    if (j.contains("Tournament Stage")) {
        if (const auto& tournamentStage = j["Tournament Stage"]; tournamentStage.is_number_unsigned())
            dynamicData.tournamentStage = tournamentStage;
    }

    if (j.contains("Tournament Team 1")) {
        if (const auto& tournamentTeam1 = j["Tournament Team 1"]; tournamentTeam1.is_number_unsigned())
            dynamicData.tournamentTeam1 = tournamentTeam1;
    }

    if (j.contains("Tournament Team 2")) {
        if (const auto& tournamentTeam2 = j["Tournament Team 2"]; tournamentTeam2.is_number_unsigned())
            dynamicData.tournamentTeam2 = tournamentTeam2;
    }

    if (j.contains("Tournament Player")) {
        if (const auto& tournamentPlayer = j["Tournament Player"]; tournamentPlayer.is_number_unsigned())
            dynamicData.proPlayer = tournamentPlayer;
    }

    dynamicData.stickers = loadSkinStickersFromJson(j);
    return Inventory::emplaceDynamicData(std::move(dynamicData));
}

[[nodiscard]] std::size_t loadDynamicGloveDataFromJson(const json& j) noexcept
{
    DynamicGloveData dynamicData;

    if (j.contains("Wear")) {
        if (const auto& wear = j["Wear"]; wear.is_number_float())
            dynamicData.wear = wear;
    }

    if (j.contains("Seed")) {
        if (const auto& seed = j["Seed"]; seed.is_number_integer())
            dynamicData.seed = seed;
    }

    return Inventory::emplaceDynamicData(std::move(dynamicData));
}

[[nodiscard]] std::size_t loadDynamicMusicDataFromJson(const json& j) noexcept
{
    DynamicMusicData dynamicData;

    if (j.contains("StatTrak")) {
        if (const auto& statTrak = j["StatTrak"]; statTrak.is_number_integer() && statTrak > -1)
            dynamicData.statTrak = statTrak;
    }

    return Inventory::emplaceDynamicData(std::move(dynamicData));
}

[[nodiscard]] auto loadAgentPatchesFromJson(const json& j) noexcept
{
    std::array<PatchConfig, 5> agentPatches;

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

[[nodiscard]] std::size_t loadDynamicAgentDataFromJson(const json& j) noexcept
{
    DynamicAgentData dynamicData;
    dynamicData.patches = loadAgentPatchesFromJson(j);
    return Inventory::emplaceDynamicData(std::move(dynamicData));
}

[[nodiscard]] std::size_t loadDynamicServiceMedalDataFromJson(const json& j) noexcept
{
    DynamicServiceMedalData dynamicData;

    if (j.contains("Issue Date Timestamp")) {
        if (const auto& issueDateTimestamp = j["Issue Date Timestamp"]; issueDateTimestamp.is_number_unsigned())
            dynamicData.issueDateTimestamp = issueDateTimestamp;
    }

    return Inventory::emplaceDynamicData(std::move(dynamicData));
}

[[nodiscard]] std::size_t loadDynamicSouvenirPackageDataFromJson(const json& j) noexcept
{
    DynamicSouvenirPackageData dynamicData;

    if (j.contains("Tournament Stage")) {
        if (const auto& tournamentStage = j["Tournament Stage"]; tournamentStage.is_number_unsigned())
            dynamicData.tournamentStage = tournamentStage;
    }

    if (j.contains("Tournament Team 1")) {
        if (const auto& tournamentTeam1 = j["Tournament Team 1"]; tournamentTeam1.is_number_unsigned())
            dynamicData.tournamentTeam1 = tournamentTeam1;
    }

    if (j.contains("Tournament Team 2")) {
        if (const auto& tournamentTeam2 = j["Tournament Team 2"]; tournamentTeam2.is_number_unsigned())
            dynamicData.tournamentTeam2 = tournamentTeam2;
    }

    if (j.contains("Tournament Player")) {
        if (const auto& tournamentPlayer = j["Tournament Player"]; tournamentPlayer.is_number_unsigned())
            dynamicData.proPlayer = tournamentPlayer;
    }

    return Inventory::emplaceDynamicData(std::move(dynamicData));
}

void loadEquipmentFromJson(const json& j) noexcept
{
    if (!j.contains("Equipment"))
        return;

    const auto& equipment = j["Equipment"];
    if (!equipment.is_array())
        return;

    for (const auto& equipped : equipment) {
        if (!equipped.contains("Slot"))
            continue;

        const auto& slot = equipped["Slot"];
        if (!slot.is_number_integer())
            continue;

        if (equipped.contains("CT")) {
            if (const auto& ct = equipped["CT"]; ct.is_number_integer())
                Inventory::equipItem(Team::CT, slot, ct);
        }

        if (equipped.contains("TT")) {
            if (const auto& tt = equipped["TT"]; tt.is_number_integer())
                Inventory::equipItem(Team::TT, slot, tt);
        }

        if (equipped.contains("NOTEAM")) {
            if (const auto& noteam = equipped["NOTEAM"]; noteam.is_number_integer())
                Inventory::equipItem(Team::None, slot, noteam);
        }
    }
}

void InventoryChanger::fromJson(const json& j) noexcept
{
    if (!j.contains("Items"))
        return;

    const auto& items = j["Items"];
    if (!items.is_array())
        return;

    for (const auto& jsonItem : items) {
        if (jsonItem.empty() || !jsonItem.is_object())
            continue;

        if (!jsonItem.contains("Weapon ID") || !jsonItem["Weapon ID"].is_number_integer())
            continue;

        const WeaponId weaponID = jsonItem["Weapon ID"];
        int paintKit = 0;

        if (jsonItem.contains("Paint Kit") && jsonItem["Paint Kit"].is_number_integer())
            paintKit = jsonItem["Paint Kit"];
        else if (jsonItem.contains("Sticker ID") && jsonItem["Sticker ID"].is_number_integer())
            paintKit = jsonItem["Sticker ID"];
        else if (jsonItem.contains("Music ID") && jsonItem["Music ID"].is_number_integer())
            paintKit = jsonItem["Music ID"];
        else if (jsonItem.contains("Patch ID") && jsonItem["Patch ID"].is_number_integer())
            paintKit = jsonItem["Patch ID"];
        else if (jsonItem.contains("Graffiti ID") && jsonItem["Graffiti ID"].is_number_integer())
            paintKit = jsonItem["Graffiti ID"];

        const auto itemIndex = StaticData::getItemIndex(weaponID, paintKit);
        if (itemIndex == StaticData::InvalidItemIdx)
            continue;

        const auto& item = StaticData::gameItems()[itemIndex];
        auto dynamicDataIdx = Inventory::InvalidDynamicDataIdx;

        if (item.isSkin()) {
            dynamicDataIdx = loadDynamicSkinDataFromJson(jsonItem);
        } else if (item.isGlove()) {
            dynamicDataIdx = loadDynamicGloveDataFromJson(jsonItem);
        } else if (item.isMusic()) {
            dynamicDataIdx = loadDynamicMusicDataFromJson(jsonItem);
        } else if (item.isAgent()) {
            dynamicDataIdx = loadDynamicAgentDataFromJson(jsonItem);
        } else if (item.isServiceMedal()) {
            dynamicDataIdx = loadDynamicServiceMedalDataFromJson(jsonItem);
        } else if (item.isCase() && StaticData::cases()[item.dataIndex].isSouvenirPackage()) {
            dynamicDataIdx = loadDynamicSouvenirPackageDataFromJson(jsonItem);
        }

        Inventory::addItemAcknowledged(itemIndex, dynamicDataIdx);
    }

    loadEquipmentFromJson(j);
}

void InventoryChanger::resetConfig() noexcept
{
    Inventory::clear();
}
