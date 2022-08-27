#include "Inventory.h"

#include <Interfaces.h>
#include <InventoryChanger/GameItems/Lookup.h>
#include <InventoryChanger/Inventory/Item.h>
#include <SDK/EconItemView.h>
#include <SDK/Panorama.h>

namespace inventory_changer::game_integration
{

namespace
{

[[nodiscard]] EconItem* getEconItem(std::uint64_t itemID)
{
    if (const auto view = memory->findOrCreateEconItemViewForItemID(itemID))
        return memory->getSOCData(view);
    return nullptr;
}

void initItemCustomizationNotification(std::string_view typeStr, std::uint64_t itemID)
{
    const auto idx = memory->registeredPanoramaEvents->find(memory->makePanoramaSymbol("PanoramaComponent_Inventory_ItemCustomizationNotification"));
    if (idx == -1)
        return;

    using namespace std::string_view_literals;
    std::string args{ "0,'" }; args += typeStr; args += "','"sv; args += std::to_string(itemID); args += '\'';
    const char* dummy;
    if (const auto event = memory->registeredPanoramaEvents->memory[idx].value.createEventFromString(nullptr, args.c_str(), &dummy))
        interfaces->panoramaUIEngine->accessUIEngine()->dispatchEvent(event);
}

void updateNameTag(std::uint64_t itemID, const char* newNameTag)
{
    const auto econItem = getEconItem(itemID);
    if (!econItem)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    memory->setCustomName(econItem, newNameTag);
    localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);
}

void updatePatch(std::uint64_t itemID, int patchID, std::uint8_t slot)
{
    const auto econItem = getEconItem(itemID);
    if (!econItem)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    EconItemAttributeSetter attributeSetter{ *memory->itemSystem()->getItemSchema() };
    attributeSetter.setStickerID(*econItem, slot, patchID);
    localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);
}

void setItemHiddenFlag(std::uint64_t itemID, bool hide)
{
    const auto econItem = getEconItem(itemID);
    if (!econItem)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    if (hide)
        econItem->flags |= 16;
    else
        econItem->flags &= ~16;

    localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);
}

}

void initSkinEconItem(const game_items::Storage& gameItemStorage, const inventory::Item& inventoryItem, EconItem& econItem) noexcept
{
    EconItemAttributeSetter attributeSetter{ *memory->itemSystem()->getItemSchema() };

    const auto paintKit = gameItemStorage.getPaintKit(inventoryItem.gameItem()).id;
    attributeSetter.setPaintKit(econItem, static_cast<float>(paintKit));

    const auto skin = get<inventory::Skin>(inventoryItem);
    if (!skin)
        return;

    const auto& dynamicData = *skin;
    const auto isMP5LabRats = Helpers::isMP5LabRats(inventoryItem.gameItem().getWeaponID(), paintKit);
    if (dynamicData.isSouvenir() || isMP5LabRats) {
        econItem.quality = 12;
    } else {
        if (dynamicData.statTrak > -1) {
            attributeSetter.setStatTrak(econItem, dynamicData.statTrak);
            attributeSetter.setStatTrakType(econItem, 0);
            econItem.quality = 9;
        }
        if (Helpers::isKnife(econItem.weaponId))
            econItem.quality = 3;
    }

    if (isMP5LabRats) {
        attributeSetter.setSpecialEventID(econItem, 1);
    } else {
        if (dynamicData.tournamentID != csgo::Tournament{})
            attributeSetter.setTournamentID(econItem, static_cast<int>(dynamicData.tournamentID));

        if (dynamicData.tournamentStage != TournamentStage{ 0 }) {
            attributeSetter.setTournamentStage(econItem, static_cast<int>(dynamicData.tournamentStage));
            attributeSetter.setTournamentTeam1(econItem, static_cast<int>(dynamicData.tournamentTeam1));
            attributeSetter.setTournamentTeam2(econItem, static_cast<int>(dynamicData.tournamentTeam2));
            if (dynamicData.proPlayer != static_cast<csgo::ProPlayer>(0))
                attributeSetter.setTournamentPlayer(econItem, static_cast<int>(dynamicData.proPlayer));
        }
    }

    attributeSetter.setWear(econItem, dynamicData.wear);
    attributeSetter.setSeed(econItem, static_cast<float>(dynamicData.seed));
    memory->setCustomName(&econItem, dynamicData.nameTag.c_str());

    for (std::size_t j = 0; j < dynamicData.stickers.size(); ++j) {
        const auto& sticker = dynamicData.stickers[j];
        if (sticker.stickerID == 0)
            continue;

        attributeSetter.setStickerID(econItem, j, sticker.stickerID);
        attributeSetter.setStickerWear(econItem, j, sticker.wear);
    }
}

