#include "InventoryConfig.h"

#include "GameItems/Lookup.h"
#include "Inventory/Item.h"
#include "Inventory/Structs.h"
#include "Inventory/StructsFromJson.h"

#include "Backend/BackendSimulator.h"

#include "StaticData.h"

#include "BackendResponseHandler.h"

constexpr auto CONFIG_VERSION = 4;

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

    const auto& backend = inventory_changer::backend::BackendSimulator::instance();
    const auto& loadout = backend.getLoadout();
    const auto& inventory = backend.getInventory();
    auto& items = j["Items"];
    for (auto itemIt = inventory.begin(); itemIt != inventory.end(); ++itemIt) {
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
        itemConfig["Item Name"] = StaticData::getWeaponName(gameItem.getWeaponID());

        if (gameItem.isSticker()) {
            itemConfig["Sticker ID"] = StaticData::lookup().getStorage().getStickerKit(gameItem).id;
        } else if (gameItem.isGloves()) {
            const auto& staticData = StaticData::lookup().getStorage().getPaintKit(gameItem);
            itemConfig["Paint Kit"] = staticData.id;
            itemConfig["Paint Kit Name"] = staticData.name.forDisplay;

            if (const auto glove = item.get<inventory::Glove>()) {
                itemConfig["Wear"] = glove->wear;
                itemConfig["Seed"] = glove->seed;
            }
        } else if (gameItem.isSkin()) {
            const auto& staticData = StaticData::lookup().getStorage().getPaintKit(gameItem);
            itemConfig["Paint Kit"] = staticData.id;
            itemConfig["Paint Kit Name"] = staticData.name.forDisplay;

            if (const auto skin = item.get<inventory::Skin>())
                itemConfig.update(::toJson(*skin));

        } else if (gameItem.isMusic()) {
            itemConfig["Music ID"] = StaticData::lookup().getStorage().getMusicKit(gameItem).id;
            if (const auto music = item.get<inventory::Music>(); music && music->statTrak > -1)
                itemConfig["StatTrak"] = music->statTrak;
        } else if (gameItem.isPatch()) {
            itemConfig["Patch ID"] = StaticData::lookup().getStorage().getPatch(gameItem).id;
        } else if (gameItem.isGraffiti()) {
            itemConfig["Graffiti ID"] = StaticData::lookup().getStorage().getGraffitiKit(gameItem).id;
            if (const auto graffiti = item.get<inventory::Graffiti>(); graffiti && graffiti->usesLeft >= 0) {
                itemConfig["Uses Left"] = graffiti->usesLeft;
                itemConfig["Item Name"] = StaticData::getWeaponName(WeaponId::Graffiti);
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
            if (StaticData::isSouvenirPackage(gameItem)) {
                if (const auto souvenirPackage = item.get<inventory::SouvenirPackage>(); souvenirPackage && souvenirPackage->tournamentStage != TournamentStage{}) {
                    itemConfig["Tournament Stage"] = souvenirPackage->tournamentStage;
                    itemConfig["Tournament Team 1"] = souvenirPackage->tournamentTeam1;
                    itemConfig["Tournament Team 2"] = souvenirPackage->tournamentTeam2;
                    itemConfig["Tournament Player"] = souvenirPackage->proPlayer;
                }
            }
        }

        items.push_back(std::move(itemConfig));
    }

    return j;
}

[[nodiscard]] inventory::Agent loadDynamicAgentDataFromJson(const json& j) noexcept
{
    inventory::Agent dynamicData;
    dynamicData.patches = inventory::agentPatchesFromJson(j);
    return dynamicData;
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

[[nodiscard]] inventory::ItemData itemFromJson(const game_items::Item& gameItem, const json& j)
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

void InventoryChanger::fromJson(const json& j) noexcept
{
    if (!j.contains("Items"))
        return;

    const auto& items = j["Items"];
    if (!items.is_array())
        return;

    auto& backend = inventory_changer::backend::BackendSimulator::instance();
    for (const auto& jsonItem : items) {
        std::optional<std::reference_wrapper<const game_items::Item>> itemOptional = gameItemFromJson(StaticData::lookup(), jsonItem);
        if (!itemOptional.has_value())
            continue;

        const game_items::Item& item = itemOptional->get();
        const auto itemAdded = backend.addItemAcknowledged(inventory::Item{ item, itemFromJson(item, jsonItem) });

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
}

void InventoryChanger::resetConfig() noexcept
{
    auto& backend = inventory_changer::backend::BackendSimulator::instance();
    backend.clearInventory();
    backend.run(inventory_changer::BackendResponseHandler{ backend }, std::chrono::milliseconds{ 0 });
}
