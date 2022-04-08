#include "InventoryConfig.h"

#include "GameItems/Lookup.h"
#include "Inventory/Item.h"
#include "Inventory/Structs.h"
#include "Inventory/StructsFromJson.h"

#include "Backend/BackendSimulator.h"

constexpr auto CONFIG_VERSION = 3;

[[nodiscard]] json toJson(const inventory::Skin& skin)
{
    json j;

    if (skin.tournamentID != 0)
        j["Tournament ID"] = skin.tournamentID;

    j["Wear"] = skin.wear;
    j["Seed"] = skin.seed;
    if (skin.statTrak > -1)
        j["StatTrak"] = skin.statTrak;

    if (!skin.nameTag.empty())
        j["Name Tag"] = skin.nameTag;

    auto& stickers = j["Stickers"];
    for (std::size_t i = 0; i < skin.stickers.size(); ++i) {
        const auto& sticker = skin.stickers[i];
        if (sticker.stickerID == 0)
            continue;

        json stickerConfig;
        stickerConfig["Sticker ID"] = sticker.stickerID;
        stickerConfig["Wear"] = sticker.wear;
        stickerConfig["Slot"] = i;
        stickers.push_back(std::move(stickerConfig));
    }

    if (skin.tournamentStage != TournamentStage{}) {
        j["Tournament Stage"] = skin.tournamentStage;
        j["Tournament Team 1"] = skin.tournamentTeam1;
        j["Tournament Team 2"] = skin.tournamentTeam2;
        j["Tournament Player"] = skin.proPlayer;
    }

    return j;
}

