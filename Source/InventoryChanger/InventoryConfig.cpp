#include <nlohmann/json.hpp>

#include "InventoryConfig.h"

#include "GameItems/Lookup.h"
#include "Inventory/Item.h"
#include "Inventory/Structs.h"
#include "Inventory/StructsFromJson.h"

#include "Backend/BackendSimulator.h"

#include "Backend/Response/ResponseHandler.h"
#include "GameIntegration/Inventory.h"

#include "InventoryChanger.h"
#include "WeaponNames.h"

constexpr auto CONFIG_VERSION = 4;

[[nodiscard]] json toJson(const inventory_changer::inventory::Skin& skin)
{
    json j;

    if (skin.tournamentID != csgo::Tournament{})
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
        if (sticker.stickerID == csgo::StickerId::Default)
            continue;

        json stickerConfig;
        stickerConfig["Sticker ID"] = sticker.stickerID;
        if (sticker.wear != 0.0f)
            stickerConfig["Wear"] = sticker.wear;
        stickerConfig["Slot"] = i;
        stickers.push_back(std::move(stickerConfig));
    }

    if (skin.tournamentStage != csgo::TournamentStage{}) {
        j["Tournament Stage"] = skin.tournamentStage;
        j["Tournament Team 1"] = skin.tournamentTeam1;
        j["Tournament Team 2"] = skin.tournamentTeam2;
        j["Tournament Player"] = skin.proPlayer;
    }

    return j;
}

[[nodiscard]] json toJson(const inventory_changer::backend::PickEm& pickEm)
{
    json picks;

    for (const auto& [position, pick] : pickEm.getPicks()) {
        picks.push_back(json{ { "Tournament", position.tournament },
                              { "Group", position.group },
                              { "Index", position.indexInGroup },
                              { "Team", pick } });
    }

    return json{ { "Picks", std::move(picks) } };
}

