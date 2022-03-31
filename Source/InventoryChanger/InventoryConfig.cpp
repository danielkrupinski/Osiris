#include "Inventory.h"
#include "InventoryConfig.h"

#include "GameItems/Lookup.h"
#include "Inventory/Item.h"
#include "Inventory/Structs.h"
#include "Inventory/StructsFromJson.h"

#include "Backend/BackendSimulator.h"

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
        itemConfig["Weapon ID"] = gameItem.getWeaponID();
        itemConfig["Item Name"] = StaticData::getWeaponName(gameItem.getWeaponID());

        if (gameItem.isSticker()) {
            itemConfig["Sticker ID"] = StaticData::lookup().getStorage().getStickerKit(gameItem).id;
        } else if (gameItem.isGloves()) {
            const auto& staticData = StaticData::lookup().getStorage().getPaintKit(gameItem);
            itemConfig["Paint Kit"] = staticData.id;
            itemConfig["Paint Kit Name"] = staticData.name.forDisplay;

            const auto& dynamicData = Inventory::dynamicGloveData(item);

            itemConfig["Wear"] = dynamicData.wear;
            itemConfig["Seed"] = dynamicData.seed;
        } else if (gameItem.isSkin()) {
            const auto& staticData = StaticData::lookup().getStorage().getPaintKit(gameItem);
            itemConfig["Paint Kit"] = staticData.id;
            itemConfig["Paint Kit Name"] = staticData.name.forDisplay;

            const auto& dynamicData = Inventory::dynamicSkinData(item);

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
        } else if (gameItem.isMusic()) {
            itemConfig["Music ID"] = StaticData::lookup().getStorage().getMusicKit(gameItem).id;
            if (const auto& dynamicData = Inventory::dynamicMusicData(item); dynamicData.statTrak > -1)
                itemConfig["StatTrak"] = dynamicData.statTrak;
        } else if (gameItem.isPatch()) {
            itemConfig["Patch ID"] = StaticData::lookup().getStorage().getPatch(gameItem).id;
        } else if (gameItem.isGraffiti()) {
            itemConfig["Graffiti ID"] = StaticData::lookup().getStorage().getGraffitiKit(gameItem).id;
            if (const auto& dynamicData = Inventory::dynamicGraffitiData(item); dynamicData.usesLeft >= 0) {
                itemConfig["Uses Left"] = dynamicData.usesLeft;
                itemConfig["Item Name"] = StaticData::getWeaponName(WeaponId::Graffiti);
            }
        } else if (gameItem.isAgent()) {
            const auto& dynamicData = Inventory::dynamicAgentData(item);
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
        } else if (gameItem.isServiceMedal()) {
            if (const auto& dynamicData = Inventory::dynamicServiceMedalData(item); dynamicData.issueDateTimestamp != 0)
                itemConfig["Issue Date Timestamp"] = dynamicData.issueDateTimestamp;
        } else if (gameItem.isCase()) {
            if (StaticData::isSouvenirPackage(gameItem)) {
                if (const auto& dynamicData = Inventory::dynamicSouvenirPackageData(item); dynamicData.tournamentStage != TournamentStage{}) {
                    itemConfig["Tournament Stage"] = dynamicData.tournamentStage;
                    itemConfig["Tournament Team 1"] = dynamicData.tournamentTeam1;
                    itemConfig["Tournament Team 2"] = dynamicData.tournamentTeam2;
                    itemConfig["Tournament Player"] = dynamicData.proPlayer;
                }
            }
        }

        items.push_back(std::move(itemConfig));
    }

    if (const auto localInventory = memory->inventoryManager->getLocalInventory()) {
        auto& equipment = j["Equipment"];
        for (std::uint8_t i = 0; i < 57; ++i) {
            json slot;

            using namespace inventory_changer::backend;
            if (const auto ct = BackendSimulator::instance().getLoadout().getItemInSlotCT(i); ct.has_value())
                slot["CT"] = *ct;
            if (const auto tt = BackendSimulator::instance().getLoadout().getItemInSlotTT(i); tt.has_value())
                slot["TT"] = *tt;
            if (const auto noTeam = BackendSimulator::instance().getLoadout().getItemInSlotNoTeam(i); noTeam.has_value())
                slot["NOTEAM"] = *noTeam;

            /*
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
            */
            if (!slot.empty()) {
                slot["Slot"] = i;
                equipment.push_back(std::move(slot));
            }
        }
    }

    return j;
}