json InventoryChanger::toJson() noexcept
{
    json j;

    j["Version"] = CONFIG_VERSION;

    const auto& inventory = inventory_changer::backend::BackendSimulator::instance().getInventory();
    for (auto& items = j["Items"]; const auto & item : inventory) {
        json itemConfig;

        const auto& gameItem = item.gameItem();
        itemConfig["Weapon ID"] = gameItem.getWeaponID();
        itemConfig["Item Name"] = StaticData::getWeaponName(gameItem.getWeaponID());

        if (gameItem.isSticker()) {
            itemConfig["Sticker ID"] = StaticData::lookup().getStorage().getStickerKit(gameItem).id;
        } else if (gameItem.isGloves()) {
            const auto& staticData = StaticData::lookup().getStorage().getPaintKit(gameItem);
            itemConfig["Paint Kit"] = staticData.id;
            itemConfig["Paint Kit Name"] = staticData.name.forDisplay;

            const auto& dynamicData = *item.get<inventory::Glove>();

            itemConfig["Wear"] = dynamicData.wear;
            itemConfig["Seed"] = dynamicData.seed;
        } else if (gameItem.isSkin()) {
            const auto& staticData = StaticData::lookup().getStorage().getPaintKit(gameItem);
            itemConfig["Paint Kit"] = staticData.id;
            itemConfig["Paint Kit Name"] = staticData.name.forDisplay;

            if (const auto skin = item.get<inventory::Skin>())
                itemConfig.update(::toJson(*skin));

        } else if (gameItem.isMusic()) {
            itemConfig["Music ID"] = StaticData::lookup().getStorage().getMusicKit(gameItem).id;
            if (const auto& dynamicData = *item.get<inventory::Music>(); dynamicData.statTrak > -1)
                itemConfig["StatTrak"] = dynamicData.statTrak;
        } else if (gameItem.isPatch()) {
            itemConfig["Patch ID"] = StaticData::lookup().getStorage().getPatch(gameItem).id;
        } else if (gameItem.isGraffiti()) {
            itemConfig["Graffiti ID"] = StaticData::lookup().getStorage().getGraffitiKit(gameItem).id;
            if (const auto& dynamicData = *item.get<inventory::Graffiti>(); dynamicData.usesLeft >= 0) {
                itemConfig["Uses Left"] = dynamicData.usesLeft;
                itemConfig["Item Name"] = StaticData::getWeaponName(WeaponId::Graffiti);
            }
        } else if (gameItem.isAgent()) {
            const auto& dynamicData = *item.get<inventory::Agent>();
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
            if (const auto& dynamicData = *item.get<inventory::ServiceMedal>(); dynamicData.issueDateTimestamp != 0)
                itemConfig["Issue Date Timestamp"] = dynamicData.issueDateTimestamp;
        } else if (gameItem.isCase()) {
            if (StaticData::isSouvenirPackage(gameItem)) {
                if (const auto& dynamicData = *item.get<inventory::SouvenirPackage>(); dynamicData.tournamentStage != TournamentStage{}) {
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

            /*
            using namespace inventory_changer::backend;
            if (const auto ct = BackendSimulator::instance().getLoadout().getItemInSlotCT(i); ct.has_value())
                slot["CT"] = *ct;
            if (const auto tt = BackendSimulator::instance().getLoadout().getItemInSlotTT(i); tt.has_value())
                slot["TT"] = *tt;
            if (const auto noTeam = BackendSimulator::instance().getLoadout().getItemInSlotNoTeam(i); noTeam.has_value())
                slot["NOTEAM"] = *noTeam;
            */
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

        /*
        if (equipped.contains("CT")) {
            if (const auto& ct = equipped["CT"]; ct.is_number_integer()) {
                inventory_changer::backend::BackendSimulator::instance().equipItemCT(ct, slot);
            }
        }

        if (equipped.contains("TT")) {
            if (const auto& tt = equipped["TT"]; tt.is_number_integer()) {
                inventory_changer::backend::BackendSimulator::instance().equipItemTT(tt, slot);
            }
        }

        if (equipped.contains("NOTEAM")) {
            if (const auto& noteam = equipped["NOTEAM"]; noteam.is_number_integer()) {
                inventory_changer::backend::BackendSimulator::instance().equipItemNoTeam(noteam, slot);
            }
        }
        */
    }
}

[[nodiscard]] game_items::Lookup::OptionalItemReference gameItemFromJson(const game_items::Lookup& lookup, const json& j)
{
    if (const auto stickerID = j.find("Sticker ID"); stickerID != j.end() && stickerID->is_number_integer())
        return lookup.findSticker(stickerID->get<int>());
    if (const auto musicID = j.find("Music ID"); musicID != j.end() && musicID->is_number_integer())
        return lookup.findMusic(musicID->get<int>());
    if (const auto patchID = j.find("Patch ID"); patchID != j.end() && patchID->is_number_integer())
        return lookup.findPatch(patchID->get<int>());
    if (const auto graffitiID = j.find("Graffiti ID"); graffitiID != j.end() && graffitiID->is_number_integer())
        return lookup.findGraffiti(graffitiID->get<int>());

    const auto weaponID = j.find("Weapon ID");
    if (weaponID == j.end() || !weaponID->is_number_integer())
        return {};

    if (const auto paintKit = j.find("Paint Kit"); paintKit != j.end() && paintKit->is_number_integer())
        return lookup.findItem(weaponID->get<WeaponId>(), paintKit->get<int>());

    return lookup.findItem(weaponID->get<WeaponId>());
}

[[nodiscard]] inventory::StructWrapper itemFromJson(const game_items::Item& gameItem, const json& j)
{
    if (gameItem.isSkin())
        return inventory::skinFromJson(j);
    if (gameItem.isGloves())
        return inventory::gloveFromJson(j);
    if (gameItem.isMusic())
        return inventory::musicFromJson(j);
    if (gameItem.isAgent())
        return loadDynamicAgentDataFromJson(j);
    if (gameItem.isServiceMedal())
        return inventory::serviceMedalFromJson(j);
    if (gameItem.isCase() && StaticData::isSouvenirPackage(gameItem))
        return inventory::souvenirPackageFromJson(j);
    if (gameItem.isGraffiti())
        return inventory::graffitiFromJson(j);
    return {};
}

void InventoryChanger::fromJson(const json& j) noexcept
{
    if (!j.contains("Items"))
        return;

    const auto& items = j["Items"];
    if (!items.is_array())
        return;

    for (const auto& jsonItem : items) {
        std::optional<std::reference_wrapper<const game_items::Item>> itemOptional = gameItemFromJson(StaticData::lookup(), jsonItem);
        if (!itemOptional.has_value())
            continue;

        const game_items::Item& item = itemOptional->get();
        inventory_changer::backend::BackendSimulator::instance().addItem(inventory::Item_v2{ item, itemFromJson(item, jsonItem) });
    }

    loadEquipmentFromJson(j);
}

void InventoryChanger::resetConfig() noexcept
{
    inventory_changer::backend::BackendSimulator::instance().clearInventory();
}