std::uint64_t Inventory::createSOCItem(const game_items::Storage& gameItemStorage, const inventory::Item& inventoryItem, bool asUnacknowledged)
{
    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return 0;

    const auto baseTypeCache = localInventory->getItemBaseTypeCache();
    if (!baseTypeCache)
        return 0;

    const auto econItem = memory->createEconItemSharedObject();
    econItem->itemID = localInventory->getHighestIDs().first + 1;
    econItem->originalID = 0;
    econItem->accountID = localInventory->getAccountID();
    econItem->inventory = asUnacknowledged ? 0 : localInventory->getHighestIDs().second + 1;

    const auto& item = inventoryItem.gameItem();
    econItem->rarity = static_cast<std::uint16_t>(item.getRarity());
    constexpr auto foundInCrateOrigin = 8;
    econItem->origin = foundInCrateOrigin;
    econItem->quality = 4;
    econItem->weaponId = item.getWeaponID();

    if (inventoryItem.getState() == inventory::Item::State::InXrayScanner)
        econItem->flags |= 16;

    EconItemAttributeSetter attributeSetter{ *memory->itemSystem()->getItemSchema() };

    if (const auto tradableAfterDate = inventoryItem.getProperties().common.tradableAfterDate; tradableAfterDate != 0) {
        attributeSetter.setTradableAfterDate(*econItem, tradableAfterDate);
    }

    if (item.isSticker()) {
        attributeSetter.setStickerID(*econItem, 0, gameItemStorage.getStickerKit(item).id);
    } else if (item.isPatch()) {
        attributeSetter.setStickerID(*econItem, 0, gameItemStorage.getPatch(item).id);
    } else if (item.isGraffiti()) {
        attributeSetter.setStickerID(*econItem, 0, gameItemStorage.getGraffitiKit(item).id);
        if (const auto graffiti = get<inventory::Graffiti>(inventoryItem); graffiti && graffiti->usesLeft >= 0) {
            econItem->weaponId = WeaponId::Graffiti;
            attributeSetter.setSpraysRemaining(*econItem, graffiti->usesLeft);
        }
    } else if (item.isMusic()) {
        attributeSetter.setMusicID(*econItem, gameItemStorage.getMusicKit(item).id);
        if (const auto music = get<inventory::Music>(inventoryItem); music && music->statTrak > -1) {
            attributeSetter.setStatTrak(*econItem, music->statTrak);
            attributeSetter.setStatTrakType(*econItem, 1);
            econItem->quality = 9;
        }
    } else if (item.isSkin()) {
        initSkinEconItem(gameItemStorage, inventoryItem, *econItem);
    } else if (item.isGloves()) {
        econItem->quality = 3;
        attributeSetter.setPaintKit(*econItem, static_cast<float>(gameItemStorage.getPaintKit(item).id));

        if (const auto glove = get<inventory::Gloves>(inventoryItem)) {
            attributeSetter.setWear(*econItem, glove->wear);
            attributeSetter.setSeed(*econItem, static_cast<float>(glove->seed));
        }
    } else if (item.isCollectible()) {
        if (gameItemStorage.isCollectibleGenuine(item))
            econItem->quality = 1;
    } else if (item.isAgent()) {
        if (const auto agent = get<inventory::Agent>(inventoryItem)) {
            for (std::size_t j = 0; j < agent->patches.size(); ++j) {
                const auto& patch = agent->patches[j];
                if (patch.patchID == 0)
                    continue;

                attributeSetter.setStickerID(*econItem, j, patch.patchID);
            }
        }
    } else if (item.isServiceMedal()) {
        if (const auto serviceMedal = get<inventory::ServiceMedal>(inventoryItem); serviceMedal && serviceMedal->issueDateTimestamp != 0)
            attributeSetter.setIssueDate(*econItem, serviceMedal->issueDateTimestamp);
    } else if (item.isTournamentCoin()) {
        if (const auto tournamentCoin = get<inventory::TournamentCoin>(inventoryItem))
            attributeSetter.setDropsAwarded(*econItem, tournamentCoin->dropsAwarded);
        attributeSetter.setDropsRedeemed(*econItem, 0);
        attributeSetter.setStickerID(*econItem, 0, gameItemStorage.getDefaultTournamentGraffitiID(item));
        attributeSetter.setCampaignCompletion(*econItem, 1);
    } else if (item.isCrate()) {
        if (const auto souvenirPackage = get<inventory::SouvenirPackage>(inventoryItem); souvenirPackage && souvenirPackage->tournamentStage != TournamentStage{ 0 }) {
            attributeSetter.setTournamentStage(*econItem, static_cast<int>(souvenirPackage->tournamentStage));
            attributeSetter.setTournamentTeam1(*econItem, static_cast<int>(souvenirPackage->tournamentTeam1));
            attributeSetter.setTournamentTeam2(*econItem, static_cast<int>(souvenirPackage->tournamentTeam2));
            if (souvenirPackage->proPlayer != static_cast<csgo::ProPlayer>(0))
                attributeSetter.setTournamentPlayer(*econItem, static_cast<int>(souvenirPackage->proPlayer));
        }
    } else if (item.isCaseKey()) {
        constexpr auto nonEconomyFlag = 8;
        econItem->flags |= nonEconomyFlag;
    } else if (item.isStorageUnit()) {
        if (const auto storageUnit = get<inventory::StorageUnit>(inventoryItem); storageUnit && storageUnit->modificationDateTimestamp != 0) {
            attributeSetter.setModificationDate(*econItem, storageUnit->modificationDateTimestamp);
            memory->setCustomName(econItem, storageUnit->name.c_str());
        }
    }

    baseTypeCache->addObject(econItem);
    localInventory->soCreated(localInventory->getSOID(), (SharedObject*)econItem, 4);

    if (const auto inventoryComponent = *memory->uiComponentInventory) {
        memory->setItemSessionPropertyValue(inventoryComponent, econItem->itemID, "recent", "0");
        memory->setItemSessionPropertyValue(inventoryComponent, econItem->itemID, "updated", "0");
    }

    if (const auto view = memory->findOrCreateEconItemViewForItemID(econItem->itemID))
        view->clearInventoryImageRGBA();

    return econItem->itemID;
}