namespace inventory_changer
{

[[nodiscard]] const game_items::Item* gameItemFromJson(const game_items::Lookup& lookup, const json& j)
{
    if (const auto stickerID = j.find("Sticker ID"); stickerID != j.end() && stickerID->is_number_integer())
        return lookup.findSticker(stickerID->get<csgo::StickerId>());
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

[[nodiscard]] inventory::Item::VariantProperties itemFromJson(const game_items::Storage& gameItemStorage, const game_items::Item& gameItem, const json& j)
{
    if (gameItem.isSkin())
        return inventory::skinFromJson(j);
    if (gameItem.isGloves())
        return inventory::glovesFromJson(j);
    if (gameItem.isMusic())
        return inventory::musicFromJson(j);
    if (gameItem.isAgent())
        return inventory::agentFromJson(j);
    if (gameItem.isServiceMedal())
        return inventory::serviceMedalFromJson(j);
    if (gameItem.isCrate() && gameItemStorage.isSouvenirPackage(gameItem))
        return inventory::souvenirPackageFromJson(j);
    if (gameItem.isGraffiti())
        return inventory::graffitiFromJson(j);
    if (gameItem.isStorageUnit())
        return inventory::storageUnitFromJson(j);
    return {};
}

}

struct EquippedState {
    bool ct = false;
    bool tt = false;
    bool noTeam = false;
};

[[nodiscard]] EquippedState equippedFromJson(const json& j)
{
    EquippedState state;

    if (const auto equipped = j.find("Equipped"); equipped != j.end() && equipped->is_array()) {
        for (auto& team : *equipped) {
            if (!team.is_string())
                continue;

            const auto teamString = team.get<std::string>();
            if (teamString == "CT") {
                state.ct = true;
            } else if (teamString == "TT") {
                state.tt = true;
            } else if (teamString == "NOTEAM") {
                state.noTeam = true;
            }
        }
    }
    return state;
}

[[nodiscard]] std::uint8_t equippedSlotFromJson(const json& j)
{
    if (const auto equippedSlot = j.find("Equipped Slot"); equippedSlot != j.end() && equippedSlot->is_number_integer())
        return equippedSlot->get<std::uint8_t>();
    return static_cast<std::uint8_t>(-1);
}

[[nodiscard]] std::optional<inventory_changer::backend::PickEm::PickPosition> pickPositionFromJson(const json& j)
{
    const auto tournament = j.find("Tournament");
    if (tournament == j.end() || !tournament->is_number_unsigned())
        return {};

    const auto group = j.find("Group");
    if (group == j.end() || !group->is_number_unsigned())
        return {};

    const auto indexInGroup = j.find("Index");
    if (indexInGroup == j.end() || !indexInGroup->is_number_unsigned())
        return {};

    return inventory_changer::backend::PickEm::PickPosition{ static_cast<csgo::Tournament>(tournament->get<std::uint8_t>()), group->get<std::uint16_t>(), indexInGroup->get<std::uint8_t>() };
}

[[nodiscard]] csgo::TournamentTeam pickTeamFromJson(const json& j)
{
    if (const auto team = j.find("Team"); team != j.end() && team->is_number_unsigned())
        return team->get<csgo::TournamentTeam>();
    return csgo::TournamentTeam::None;
}

void pickEmFromJson(const json& j, inventory_changer::backend::BackendSimulator& backend)
{
    const auto pickEm = j.find("Pick'Em");
    if (pickEm == j.end() || !pickEm->is_object())
        return;

    const auto picks = pickEm->find("Picks");
    if (picks == pickEm->end() || !picks->is_array())
        return;

    for (const auto& pick : *picks) {
        const auto position = pickPositionFromJson(pick);
        if (!position.has_value())
            continue;

        const auto team = pickTeamFromJson(pick);
        if (team == csgo::TournamentTeam::None)
            continue;

        backend.getPickEmHandler().pickSticker(*position, team);
    }

}

[[nodiscard]] bool isInXRayScanner(const inventory_changer::backend::XRayScanner& xrayScanner, inventory_changer::backend::ItemIterator item) noexcept
{
    const auto items = xrayScanner.getItems();
    return items.has_value() && (items->crate == item || items->reward == item);
}

namespace inventory_changer
{

void stickerToJson(const game_items::Item& sticker, const game_items::Storage& gameItemStorage, json& j)
{
    j["Sticker ID"] = gameItemStorage.getStickerKit(sticker).id;
}

void glovesToJson(const inventory::Item& gloves, const game_items::Storage& gameItemStorage, json& j)
{
    const auto& paintKit = gameItemStorage.getPaintKit(gloves.gameItem());
    j["Paint Kit"] = paintKit.id;
    j["Paint Kit Name"] = paintKit.name.forDisplay;

    if (const auto glove = get<inventory::Gloves>(gloves)) {
        j["Wear"] = glove->wear;
        j["Seed"] = glove->seed;
    }
}

void musicToJson(const inventory::Item& item, const game_items::Storage& gameItemStorage, json& j)
{
    j["Music ID"] = gameItemStorage.getMusicKit(item.gameItem()).id;
    if (const auto music = get<inventory::Music>(item); music && music->statTrak > -1)
        j["StatTrak"] = music->statTrak;
}

void agentToJson(const inventory::Item& item, json& j)
{
    const auto agent = get<inventory::Agent>(item);
    if (!agent)
        return;

    auto& stickers = j["Patches"];
    for (std::size_t i = 0; i < agent->patches.size(); ++i) {
        const auto& patch = agent->patches[i];
        if (patch.patchID == 0)
            continue;

        json patchConfig;
        patchConfig["Patch ID"] = patch.patchID;
        patchConfig["Slot"] = i;
        stickers.push_back(std::move(patchConfig));
    }
}

void storageUnitToJson(const inventory::Item& item, json& j)
{
    if (const auto storageUnit = get<inventory::StorageUnit>(item); storageUnit && storageUnit->modificationDateTimestamp != 0) {
        j["Modification Date Timestamp"] = storageUnit->modificationDateTimestamp;
        j["Name"] = storageUnit->name;
    }
}

void commonPropertiesToJson(const inventory::Item::CommonProperties& properties, json& j)
{
    if (properties.tradableAfterDate != 0)
        j.emplace("Tradable After Date", properties.tradableAfterDate);
    if (properties.purchasedFromStore)
        j.emplace("Purchased From Store", true);
}

[[nodiscard]] json itemsToJson(const OtherInterfaces& interfaces, const Memory& memory, const backend::BackendSimulator& backend)
{
    const auto& gameItemStorage = backend.getGameItemLookup().getStorage();
    const auto& loadout = backend.getLoadout();
    const auto& inventory = backend.getInventory();
    const auto& storageUnitIDs = backend.getStorageUnitManager().getStorageUnitIDs();
    const auto& xrayScanner = backend.getXRayScanner();

    json items;
    for (auto itemIt = inventory.begin(); itemIt != inventory.end(); ++itemIt) {
        if (isInXRayScanner(xrayScanner, itemIt))
            continue;

        json itemConfig;

        if (const auto slotCT = loadout.getItemEquippedSlotCT(itemIt); slotCT.has_value()) {
            itemConfig["Equipped Slot"] = *slotCT;
            itemConfig["Equipped"].push_back("CT");
        }
        if (const auto slotTT = loadout.getItemEquippedSlotTT(itemIt); slotTT.has_value()) {
            itemConfig["Equipped Slot"] = *slotTT;
            itemConfig["Equipped"].push_back("TT");
        }
        if (const auto slotNoTeam = loadout.getItemEquippedSlotNoTeam(itemIt); slotNoTeam.has_value()) {
            itemConfig["Equipped Slot"] = *slotNoTeam;
            itemConfig["Equipped"].push_back("NOTEAM");
        }

        const auto& item = *itemIt;
        const auto& gameItem = item.gameItem();
        itemConfig["Weapon ID"] = gameItem.getWeaponID();
        itemConfig["Item Name"] = WeaponNames::instance(interfaces, memory).getWeaponName(gameItem.getWeaponID());

        commonPropertiesToJson(item.getProperties().common, itemConfig);

        if (gameItem.isSticker()) {
            stickerToJson(gameItem, gameItemStorage, itemConfig);
        } else if (gameItem.isGloves()) {
            glovesToJson(item, gameItemStorage, itemConfig);
        } else if (gameItem.isSkin()) {
            const auto& staticData = gameItemStorage.getPaintKit(gameItem);
            itemConfig["Paint Kit"] = staticData.id;
            itemConfig["Paint Kit Name"] = staticData.name.forDisplay;

            if (const auto skin = get<inventory::Skin>(item))
                itemConfig.update(::toJson(*skin));

        } else if (gameItem.isMusic()) {
            musicToJson(item, gameItemStorage, itemConfig);
        } else if (gameItem.isPatch()) {
            itemConfig["Patch ID"] = gameItemStorage.getPatch(gameItem).id;
        } else if (gameItem.isGraffiti()) {
            itemConfig["Graffiti ID"] = gameItemStorage.getGraffitiKit(gameItem).id;
            if (const auto graffiti = get<inventory::Graffiti>(item); graffiti && graffiti->usesLeft >= 0) {
                itemConfig["Uses Left"] = graffiti->usesLeft;
                itemConfig["Item Name"] = WeaponNames::instance(interfaces, memory).getWeaponName(WeaponId::Graffiti);
            }
        } else if (gameItem.isAgent()) {
            agentToJson(item, itemConfig);
        } else if (gameItem.isServiceMedal()) {
            if (const auto serviceMedal = get<inventory::ServiceMedal>(item); serviceMedal && serviceMedal->issueDateTimestamp != 0)
                itemConfig["Issue Date Timestamp"] = serviceMedal->issueDateTimestamp;
        } else if (gameItem.isCrate()) {
            if (const auto souvenirPackage = get<inventory::SouvenirPackage>(item); souvenirPackage && souvenirPackage->tournamentStage != csgo::TournamentStage{}) {
                itemConfig["Tournament Stage"] = souvenirPackage->tournamentStage;
                itemConfig["Tournament Team 1"] = souvenirPackage->tournamentTeam1;
                itemConfig["Tournament Team 2"] = souvenirPackage->tournamentTeam2;
                itemConfig["Tournament Player"] = souvenirPackage->proPlayer;
            }
        } else if (gameItem.isStorageUnit()) {
            storageUnitToJson(item, itemConfig);
        }

        if (const auto storageUnitID = storageUnitIDs.find(itemIt); storageUnitID != storageUnitIDs.end())
            itemConfig["Storage Unit ID"] = storageUnitID->second;

        items.push_back(std::move(itemConfig));
    }

    return items;
}

}

json inventory_changer::toJson(const OtherInterfaces& interfaces, const Memory& memory, const InventoryChanger& inventoryChanger)
{
    json j;

    j["Version"] = CONFIG_VERSION;

    const auto& backend = inventoryChanger.getBackend();

    j.emplace("Items", itemsToJson(interfaces, memory, backend));
    j.emplace("Pick'Em", ::toJson(backend.getPickEm()));
    return j;
}

std::optional<std::uint32_t> storageUnitIdFromJson(const json& j)
{
    if (const auto it = j.find("Storage Unit ID"); it != j.end() && it->is_number_unsigned())
        return it->get<std::uint32_t>();
    return {};
}

namespace inventory_changer
{

[[nodiscard]] inventory::Item::CommonProperties commonPropertiesFromJson(const json& j)
{
    inventory::Item::CommonProperties properties;

    if (const auto it = j.find("Tradable After Date"); it != j.end() && it->is_number_unsigned())
        properties.tradableAfterDate = it->get<std::uint32_t>();

    if (const auto it = j.find("Purchased From Store"); it != j.end() && it->is_boolean())
        properties.purchasedFromStore = it->get<bool>();

    return properties;
}

}

void inventory_changer::fromJson(const json& j, InventoryChanger& inventoryChanger)
{
    auto& backend = inventoryChanger.getBackend();
    const auto& lookup = backend.getGameItemLookup();

    pickEmFromJson(j, backend);

    if (!j.contains("Items"))
        return;

    const auto& items = j["Items"];
    if (!items.is_array())
        return;

    std::unordered_map<std::uint32_t, inventory_changer::backend::ItemIterator> storageUnits;
    std::vector<std::pair<inventory_changer::backend::ItemIterator, std::uint32_t>> itemsToBindToStorageUnits;

    for (const auto& jsonItem : items) {
        const auto item = gameItemFromJson(lookup, jsonItem);
        if (!item)
            continue;

        const auto itemAdded = backend.getInventoryHandler().addItem(inventory::Item{ *item, { commonPropertiesFromJson(jsonItem), itemFromJson(lookup.getStorage(), *item, jsonItem) } }, false);

        if (const auto storageUnitID = storageUnitIdFromJson(jsonItem); storageUnitID.has_value()) {
            if (!item->isStorageUnit()) {
                itemsToBindToStorageUnits.emplace_back(itemAdded, *storageUnitID);
            } else {
                storageUnits.emplace(*storageUnitID, itemAdded);
            }
        }

        if (const auto equippedSlot = equippedSlotFromJson(jsonItem); equippedSlot != static_cast<std::uint8_t>(-1)) {
            const auto equippedState = equippedFromJson(jsonItem);
            if (equippedState.ct)
                backend.getLoadoutHandler().equipItem(itemAdded, equippedSlot, csgo::Team::CT);
            if (equippedState.tt)
                backend.getLoadoutHandler().equipItem(itemAdded, equippedSlot, csgo::Team::TT);
            if (equippedState.noTeam)
                backend.getLoadoutHandler().equipItem(itemAdded, equippedSlot, csgo::Team::None);
        }

    }

    for (auto [item, storageUnitID] : itemsToBindToStorageUnits) {
        if (const auto storageUnit = storageUnits.find(storageUnitID); storageUnit != storageUnits.end()) {
            backend.getStorageUnitHandler().bindItemToStorageUnit(item, storageUnit->second);
        }
    }
}
