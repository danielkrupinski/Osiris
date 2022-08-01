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
        if (sticker.wear != 0.0f)
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

[[nodiscard]] inventory::ItemData itemFromJson(const game_items::Storage& gameItemStorage, const game_items::Item& gameItem, const json& j)
{
    if (gameItem.isSkin())
        return inventory::skinFromJson(j);
    if (gameItem.isGloves())
        return inventory::gloveFromJson(j);
    if (gameItem.isMusic())
        return inventory::musicFromJson(j);
    if (gameItem.isAgent())
        return inventory::agentFromJson(j);
    if (gameItem.isServiceMedal())
        return inventory::serviceMedalFromJson(j);
    if (gameItem.isCase() && gameItemStorage.isSouvenirPackage(gameItem))
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

    return inventory_changer::backend::PickEm::PickPosition{ tournament->get<std::uint8_t>(), group->get<std::uint16_t>(), indexInGroup->get<std::uint8_t>() };
}

[[nodiscard]] TournamentTeam pickTeamFromJson(const json& j)
{
    if (const auto team = j.find("Team"); team != j.end() && team->is_number_unsigned())
        return team->get<TournamentTeam>();
    return TournamentTeam::None;
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
        if (team == TournamentTeam::None)
            continue;

        backend.getRequestor().request<inventory_changer::backend::request::PickStickerPickEm>(*position, team);
    }

}

[[nodiscard]] bool isInXRayScanner(const inventory_changer::backend::XRayScanner& xrayScanner, inventory_changer::backend::ItemIterator item) noexcept
{
    const auto items = xrayScanner.getItems();
    return items.has_value() && (items->crate == item || items->reward == item);
}

json inventory_changer::toJson(const InventoryChanger& inventoryChanger)
{
    json j;

    j["Version"] = CONFIG_VERSION;

    const auto& backend = inventoryChanger.getBackend();
    const auto& gameItemStorage = backend.getGameItemLookup().getStorage();
    const auto& loadout = backend.getLoadout();
    const auto& inventory = backend.getInventory();
    const auto& storageUnitIDs = backend.getStorageUnitManager().getStorageUnitIDs();
    const auto& xrayScanner = backend.getXRayScanner();

    auto& items = j["Items"];
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
        itemConfig["Item Name"] = WeaponNames::instance().getWeaponName(gameItem.getWeaponID());

        if (gameItem.isSticker()) {
            itemConfig["Sticker ID"] = gameItemStorage.getStickerKit(gameItem).id;
        } else if (gameItem.isGloves()) {
            const auto& staticData = gameItemStorage.getPaintKit(gameItem);
            itemConfig["Paint Kit"] = staticData.id;
            itemConfig["Paint Kit Name"] = staticData.name.forDisplay;

            if (const auto glove = item.get<inventory::Glove>()) {
                itemConfig["Wear"] = glove->wear;
                itemConfig["Seed"] = glove->seed;
            }
        } else if (gameItem.isSkin()) {
            const auto& staticData = gameItemStorage.getPaintKit(gameItem);
            itemConfig["Paint Kit"] = staticData.id;
            itemConfig["Paint Kit Name"] = staticData.name.forDisplay;

            if (const auto skin = item.get<inventory::Skin>())
                itemConfig.update(::toJson(*skin));

        } else if (gameItem.isMusic()) {
            itemConfig["Music ID"] = gameItemStorage.getMusicKit(gameItem).id;
            if (const auto music = item.get<inventory::Music>(); music && music->statTrak > -1)
                itemConfig["StatTrak"] = music->statTrak;
        } else if (gameItem.isPatch()) {
            itemConfig["Patch ID"] = gameItemStorage.getPatch(gameItem).id;
        } else if (gameItem.isGraffiti()) {
            itemConfig["Graffiti ID"] = gameItemStorage.getGraffitiKit(gameItem).id;
            if (const auto graffiti = item.get<inventory::Graffiti>(); graffiti && graffiti->usesLeft >= 0) {
                itemConfig["Uses Left"] = graffiti->usesLeft;
                itemConfig["Item Name"] = WeaponNames::instance().getWeaponName(WeaponId::Graffiti);
            }
        } else if (gameItem.isAgent()) {
            if (const auto agent = item.get<inventory::Agent>()) {
                auto& stickers = itemConfig["Patches"];
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
        } else if (gameItem.isServiceMedal()) {
            if (const auto serviceMedal = item.get<inventory::ServiceMedal>(); serviceMedal && serviceMedal->issueDateTimestamp != 0)
                itemConfig["Issue Date Timestamp"] = serviceMedal->issueDateTimestamp;
        } else if (gameItem.isCase()) {
            if (const auto souvenirPackage = item.get<inventory::SouvenirPackage>(); souvenirPackage && souvenirPackage->tournamentStage != TournamentStage{}) {
                itemConfig["Tournament Stage"] = souvenirPackage->tournamentStage;
                itemConfig["Tournament Team 1"] = souvenirPackage->tournamentTeam1;
                itemConfig["Tournament Team 2"] = souvenirPackage->tournamentTeam2;
                itemConfig["Tournament Player"] = souvenirPackage->proPlayer;
            }
        } else if (gameItem.isStorageUnit()) {
            if (const auto storageUnit = item.get<inventory_changer::inventory::StorageUnit>(); storageUnit && storageUnit->modificationDateTimestamp != 0) {
                itemConfig["Modification Date Timestamp"] = storageUnit->modificationDateTimestamp;
                itemConfig["Name"] = storageUnit->name;
            }
        }

        if (const auto storageUnitID = storageUnitIDs.find(itemIt); storageUnitID != storageUnitIDs.end())
            itemConfig["Storage Unit ID"] = storageUnitID->second;

        items.push_back(std::move(itemConfig));
    }

    j.emplace("Pick'Em", ::toJson(backend.getPickEm()));
    return j;
}

std::optional<std::uint32_t> storageUnitIdFromJson(const json& j)
{
    if (const auto it = j.find("Storage Unit ID"); it != j.end() && it->is_number_unsigned())
        return it->get<std::uint32_t>();
    return {};
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
        std::optional<std::reference_wrapper<const game_items::Item>> itemOptional = gameItemFromJson(lookup, jsonItem);
        if (!itemOptional.has_value())
            continue;

        const game_items::Item& item = itemOptional->get();
        const auto itemAdded = backend.addItemAcknowledged(inventory::Item{ item, itemFromJson(lookup.getStorage(), item, jsonItem) });

        if (const auto storageUnitID = storageUnitIdFromJson(jsonItem); storageUnitID.has_value()) {
            if (!item.isStorageUnit()) {
                itemsToBindToStorageUnits.emplace_back(itemAdded, *storageUnitID);
            } else {
                storageUnits.emplace(*storageUnitID, itemAdded);
            }
        }

        if (const auto equippedSlot = equippedSlotFromJson(jsonItem); equippedSlot != static_cast<std::uint8_t>(-1)) {
            const auto equippedState = equippedFromJson(jsonItem);
            if (equippedState.ct)
                backend.equipItemCT(itemAdded, equippedSlot);
            if (equippedState.tt)
                backend.equipItemTT(itemAdded, equippedSlot);
            if (equippedState.noTeam)
                backend.equipItemNoTeam(itemAdded, equippedSlot);
        }

    }

    for (auto [item, storageUnitID] : itemsToBindToStorageUnits) {
        if (const auto storageUnit = storageUnits.find(storageUnitID); storageUnit != storageUnits.end()) {
            backend.getRequestor().request<inventory_changer::backend::request::BindItemToStorageUnit>(item, storageUnit->second);
        }
    }
}