std::uint64_t Inventory::assingNewItemID(std::uint64_t itemID)
{
    const auto econItem = getEconItem(itemID);
    if (!econItem)
        return itemID;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return itemID;

    localInventory->soDestroyed(localInventory->getSOID(), (SharedObject*)econItem, 4);
    const auto newItemID = localInventory->getHighestIDs().first + 1;
    econItem->itemID = newItemID;
    localInventory->soCreated(localInventory->getSOID(), (SharedObject*)econItem, 4);

    if (const auto newView = memory->findOrCreateEconItemViewForItemID(newItemID))
        newView->clearInventoryImageRGBA();

    if (const auto inventoryComponent = *memory->uiComponentInventory) {
        memory->setItemSessionPropertyValue(inventoryComponent, newItemID, "recent", "0");
        memory->setItemSessionPropertyValue(inventoryComponent, newItemID, "updated", "0");
    }

    return newItemID;
}

void Inventory::applySticker(std::uint64_t itemID, int stickerID, std::uint8_t slot)
{
    const auto econItem = getEconItem(itemID);
    if (!econItem)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    EconItemAttributeSetter attributeSetter{ *memory->itemSystem()->getItemSchema() };
    attributeSetter.setStickerID(*econItem, slot, stickerID);
    attributeSetter.setStickerWear(*econItem, slot, 0.0f);

    localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);
    initItemCustomizationNotification("sticker_apply", itemID);
}

