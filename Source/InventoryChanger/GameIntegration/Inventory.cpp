#include "Inventory.h"

#include <InventoryChanger/EconItemFunctions.h>
#include <InventoryChanger/GameItems/Lookup.h>
#include <InventoryChanger/Inventory/Item.h>
#include <CSGO/CSPlayerInventory.h>
#include <CSGO/EconItemView.h>
#include <CSGO/Panorama.h>
#include <CSGO/UiComponentInventory.h>

#include <CSGO/Constants/EconItemFlags.h>

#include <Utils/FlagsBuilder.h>
#include <Utils/StringBuilder.h>
#include <Interfaces/OtherInterfaces.h>

namespace inventory_changer::game_integration
{

namespace
{

[[nodiscard]] csgo::EconItemPOD* getEconItem(const Memory& memory, const EconItemViewFunctions& econItemViewFunctions, ItemId itemID)
{
    if (const auto view = csgo::EconItemView::from(retSpoofGadgets->client, memory.findOrCreateEconItemViewForItemID(static_cast<csgo::ItemId>(itemID)), econItemViewFunctions); view.getPOD() != nullptr)
        return view.getSOCData();
    return nullptr;
}

void initItemCustomizationNotification(const OtherInterfaces& interfaces, const Memory& memory, std::string_view typeStr, ItemId itemID)
{
    const auto idx = memory.registeredPanoramaEvents->find(memory.makePanoramaSymbol("PanoramaComponent_Inventory_ItemCustomizationNotification"));
    if (idx == -1)
        return;

    const char* dummy;
    if (const auto event = retSpoofGadgets->client.invokeCdecl<void*>(std::uintptr_t(memory.registeredPanoramaEvents->memory[idx].value.createEventFromString), nullptr, StringBuilderStorage<50>{}.builder().put("0,'", typeStr, "','", static_cast<csgo::ItemId>(itemID), '\'').cstring(), &dummy))
        csgo::UIEngine::from(retSpoofGadgets->client, interfaces.getPanoramaUIEngine().accessUIEngine()).dispatchEvent(event);
}

void updateNameTag(const Memory& memory, ItemId itemID, const char* newNameTag, const EconItemFunctions& econItemFunctions, const EconItemViewFunctions& econItemViewFunctions)
{
    auto econItem = csgo::EconItem::from(retSpoofGadgets->client, getEconItem(memory, econItemViewFunctions, itemID), econItemFunctions);
    if (econItem.getPOD() == nullptr)
        return;

    const auto localInventory = csgo::CSPlayerInventory::from(retSpoofGadgets->client, memory.inventoryManager.getLocalInventory());
    if (localInventory.getPOD() == nullptr)
        return;

    econItem.setCustomName(newNameTag);
    localInventory.soUpdated(localInventory.getSOID(), (csgo::SharedObjectPOD*)econItem.getPOD(), 4);
}

void updatePatch(const Memory& memory, ItemId itemID, int patchID, std::uint8_t slot, const EconItemFunctions& econItemFunctions, const EconItemViewFunctions& econItemViewFunctions)
{
    auto econItem = csgo::EconItem::from(retSpoofGadgets->client, getEconItem(memory, econItemViewFunctions, itemID), econItemFunctions);
    if (econItem.getPOD() == nullptr)
        return;

    const auto localInventory = csgo::CSPlayerInventory::from(retSpoofGadgets->client, memory.inventoryManager.getLocalInventory());
    if (localInventory.getPOD() == nullptr)
        return;

    csgo::EconItemAttributeSetter attributeSetter{ csgo::ItemSchema::from(retSpoofGadgets->client, memory.itemSystem().getItemSchema()) };
    attributeSetter.setStickerID(econItem, slot, patchID);
    localInventory.soUpdated(localInventory.getSOID(), (csgo::SharedObjectPOD*)econItem.getPOD(), 4);
}

void setItemHiddenFlag(const Memory& memory, const EconItemViewFunctions& econItemViewFunctions, ItemId itemID, bool hide)
{
    const auto econItem = getEconItem(memory, econItemViewFunctions, itemID);
    if (!econItem)
        return;

    const auto localInventory = csgo::CSPlayerInventory::from(retSpoofGadgets->client, memory.inventoryManager.getLocalInventory());
    if (localInventory.getPOD() == nullptr)
        return;

    using enum csgo::EconItemFlags;

    if (hide)
        econItem->flags |= static_cast<std::uint8_t>(InXrayScanner);
    else
        econItem->flags &= ~static_cast<std::uint8_t>(InXrayScanner);

    localInventory.soUpdated(localInventory.getSOID(), (csgo::SharedObjectPOD*)econItem, 4);
}

}

void initSkinEconItem(const Memory& memory, const game_items::Storage& gameItemStorage, const inventory::Item& inventoryItem, csgo::EconItem& econItem) noexcept
{
    csgo::EconItemAttributeSetter attributeSetter{ csgo::ItemSchema::from(retSpoofGadgets->client, memory.itemSystem().getItemSchema()) };

    const auto paintKit = gameItemStorage.getPaintKit(inventoryItem.gameItem()).id;
    attributeSetter.setPaintKit(econItem, static_cast<float>(paintKit));

    const auto skin = get<inventory::Skin>(inventoryItem);
    if (!skin)
        return;

    const auto& dynamicData = *skin;
    const auto isMP5LabRats = Helpers::isMP5LabRats(inventoryItem.gameItem().getWeaponID(), paintKit);
    if (dynamicData.isSouvenir() || isMP5LabRats) {
        econItem.getPOD()->quality = 12;
    } else {
        if (dynamicData.statTrak > -1) {
            attributeSetter.setStatTrak(econItem, dynamicData.statTrak);
            attributeSetter.setStatTrakType(econItem, 0);
            econItem.getPOD()->quality = 9;
        }
        if (Helpers::isKnife(econItem.getPOD()->weaponId))
            econItem.getPOD()->quality = 3;
    }

    if (isMP5LabRats) {
        attributeSetter.setSpecialEventID(econItem, 1);
    } else {
        if (dynamicData.tournamentID != csgo::Tournament{})
            attributeSetter.setTournamentID(econItem, static_cast<int>(dynamicData.tournamentID));

        if (dynamicData.tournamentStage != csgo::TournamentStage{}) {
            attributeSetter.setTournamentStage(econItem, static_cast<int>(dynamicData.tournamentStage));
            attributeSetter.setTournamentTeam1(econItem, static_cast<int>(dynamicData.tournamentTeam1));
            attributeSetter.setTournamentTeam2(econItem, static_cast<int>(dynamicData.tournamentTeam2));
            if (dynamicData.proPlayer != csgo::ProPlayer{})
                attributeSetter.setTournamentPlayer(econItem, static_cast<int>(dynamicData.proPlayer));
        }
    }

    attributeSetter.setWear(econItem, dynamicData.wear);
    attributeSetter.setSeed(econItem, static_cast<float>(dynamicData.seed));
    econItem.setCustomName(dynamicData.nameTag.c_str());

    for (std::size_t j = 0; j < dynamicData.stickers.size(); ++j) {
        const auto& sticker = dynamicData.stickers[j];
        if (sticker.stickerID == csgo::StickerId::Default)
            continue;

        attributeSetter.setStickerID(econItem, j, static_cast<int>(sticker.stickerID));
        attributeSetter.setStickerWear(econItem, j, sticker.wear);
    }
}

[[nodiscard]] std::uint8_t computeItemFlags(const inventory::Item& item)
{
    using enum csgo::EconItemFlags;
    return FlagsBuilder<std::uint8_t, csgo::EconItemFlags>{}
        .set<PurchasedFromStore>(item.getProperties().common.purchasedFromStore)
        .set<NonEconomy>(item.gameItem().isCaseKey())
        .set<InXrayScanner>(item.getState() == inventory::Item::State::InXrayScanner)
        .get();
}

Inventory::Inventory(OtherInterfaces interfaces, const Memory& memory, const ClientPatternFinder& clientPatternFinder)
    : interfaces{ interfaces }, memory{ memory }, econItemFunctions{ createEconItemFunctions(clientPatternFinder) }, econItemViewFunctions{ createEconItemViewFunctions(clientPatternFinder) },
    createEconItemSharedObject{ retSpoofGadgets->client, clientPatternFinder.createEconItemSharedObject() },
    uiComponentInventory{ clientPatternFinder.uiComponentInventory() },
    setItemSessionPropertyValue{ clientPatternFinder.setItemSessionPropertyValue() }
{
}

ItemId Inventory::createSOCItem(const game_items::Storage& gameItemStorage, const inventory::Item& inventoryItem, bool asUnacknowledged)
{
    const auto localInventory = csgo::CSPlayerInventory::from(retSpoofGadgets->client, memory.inventoryManager.getLocalInventory());
    if (localInventory.getPOD() == nullptr)
        return {};

    const auto baseTypeCache = getItemBaseTypeCache(localInventory, memory.createBaseTypeCache);
    if (!baseTypeCache)
        return {};

    const auto econItemPOD = createEconItemSharedObject();
    auto econItem = csgo::EconItem::from(retSpoofGadgets->client, econItemPOD, econItemFunctions);
    econItemPOD->itemID = baseTypeCache->getHighestIDs().first + 1;
    econItemPOD->originalID = 0;
    econItemPOD->accountID = localInventory.getAccountID();
    econItemPOD->inventory = asUnacknowledged ? 0 : baseTypeCache->getHighestIDs().second + 1;

    const auto& item = inventoryItem.gameItem();
    econItemPOD->rarity = static_cast<std::uint16_t>(item.getRarity());
    constexpr auto foundInCrateOrigin = 8;
    econItemPOD->origin = foundInCrateOrigin;
    econItemPOD->quality = 4;
    econItemPOD->weaponId = item.getWeaponID();
    econItemPOD->flags = computeItemFlags(inventoryItem);

    csgo::EconItemAttributeSetter attributeSetter{ csgo::ItemSchema::from(retSpoofGadgets->client, memory.itemSystem().getItemSchema()) };

    if (const auto tradableAfterDate = inventoryItem.getProperties().common.tradableAfterDate; tradableAfterDate != 0) {
        attributeSetter.setTradableAfterDate(econItem, tradableAfterDate);
    }

    if (item.isSticker()) {
        attributeSetter.setStickerID(econItem, 0, static_cast<int>(gameItemStorage.getStickerKit(item).id));
    } else if (item.isPatch()) {
        attributeSetter.setStickerID(econItem, 0, gameItemStorage.getPatch(item).id);
    } else if (item.isGraffiti()) {
        attributeSetter.setStickerID(econItem, 0, gameItemStorage.getGraffitiKit(item).id);
        if (const auto graffiti = get<inventory::Graffiti>(inventoryItem); graffiti && graffiti->usesLeft >= 0) {
            econItemPOD->weaponId = WeaponId::Graffiti;
            attributeSetter.setSpraysRemaining(econItem, graffiti->usesLeft);
        }
    } else if (item.isMusic()) {
        attributeSetter.setMusicID(econItem, gameItemStorage.getMusicKit(item).id);
        if (const auto music = get<inventory::Music>(inventoryItem); music && music->statTrak > -1) {
            attributeSetter.setStatTrak(econItem, music->statTrak);
            attributeSetter.setStatTrakType(econItem, 1);
            econItemPOD->quality = 9;
        }
    } else if (item.isSkin()) {
        initSkinEconItem(memory, gameItemStorage, inventoryItem, econItem);
    } else if (item.isGloves()) {
        econItemPOD->quality = 3;
        attributeSetter.setPaintKit(econItem, static_cast<float>(gameItemStorage.getPaintKit(item).id));

        if (const auto glove = get<inventory::Gloves>(inventoryItem)) {
            attributeSetter.setWear(econItem, glove->wear);
            attributeSetter.setSeed(econItem, static_cast<float>(glove->seed));
        }
    } else if (item.isCollectible()) {
        if (gameItemStorage.isCollectibleGenuine(item))
            econItemPOD->quality = 1;
    } else if (item.isAgent()) {
        if (const auto agent = get<inventory::Agent>(inventoryItem)) {
            for (std::size_t j = 0; j < agent->patches.size(); ++j) {
                const auto& patch = agent->patches[j];
                if (patch.patchID == 0)
                    continue;

                attributeSetter.setStickerID(econItem, j, patch.patchID);
            }
        }
    } else if (item.isServiceMedal()) {
        if (const auto serviceMedal = get<inventory::ServiceMedal>(inventoryItem); serviceMedal && serviceMedal->issueDateTimestamp != 0)
            attributeSetter.setIssueDate(econItem, serviceMedal->issueDateTimestamp);
    } else if (item.isTournamentCoin()) {
        if (const auto tournamentCoin = get<inventory::TournamentCoin>(inventoryItem))
            attributeSetter.setDropsAwarded(econItem, tournamentCoin->dropsAwarded);
        attributeSetter.setDropsRedeemed(econItem, 0);
        attributeSetter.setStickerID(econItem, 0, gameItemStorage.getDefaultTournamentGraffitiID(item));
        attributeSetter.setCampaignCompletion(econItem, 1);
    } else if (item.isCrate()) {
        if (const auto souvenirPackage = get<inventory::SouvenirPackage>(inventoryItem); souvenirPackage && souvenirPackage->tournamentStage != csgo::TournamentStage{}) {
            attributeSetter.setTournamentStage(econItem, static_cast<int>(souvenirPackage->tournamentStage));
            attributeSetter.setTournamentTeam1(econItem, static_cast<int>(souvenirPackage->tournamentTeam1));
            attributeSetter.setTournamentTeam2(econItem, static_cast<int>(souvenirPackage->tournamentTeam2));
            if (souvenirPackage->proPlayer != csgo::ProPlayer{})
                attributeSetter.setTournamentPlayer(econItem, static_cast<int>(souvenirPackage->proPlayer));
        }
    } else if (item.isStorageUnit()) {
        if (const auto storageUnit = get<inventory::StorageUnit>(inventoryItem); storageUnit && storageUnit->modificationDateTimestamp != 0) {
            attributeSetter.setModificationDate(econItem, storageUnit->modificationDateTimestamp);
            econItem.setCustomName(storageUnit->name.c_str());
        }
    }

    csgo::SharedObjectTypeCache::from(retSpoofGadgets->client, baseTypeCache).addObject((csgo::SharedObjectPOD*)econItemPOD);
    localInventory.soCreated(localInventory.getSOID(), (csgo::SharedObjectPOD*)econItemPOD, 4);

    if (const auto inventoryComponent = csgo::UiComponentInventory::from(retSpoofGadgets->client, *uiComponentInventory, setItemSessionPropertyValue); inventoryComponent.getPOD() != nullptr) {
        inventoryComponent.setItemSessionPropertyValue(econItemPOD->itemID, "recent", "0");
        inventoryComponent.setItemSessionPropertyValue(econItemPOD->itemID, "updated", "0");
    }

    if (const auto view = csgo::EconItemView::from(retSpoofGadgets->client, memory.findOrCreateEconItemViewForItemID(econItemPOD->itemID), econItemViewFunctions); view.getPOD() != nullptr)
        view.clearInventoryImageRGBA();

    return ItemId{ econItemPOD->itemID };
}

ItemId Inventory::assingNewItemID(ItemId itemID)
{
    const auto econItem = getEconItem(memory, econItemViewFunctions, itemID);
    if (!econItem)
        return itemID;

    const auto localInventory = csgo::CSPlayerInventory::from(retSpoofGadgets->client, memory.inventoryManager.getLocalInventory());
    if (localInventory.getPOD() == nullptr)
        return itemID;

    const auto baseTypeCache = getItemBaseTypeCache(localInventory, memory.createBaseTypeCache);
    if (!baseTypeCache)
        return itemID;

    localInventory.soDestroyed(localInventory.getSOID(), (csgo::SharedObjectPOD*)econItem, 4);
    const auto newItemID = baseTypeCache->getHighestIDs().first + 1;
    econItem->itemID = newItemID;
    localInventory.soCreated(localInventory.getSOID(), (csgo::SharedObjectPOD*)econItem, 4);

    if (const auto view = csgo::EconItemView::from(retSpoofGadgets->client, memory.findOrCreateEconItemViewForItemID(newItemID), econItemViewFunctions); view.getPOD() != nullptr)
        view.clearInventoryImageRGBA();

    if (const auto inventoryComponent = csgo::UiComponentInventory::from(retSpoofGadgets->client, *uiComponentInventory, setItemSessionPropertyValue); inventoryComponent.getPOD() != nullptr) {
        inventoryComponent.setItemSessionPropertyValue(newItemID, "recent", "0");
        inventoryComponent.setItemSessionPropertyValue(newItemID, "updated", "0");
    }

    return ItemId{ newItemID };
}

void Inventory::applySticker(ItemId itemID, csgo::StickerId stickerID, std::uint8_t slot)
{
    auto econItem = csgo::EconItem::from(retSpoofGadgets->client, getEconItem(memory, econItemViewFunctions, itemID), econItemFunctions);
    if (econItem.getPOD() == nullptr)
        return;

    const auto localInventory = csgo::CSPlayerInventory::from(retSpoofGadgets->client, memory.inventoryManager.getLocalInventory());
    if (localInventory.getPOD() == nullptr)
        return;

    csgo::EconItemAttributeSetter attributeSetter{ csgo::ItemSchema::from(retSpoofGadgets->client, memory.itemSystem().getItemSchema()) };
    attributeSetter.setStickerID(econItem, slot, static_cast<int>(stickerID));
    attributeSetter.setStickerWear(econItem, slot, 0.0f);

    localInventory.soUpdated(localInventory.getSOID(), (csgo::SharedObjectPOD*)econItem.getPOD(), 4);
    initItemCustomizationNotification(interfaces, memory, "sticker_apply", itemID);
}

void Inventory::removeSticker(ItemId itemID, std::uint8_t slot)
{
    auto econItem = csgo::EconItem::from(retSpoofGadgets->client, getEconItem(memory, econItemViewFunctions, itemID), econItemFunctions);
    if (econItem.getPOD() == nullptr)
        return;

    const auto localInventory = csgo::CSPlayerInventory::from(retSpoofGadgets->client, memory.inventoryManager.getLocalInventory());
    if (localInventory.getPOD() == nullptr)
        return;

    csgo::EconItemAttributeSetter attributeSetter{ csgo::ItemSchema::from(retSpoofGadgets->client, memory.itemSystem().getItemSchema()) };
    attributeSetter.setStickerID(econItem, slot, 0);
    attributeSetter.setStickerWear(econItem, slot, 0.0f);
    localInventory.soUpdated(localInventory.getSOID(), (csgo::SharedObjectPOD*)econItem.getPOD(), 4);
    initItemCustomizationNotification(interfaces, memory, "sticker_remove", itemID);
}

void Inventory::updateStickerWear(ItemId itemID, std::uint8_t slot, float newWear)
{
    auto econItem = csgo::EconItem::from(retSpoofGadgets->client, getEconItem(memory, econItemViewFunctions, itemID), econItemFunctions);
    if (econItem.getPOD() == nullptr)
        return;

    const auto localInventory = csgo::CSPlayerInventory::from(retSpoofGadgets->client, memory.inventoryManager.getLocalInventory());
    if (localInventory.getPOD() == nullptr)
        return;

    csgo::EconItemAttributeSetter attributeSetter{ csgo::ItemSchema::from(retSpoofGadgets->client, memory.itemSystem().getItemSchema()) };
    attributeSetter.setStickerWear(econItem, slot, newWear);
    localInventory.soUpdated(localInventory.getSOID(), (csgo::SharedObjectPOD*)econItem.getPOD(), 4);
}

void Inventory::viewerPassActivated(ItemId tournamentCoinItemID)
{
    initItemCustomizationNotification(interfaces, memory, "ticket_activated", tournamentCoinItemID);
}

void Inventory::addNameTag(ItemId itemID, const char* newNameTag)
{
    updateNameTag(memory, itemID, newNameTag, econItemFunctions, econItemViewFunctions);
    initItemCustomizationNotification(interfaces, memory, "nametag_add", itemID);
}

void Inventory::removeNameTag(ItemId itemID)
{
    updateNameTag(memory, itemID, "", econItemFunctions, econItemViewFunctions);
}

void Inventory::deleteItem(ItemId itemID)
{
    auto econItem = csgo::EconItem::from(retSpoofGadgets->client, getEconItem(memory, econItemViewFunctions, itemID), econItemFunctions);
    if (econItem.getPOD() == nullptr)
        return;

    const auto localInventory = csgo::CSPlayerInventory::from(retSpoofGadgets->client, memory.inventoryManager.getLocalInventory());
    if (localInventory.getPOD() == nullptr)
        return;

    localInventory.soDestroyed(localInventory.getSOID(), (csgo::SharedObjectPOD*)econItem.getPOD(), 4);

    if (const auto baseTypeCache = getItemBaseTypeCache(localInventory, memory.createBaseTypeCache))
        csgo::SharedObjectTypeCache::from(retSpoofGadgets->client, baseTypeCache).removeObject((csgo::SharedObjectPOD*)econItem.getPOD());

    econItem.destructor();
}

void Inventory::updateStatTrak(ItemId itemID, int newStatTrakValue)
{
    auto econItem = csgo::EconItem::from(retSpoofGadgets->client, getEconItem(memory, econItemViewFunctions, itemID), econItemFunctions);
    if (econItem.getPOD() == nullptr)
        return;

    const auto localInventory = csgo::CSPlayerInventory::from(retSpoofGadgets->client, memory.inventoryManager.getLocalInventory());
    if (localInventory.getPOD() == nullptr)
        return;

    csgo::EconItemAttributeSetter attributeSetter{ csgo::ItemSchema::from(retSpoofGadgets->client, memory.itemSystem().getItemSchema()) };
    attributeSetter.setStatTrak(econItem, newStatTrakValue);
    localInventory.soUpdated(localInventory.getSOID(), (csgo::SharedObjectPOD*)econItem.getPOD(), 4);
}

void Inventory::containerOpened(ItemId unlockedItemID)
{
    initItemCustomizationNotification(interfaces, memory, "crate_unlock", unlockedItemID);
}

void Inventory::applyPatch(ItemId itemID, int patchID, std::uint8_t slot)
{
    updatePatch(memory, itemID, patchID, slot, econItemFunctions, econItemViewFunctions);
    initItemCustomizationNotification(interfaces, memory, "patch_apply", itemID);
}

void Inventory::removePatch(ItemId itemID, std::uint8_t slot)
{
    updatePatch(memory, itemID, 0, slot, econItemFunctions, econItemViewFunctions);
    initItemCustomizationNotification(interfaces, memory, "patch_remove", itemID);
}

void Inventory::souvenirTokenActivated(ItemId itemID, std::uint32_t dropsAwarded)
{
    auto econItem = csgo::EconItem::from(retSpoofGadgets->client, getEconItem(memory, econItemViewFunctions, itemID), econItemFunctions);
    if (econItem.getPOD() == nullptr)
        return;

    const auto localInventory = csgo::CSPlayerInventory::from(retSpoofGadgets->client, memory.inventoryManager.getLocalInventory());
    if (localInventory.getPOD() == nullptr)
        return;

    csgo::EconItemAttributeSetter attributeSetter{ csgo::ItemSchema::from(retSpoofGadgets->client, memory.itemSystem().getItemSchema()) };
    attributeSetter.setDropsAwarded(econItem, dropsAwarded);
    localInventory.soUpdated(localInventory.getSOID(), (csgo::SharedObjectPOD*)econItem.getPOD(), 4);
    initItemCustomizationNotification(interfaces, memory, "ticket_activated", itemID);
}

void Inventory::unsealGraffiti(ItemId itemID)
{
    auto econItem = csgo::EconItem::from(retSpoofGadgets->client, getEconItem(memory, econItemViewFunctions, itemID), econItemFunctions);
    if (econItem.getPOD() == nullptr || econItem.getPOD()->weaponId != WeaponId::SealedGraffiti)
        return;

    const auto localInventory = csgo::CSPlayerInventory::from(retSpoofGadgets->client, memory.inventoryManager.getLocalInventory());
    if (localInventory.getPOD() == nullptr)
        return;

    csgo::EconItemAttributeSetter attributeSetter{ csgo::ItemSchema::from(retSpoofGadgets->client, memory.itemSystem().getItemSchema()) };
    attributeSetter.setSpraysRemaining(econItem, 50);
    econItem.getPOD()->weaponId = WeaponId::Graffiti;
    localInventory.soUpdated(localInventory.getSOID(), (csgo::SharedObjectPOD*)econItem.getPOD(), 4);
    initItemCustomizationNotification(interfaces, memory, "graffity_unseal", itemID);
}

void Inventory::selectTeamGraffiti(ItemId itemID, std::uint16_t graffitiID)
{
    auto econItem = csgo::EconItem::from(retSpoofGadgets->client, getEconItem(memory, econItemViewFunctions, itemID), econItemFunctions);
    if (econItem.getPOD() == nullptr)
        return;

    const auto localInventory = csgo::CSPlayerInventory::from(retSpoofGadgets->client, memory.inventoryManager.getLocalInventory());
    if (localInventory.getPOD() == nullptr)
        return;

    csgo::EconItemAttributeSetter attributeSetter{ csgo::ItemSchema::from(retSpoofGadgets->client, memory.itemSystem().getItemSchema()) };
    attributeSetter.setStickerID(econItem, 0, graffitiID);
    localInventory.soUpdated(localInventory.getSOID(), (csgo::SharedObjectPOD*)econItem.getPOD(), 4);
}

void Inventory::statTrakSwapped(ItemId itemID)
{
    initItemCustomizationNotification(interfaces, memory, "stattrack_swap", itemID);
}

void Inventory::equipItem(ItemId itemID, csgo::Team team, std::uint8_t slot)
{
    memory.inventoryManager.equipItemInSlot(team, slot, static_cast<csgo::ItemId>(itemID));
}

void Inventory::markItemUpdated(ItemId itemID)
{
    if (const auto inventoryComponent = csgo::UiComponentInventory::from(retSpoofGadgets->client, *uiComponentInventory, setItemSessionPropertyValue); inventoryComponent.getPOD() != nullptr) {
        inventoryComponent.setItemSessionPropertyValue(static_cast<csgo::ItemId>(itemID), "recent", "0");
        inventoryComponent.setItemSessionPropertyValue(static_cast<csgo::ItemId>(itemID), "updated", "1");
    }
}

void Inventory::pickEmUpdated()
{
    if (const auto idx = memory.registeredPanoramaEvents->find(memory.makePanoramaSymbol("PanoramaComponent_MatchList_PredictionUploaded")); idx != -1) {
        const char* dummy;
        if (const auto eventPtr = retSpoofGadgets->client.invokeCdecl<void*>(std::uintptr_t(memory.registeredPanoramaEvents->memory[idx].value.createEventFromString), nullptr, "", &dummy))
            csgo::UIEngine::from(retSpoofGadgets->client, interfaces.getPanoramaUIEngine().accessUIEngine()).dispatchEvent(eventPtr);
    }
}

void Inventory::hideItem(ItemId itemID)
{
    setItemHiddenFlag(memory, econItemViewFunctions, itemID, true);
}

void Inventory::unhideItem(ItemId itemID)
{
    setItemHiddenFlag(memory, econItemViewFunctions, itemID, false);
}

void Inventory::xRayItemRevealed(ItemId itemID)
{
   initItemCustomizationNotification(interfaces, memory, "xray_item_reveal", itemID);
}

void Inventory::xRayItemClaimed(ItemId itemID)
{
    const auto econItem = getEconItem(memory, econItemViewFunctions, itemID);
    if (!econItem)
        return;

    const auto localInventory = csgo::CSPlayerInventory::from(retSpoofGadgets->client, memory.inventoryManager.getLocalInventory());
    if (localInventory.getPOD() == nullptr)
        return;

    econItem->flags &= ~16;

    localInventory.soUpdated(localInventory.getSOID(), (csgo::SharedObjectPOD*)econItem, 4);

    initItemCustomizationNotification(interfaces, memory, "xray_item_claim", itemID);
}

void Inventory::nameStorageUnit(ItemId itemID, const char* newName)
{
    addNameTag(itemID, newName);
}

void Inventory::storageUnitModified(ItemId itemID, std::uint32_t modificationDate, std::uint32_t itemCount)
{
    auto econItem = csgo::EconItem::from(retSpoofGadgets->client, getEconItem(memory, econItemViewFunctions, itemID), econItemFunctions);
    if (econItem.getPOD() == nullptr)
        return;

    const auto localInventory = csgo::CSPlayerInventory::from(retSpoofGadgets->client, memory.inventoryManager.getLocalInventory());
    if (localInventory.getPOD() == nullptr)
        return;

    csgo::EconItemAttributeSetter attributeSetter{ csgo::ItemSchema::from(retSpoofGadgets->client, memory.itemSystem().getItemSchema()) };
    attributeSetter.setModificationDate(econItem, modificationDate);
    attributeSetter.setItemsCount(econItem, itemCount);

    localInventory.soUpdated(localInventory.getSOID(), (csgo::SharedObjectPOD*)econItem.getPOD(), 4);
}

void Inventory::addItemToStorageUnit(ItemId itemID, ItemId storageUnitItemID)
{
    auto econItem = csgo::EconItem::from(retSpoofGadgets->client, getEconItem(memory, econItemViewFunctions, itemID), econItemFunctions);
    if (econItem.getPOD() == nullptr)
        return;

    const auto localInventory = csgo::CSPlayerInventory::from(retSpoofGadgets->client, memory.inventoryManager.getLocalInventory());
    if (localInventory.getPOD() == nullptr)
        return;

    csgo::EconItemAttributeSetter attributeSetter{ csgo::ItemSchema::from(retSpoofGadgets->client, memory.itemSystem().getItemSchema()) };
    attributeSetter.setCasketItemIdLow(econItem, static_cast<std::uint32_t>(static_cast<csgo::ItemId>(storageUnitItemID) & 0xFFFFFFFF));
    attributeSetter.setCasketItemIdHigh(econItem, static_cast<std::uint32_t>((static_cast<csgo::ItemId>(storageUnitItemID) >> 32) & 0xFFFFFFFF));

    localInventory.soUpdated(localInventory.getSOID(), (csgo::SharedObjectPOD*)econItem.getPOD(), 4);
}

void Inventory::itemAddedToStorageUnit(ItemId storageUnitItemID)
{
    initItemCustomizationNotification(interfaces, memory, "casket_added", storageUnitItemID);
}

void Inventory::removeItemFromStorageUnit(ItemId itemID, ItemId storageUnitItemID)
{
    auto econItem = csgo::EconItem::from(retSpoofGadgets->client, getEconItem(memory, econItemViewFunctions, itemID), econItemFunctions);
    if (econItem.getPOD() == nullptr)
        return;

    const auto localInventory = csgo::CSPlayerInventory::from(retSpoofGadgets->client, memory.inventoryManager.getLocalInventory());
    if (localInventory.getPOD() == nullptr)
        return;

    csgo::EconItemAttributeSetter attributeSetter{ csgo::ItemSchema::from(retSpoofGadgets->client, memory.itemSystem().getItemSchema()) };
    attributeSetter.removeCasketItemId(econItem);

    localInventory.soUpdated(localInventory.getSOID(), (csgo::SharedObjectPOD*)econItem.getPOD(), 4);
    initItemCustomizationNotification(interfaces, memory, "casket_removed", storageUnitItemID);
}

void Inventory::updateTradableAfterDate(ItemId itemID, std::uint32_t tradableAfterDate)
{
    auto econItem = csgo::EconItem::from(retSpoofGadgets->client, getEconItem(memory, econItemViewFunctions, itemID), econItemFunctions);
    if (econItem.getPOD() == nullptr)
        return;

    const auto localInventory = csgo::CSPlayerInventory::from(retSpoofGadgets->client, memory.inventoryManager.getLocalInventory());
    if (localInventory.getPOD() == nullptr)
        return;

    csgo::EconItemAttributeSetter attributeSetter{ csgo::ItemSchema::from(retSpoofGadgets->client, memory.itemSystem().getItemSchema()) };
    attributeSetter.setTradableAfterDate(econItem, tradableAfterDate);

    localInventory.soUpdated(localInventory.getSOID(), (csgo::SharedObjectPOD*)econItem.getPOD(), 4);
}

void Inventory::storageUnitContentsLoaded(ItemId storageUnitItemID)
{
    initItemCustomizationNotification(interfaces, memory, "casket_contents", storageUnitItemID);
}

}