[[nodiscard]] auto loadAgentPatchesFromJson(const json& j) noexcept
{
    std::array<inventory::Agent::Patch, 5> agentPatches;

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

[[nodiscard]] inventory::Agent loadDynamicAgentDataFromJson(const json& j) noexcept
{
    inventory::Agent dynamicData;
    dynamicData.patches = loadAgentPatchesFromJson(j);
    return dynamicData;
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
            if (const auto& ct = equipped["CT"]; ct.is_number_integer()) {
                Inventory::equipItem(Team::CT, slot, ct);
                inventory_changer::backend::BackendSimulator::instance().equipItemCT(ct, slot);
            }
        }

        if (equipped.contains("TT")) {
            if (const auto& tt = equipped["TT"]; tt.is_number_integer()) {
                Inventory::equipItem(Team::TT, slot, tt);
                inventory_changer::backend::BackendSimulator::instance().equipItemTT(tt, slot);
            }
        }

        if (equipped.contains("NOTEAM")) {
            if (const auto& noteam = equipped["NOTEAM"]; noteam.is_number_integer()) {
                Inventory::equipItem(Team::None, slot, noteam);
                inventory_changer::backend::BackendSimulator::instance().equipItemNoTeam(noteam, slot);
            }
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
        std::optional<std::reference_wrapper<const game_items::Item>> itemOptional;
        auto dynamicDataIdx = Inventory::InvalidDynamicDataIdx;

        if (jsonItem.contains("Paint Kit") && jsonItem["Paint Kit"].is_number_integer())
            itemOptional = StaticData::lookup().findItem(weaponID, jsonItem["Paint Kit"]);
        else if (jsonItem.contains("Sticker ID") && jsonItem["Sticker ID"].is_number_integer())
            itemOptional = StaticData::lookup().findSticker(jsonItem["Sticker ID"]);
        else if (jsonItem.contains("Music ID") && jsonItem["Music ID"].is_number_integer())
            itemOptional = StaticData::lookup().findMusic(jsonItem["Music ID"]);
        else if (jsonItem.contains("Patch ID") && jsonItem["Patch ID"].is_number_integer())
            itemOptional = StaticData::lookup().findPatch(jsonItem["Patch ID"]);
        else if (jsonItem.contains("Graffiti ID") && jsonItem["Graffiti ID"].is_number_integer()) {
            itemOptional = StaticData::lookup().findGraffiti(jsonItem["Graffiti ID"]);
            if (weaponID == WeaponId::Graffiti) {
                if (itemOptional.has_value()) {
                    inventory::Graffiti dynamicData;
                    dynamicData.usesLeft = 50;
                    dynamicDataIdx = Inventory::emplaceDynamicData(std::move(dynamicData));
                }
            }
        } else
            itemOptional = StaticData::lookup().findItem(weaponID);

        if (!itemOptional.has_value())
            continue;

        const auto& item = itemOptional->get();

        if (item.isSkin()) {
            dynamicDataIdx = Inventory::emplaceDynamicData(inventory::skinFromJson(jsonItem));
        } else if (item.isGloves()) {
            dynamicDataIdx = Inventory::emplaceDynamicData(inventory::gloveFromJson(jsonItem));
        } else if (item.isMusic()) {
            dynamicDataIdx = Inventory::emplaceDynamicData(inventory::musicFromJson(jsonItem));
        } else if (item.isAgent()) {
            dynamicDataIdx = Inventory::emplaceDynamicData(loadDynamicAgentDataFromJson(jsonItem));
        } else if (item.isServiceMedal()) {
            dynamicDataIdx = Inventory::emplaceDynamicData(inventory::serviceMedalFromJson(jsonItem));
        } else if (item.isCase() && StaticData::isSouvenirPackage(item)) {
            dynamicDataIdx = Inventory::emplaceDynamicData(inventory::souvenirPackageFromJson(jsonItem));
        } else if (item.isGraffiti() && dynamicDataIdx == Inventory::InvalidDynamicDataIdx) {
            dynamicDataIdx = Inventory::emplaceDynamicData(inventory::graffitiFromJson(jsonItem));
        }

        Inventory::addItemAcknowledged(item, dynamicDataIdx);
    }

    loadEquipmentFromJson(j);
}

void InventoryChanger::resetConfig() noexcept
{
    Inventory::clear();
}