void Inventory::removeSticker(std::uint64_t itemID, std::uint8_t slot)
{
    const auto econItem = getEconItem(itemID);
    if (!econItem)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    EconItemAttributeSetter attributeSetter{ *memory->itemSystem()->getItemSchema() };
    attributeSetter.setStickerID(*econItem, slot, 0);
    attributeSetter.setStickerWear(*econItem, slot, 0.0f);
    localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);
    initItemCustomizationNotification("sticker_remove", itemID);
}

void Inventory::updateStickerWear(std::uint64_t itemID, std::uint8_t slot, float newWear)
{
    const auto econItem = getEconItem(itemID);
    if (!econItem)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    EconItemAttributeSetter attributeSetter{ *memory->itemSystem()->getItemSchema() };
    attributeSetter.setStickerWear(*econItem, slot, newWear);
    localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);
}

void Inventory::viewerPassActivated(std::uint64_t tournamentCoinItemID)
{
    initItemCustomizationNotification("ticket_activated", tournamentCoinItemID);
}

void Inventory::addNameTag(std::uint64_t itemID, const char* newNameTag)
{
    updateNameTag(itemID, newNameTag);
    initItemCustomizationNotification("nametag_add", itemID);
}

void Inventory::removeNameTag(std::uint64_t itemID)
{
    updateNameTag(itemID, "");
}

void Inventory::deleteItem(std::uint64_t itemID)
{
    const auto econItem = getEconItem(itemID);
    if (!econItem)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    localInventory->soDestroyed(localInventory->getSOID(), (SharedObject*)econItem, 4);

    if (const auto baseTypeCache = localInventory->getItemBaseTypeCache())
        baseTypeCache->removeObject(econItem);

    econItem->destructor();
}

void Inventory::updateStatTrak(std::uint64_t itemID, int newStatTrakValue)
{
    const auto econItem = getEconItem(itemID);
    if (!econItem)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    EconItemAttributeSetter attributeSetter{ *memory->itemSystem()->getItemSchema() };
    attributeSetter.setStatTrak(*econItem, newStatTrakValue);
    localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);
}

void Inventory::containerOpened(std::uint64_t unlockedItemID)
{
    initItemCustomizationNotification("crate_unlock", unlockedItemID);
}

void Inventory::applyPatch(std::uint64_t itemID, int patchID, std::uint8_t slot)
{
    updatePatch(itemID, patchID, slot);
    initItemCustomizationNotification("patch_apply", itemID);
}

void Inventory::removePatch(std::uint64_t itemID, std::uint8_t slot)
{
    updatePatch(itemID, 0, slot);
    initItemCustomizationNotification("patch_remove", itemID);
}

void Inventory::souvenirTokenActivated(std::uint64_t itemID, std::uint32_t dropsAwarded)
{
    const auto econItem = getEconItem(itemID);
    if (!econItem)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    EconItemAttributeSetter attributeSetter{ *memory->itemSystem()->getItemSchema() };
    attributeSetter.setDropsAwarded(*econItem, dropsAwarded);
    localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);
    initItemCustomizationNotification("ticket_activated", itemID);
}

void Inventory::unsealGraffiti(std::uint64_t itemID)
{
    const auto econItem = getEconItem(itemID);
    if (!econItem || econItem->weaponId != WeaponId::SealedGraffiti)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    EconItemAttributeSetter attributeSetter{ *memory->itemSystem()->getItemSchema() };
    attributeSetter.setSpraysRemaining(*econItem, 50);
    econItem->weaponId = WeaponId::Graffiti;
    localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);
    initItemCustomizationNotification("graffity_unseal", itemID);
}

void Inventory::selectTeamGraffiti(std::uint64_t itemID, std::uint16_t graffitiID)
{
    const auto econItem = getEconItem(itemID);
    if (!econItem)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    EconItemAttributeSetter attributeSetter{ *memory->itemSystem()->getItemSchema() };
    attributeSetter.setStickerID(*econItem, 0, graffitiID);
    localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);
}

void Inventory::statTrakSwapped(std::uint64_t itemID)
{
    initItemCustomizationNotification("stattrack_swap", itemID);
}

void Inventory::equipItem(std::uint64_t itemID, csgo::Team team, std::uint8_t slot)
{
    memory->inventoryManager->equipItemInSlot(team, slot, itemID);
}

void Inventory::markItemUpdated(std::uint64_t itemID)
{
    if (const auto inventoryComponent = *memory->uiComponentInventory) {
        memory->setItemSessionPropertyValue(inventoryComponent, itemID, "recent", "0");
        memory->setItemSessionPropertyValue(inventoryComponent, itemID, "updated", "1");
    }
}

void Inventory::pickEmUpdated()
{
    if (const auto idx = memory->registeredPanoramaEvents->find(memory->makePanoramaSymbol("PanoramaComponent_MatchList_PredictionUploaded")); idx != -1) {
        const char* dummy;
        if (const auto eventPtr = memory->registeredPanoramaEvents->memory[idx].value.createEventFromString(nullptr, "", &dummy))
            interfaces->panoramaUIEngine->accessUIEngine()->dispatchEvent(eventPtr);
    }
}

void Inventory::hideItem(std::uint64_t itemID)
{
    setItemHiddenFlag(itemID, true);
}

void Inventory::unhideItem(std::uint64_t itemID)
{
    setItemHiddenFlag(itemID, false);
}

void Inventory::xRayItemRevealed(std::uint64_t itemID)
{
   initItemCustomizationNotification("xray_item_reveal", itemID);
}

void Inventory::xRayItemClaimed(std::uint64_t itemID)
{
    const auto econItem = getEconItem(itemID);
    if (!econItem)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    econItem->flags &= ~16;

    localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);

    initItemCustomizationNotification("xray_item_claim", itemID);
}

void Inventory::nameStorageUnit(std::uint64_t itemID, const char* newName)
{
    addNameTag(itemID, newName);
}

void Inventory::storageUnitModified(std::uint64_t itemID, std::uint32_t modificationDate, std::uint32_t itemCount)
{
    const auto econItem = getEconItem(itemID);
    if (!econItem)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    EconItemAttributeSetter attributeSetter{ *memory->itemSystem()->getItemSchema() };
    attributeSetter.setModificationDate(*econItem, modificationDate);
    attributeSetter.setItemsCount(*econItem, itemCount);

    localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);
}

void Inventory::addItemToStorageUnit(std::uint64_t itemID, std::uint64_t storageUnitItemID)
{
    const auto econItem = getEconItem(itemID);
    if (!econItem)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    EconItemAttributeSetter attributeSetter{ *memory->itemSystem()->getItemSchema() };
    attributeSetter.setCasketItemIdLow(*econItem, static_cast<std::uint32_t>(storageUnitItemID & 0xFFFFFFFF));
    attributeSetter.setCasketItemIdHigh(*econItem, static_cast<std::uint32_t>((storageUnitItemID >> 32) & 0xFFFFFFFF));

    localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);
}

void Inventory::itemAddedToStorageUnit(std::uint64_t storageUnitItemID)
{
    initItemCustomizationNotification("casket_added", storageUnitItemID);
}

void Inventory::removeItemFromStorageUnit(std::uint64_t itemID, std::uint64_t storageUnitItemID)
{
    const auto econItem = getEconItem(itemID);
    if (!econItem)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    EconItemAttributeSetter attributeSetter{ *memory->itemSystem()->getItemSchema() };
    attributeSetter.removeCasketItemId(*econItem);

    localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);
    initItemCustomizationNotification("casket_removed", storageUnitItemID);
}

void Inventory::updateTradableAfterDate(std::uint64_t itemID, std::uint32_t tradableAfterDate)
{
    const auto econItem = getEconItem(itemID);
    if (!econItem)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    EconItemAttributeSetter attributeSetter{ *memory->itemSystem()->getItemSchema() };
    attributeSetter.setTradableAfterDate(*econItem, tradableAfterDate);

    localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);
}

}
