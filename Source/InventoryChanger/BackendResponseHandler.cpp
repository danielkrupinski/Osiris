#include "BackendResponseHandler.h"

#include "StaticData.h"
#include "GameItems/Lookup.h"
#include <SDK/EconItemView.h>
#include <SDK/Panorama.h>
#include <Interfaces.h>

namespace
{

void initSkinEconItem(const inventory::Item& inventoryItem, EconItem& econItem) noexcept
{
    EconItemAttributeSetter attributeSetter{ *memory->itemSystem()->getItemSchema() };

    const auto paintKit = StaticData::lookup().getStorage().getPaintKit(inventoryItem.gameItem()).id;
    attributeSetter.setPaintKit(econItem, static_cast<float>(paintKit));

    const auto skin = inventoryItem.get<inventory::Skin>();
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
        if (dynamicData.tournamentID != 0)
            attributeSetter.setTournamentID(econItem, dynamicData.tournamentID);

        if (dynamicData.tournamentStage != TournamentStage{ 0 }) {
            attributeSetter.setTournamentStage(econItem, static_cast<int>(dynamicData.tournamentStage));
            attributeSetter.setTournamentTeam1(econItem, static_cast<int>(dynamicData.tournamentTeam1));
            attributeSetter.setTournamentTeam2(econItem, static_cast<int>(dynamicData.tournamentTeam2));
            if (dynamicData.proPlayer != static_cast<ProPlayer>(0))
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

std::uint64_t createSOCItem(const inventory::Item& inventoryItem, bool asUnacknowledged)
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
    econItem->quality = 4;
    econItem->weaponId = item.getWeaponID();

    const auto& storage = StaticData::lookup().getStorage();

    EconItemAttributeSetter attributeSetter{ *memory->itemSystem()->getItemSchema() };

    if (item.isSticker()) {
        attributeSetter.setStickerID(*econItem, 0, storage.getStickerKit(item).id);
    } else if (item.isPatch()) {
        attributeSetter.setStickerID(*econItem, 0, storage.getPatch(item).id);
    } else if (item.isGraffiti()) {
        attributeSetter.setStickerID(*econItem, 0, storage.getGraffitiKit(item).id);
        if (const auto graffiti = inventoryItem.get<inventory::Graffiti>(); graffiti && graffiti->usesLeft >= 0) {
            econItem->weaponId = WeaponId::Graffiti;
            attributeSetter.setSpraysRemaining(*econItem, graffiti->usesLeft);
        }
    } else if (item.isMusic()) {
        attributeSetter.setMusicID(*econItem, storage.getMusicKit(item).id);
        if (const auto music = inventoryItem.get<inventory::Music>(); music && music->statTrak > -1) {
            attributeSetter.setStatTrak(*econItem, music->statTrak);
            attributeSetter.setStatTrakType(*econItem, 1);
            econItem->quality = 9;
        }
    } else if (item.isSkin()) {
        initSkinEconItem(inventoryItem, *econItem);
    } else if (item.isGloves()) {
        econItem->quality = 3;
        attributeSetter.setPaintKit(*econItem, static_cast<float>(storage.getPaintKit(item).id));

        if (const auto glove = inventoryItem.get<inventory::Glove>()) {
            attributeSetter.setWear(*econItem, glove->wear);
            attributeSetter.setSeed(*econItem, static_cast<float>(glove->seed));
        }
    } else if (item.isCollectible()) {
        if (storage.isCollectibleGenuine(item))
            econItem->quality = 1;
    } else if (item.isAgent()) {
        if (const auto agent = inventoryItem.get<inventory::Agent>()) {
            for (std::size_t j = 0; j < agent->patches.size(); ++j) {
                const auto& patch = agent->patches[j];
                if (patch.patchID == 0)
                    continue;

                attributeSetter.setStickerID(*econItem, j, patch.patchID);
            }
        }
    } else if (item.isServiceMedal()) {
        if (const auto serviceMedal = inventoryItem.get<inventory::ServiceMedal>(); serviceMedal && serviceMedal->issueDateTimestamp != 0)
            attributeSetter.setIssueDate(*econItem, serviceMedal->issueDateTimestamp);
    } else if (item.isTournamentCoin()) {
        if (const auto tournamentCoin = inventoryItem.get<inventory::TournamentCoin>())
            attributeSetter.setDropsAwarded(*econItem, tournamentCoin->dropsAwarded);
        attributeSetter.setDropsRedeemed(*econItem, 0);
        attributeSetter.setStickerID(*econItem, 0, storage.getDefaultTournamentGraffitiID(item));
        attributeSetter.setCampaignCompletion(*econItem, 1);
    } else if (item.isCase() && StaticData::isSouvenirPackage(item)) {
        if (const auto souvenirPackage = inventoryItem.get<inventory::SouvenirPackage>(); souvenirPackage && souvenirPackage->tournamentStage != TournamentStage{ 0 }) {
            attributeSetter.setTournamentStage(*econItem, static_cast<int>(souvenirPackage->tournamentStage));
            attributeSetter.setTournamentTeam1(*econItem, static_cast<int>(souvenirPackage->tournamentTeam1));
            attributeSetter.setTournamentTeam2(*econItem, static_cast<int>(souvenirPackage->tournamentTeam2));
            if (souvenirPackage->proPlayer != static_cast<ProPlayer>(0))
                attributeSetter.setTournamentPlayer(*econItem, static_cast<int>(souvenirPackage->proPlayer));
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

[[nodiscard]] std::uint64_t assingNewItemID(std::uint64_t itemID)
{
    const auto view = memory->findOrCreateEconItemViewForItemID(itemID);
    if (!view)
        return itemID;

    const auto econItem = memory->getSOCData(view);
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

    return newItemID;
}

static void initItemCustomizationNotification(std::string_view typeStr, std::uint64_t itemID)
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

void applySticker(std::uint64_t itemID, int stickerID, std::uint8_t slot)
{
    const auto view = memory->findOrCreateEconItemViewForItemID(itemID);
    if (!view)
        return;

    const auto econItem = memory->getSOCData(view);
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

void removeSticker(std::uint64_t itemID, std::uint8_t slot)
{
    const auto view = memory->findOrCreateEconItemViewForItemID(itemID);
    if (!view)
        return;

    const auto econItem = memory->getSOCData(view);
    if (!econItem)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    EconItemAttributeSetter attributeSetter{ *memory->itemSystem()->getItemSchema() };
    attributeSetter.setStickerID(*econItem, slot, 0);
    attributeSetter.setStickerWear(*econItem, slot, 0.0f);
    localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);
}

void updateStickerWear(std::uint64_t itemID, std::uint8_t slot, float newWear)
{
    const auto view = memory->findOrCreateEconItemViewForItemID(itemID);
    if (!view)
        return;

    const auto econItem = memory->getSOCData(view);
    if (!econItem)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    EconItemAttributeSetter attributeSetter{ *memory->itemSystem()->getItemSchema() };
    attributeSetter.setStickerWear(*econItem, slot, newWear);
    localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);
}

void updateNameTag(std::uint64_t itemID, const char* newNameTag)
{
    const auto view = memory->findOrCreateEconItemViewForItemID(itemID);
    if (!view)
        return;

    const auto econItem = memory->getSOCData(view);
    if (!econItem)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    memory->setCustomName(econItem, newNameTag);
    localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);
}

void deleteItem(std::uint64_t itemID)
{
    const auto view = memory->findOrCreateEconItemViewForItemID(itemID);
    if (!view)
        return;

    const auto econItem = memory->getSOCData(view);
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

void updateStatTrak(std::uint64_t itemID, int newStatTrakValue)
{
    const auto view = memory->findOrCreateEconItemViewForItemID(itemID);
    if (!view)
        return;

    const auto econItem = memory->getSOCData(view);
    if (!econItem)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    EconItemAttributeSetter attributeSetter{ *memory->itemSystem()->getItemSchema() };
    attributeSetter.setStatTrak(*econItem, newStatTrakValue);
    localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);
}

void updatePatch(std::uint64_t itemID, int patchID, std::uint8_t slot)
{
    const auto view = memory->findOrCreateEconItemViewForItemID(itemID);
    if (!view)
        return;

    const auto econItem = memory->getSOCData(view);
    if (!econItem)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    EconItemAttributeSetter attributeSetter{ *memory->itemSystem()->getItemSchema() };
    attributeSetter.setStickerID(*econItem, slot, patchID);
    localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);
}

void applyPatch(std::uint64_t itemID, int patchID, std::uint8_t slot)
{
    updatePatch(itemID, patchID, slot);
}

void removePatch(std::uint64_t itemID, std::uint8_t slot)
{
    updatePatch(itemID, 0, slot);
}

void updateSouvenirDropsAwarded(std::uint64_t itemID, std::uint32_t dropsAwarded)
{
    const auto view = memory->findOrCreateEconItemViewForItemID(itemID);
    if (!view)
        return;

    const auto econItem = memory->getSOCData(view);
    if (!econItem)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    EconItemAttributeSetter attributeSetter{ *memory->itemSystem()->getItemSchema() };
    attributeSetter.setDropsAwarded(*econItem, dropsAwarded);
    localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);
}

void graffitiUnseal(std::uint64_t itemID)
{
    const auto view = memory->findOrCreateEconItemViewForItemID(itemID);
    if (!view)
        return;

    const auto econItem = memory->getSOCData(view);
    if (!econItem || econItem->weaponId != WeaponId::SealedGraffiti)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    EconItemAttributeSetter attributeSetter{ *memory->itemSystem()->getItemSchema() };
    attributeSetter.setSpraysRemaining(*econItem, 50);
    econItem->weaponId = WeaponId::Graffiti;
    localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);
}

void selectTeamGraffiti(std::uint64_t itemID, std::uint16_t graffitiID)
{
    const auto view = memory->findOrCreateEconItemViewForItemID(itemID);
    if (!view)
        return;

    const auto econItem = memory->getSOCData(view);
    if (!econItem)
        return;

    const auto localInventory = memory->inventoryManager->getLocalInventory();
    if (!localInventory)
        return;

    EconItemAttributeSetter attributeSetter{ *memory->itemSystem()->getItemSchema() };
    attributeSetter.setStickerID(*econItem, 0, graffitiID);
    localInventory->soUpdated(localInventory->getSOID(), (SharedObject*)econItem, 4);
}

void equipItem(std::uint64_t itemID, Team team, std::uint8_t slot)
{
    memory->inventoryManager->equipItemInSlot(team, slot, itemID);
}

}

namespace inventory_changer
{

void BackendResponseHandler::operator()(const backend::response::ItemAdded& response) const
{
    const auto itemID = createSOCItem(*response.item, response.asUnacknowledged);
    backend.assignItemID(response.item, itemID);
}

void BackendResponseHandler::operator()(const backend::response::ItemMovedToFront& response) const
{
    if (const auto itemID = backend.getItemID(response.item); itemID.has_value())
        backend.updateItemID(*itemID, assingNewItemID(*itemID));
}

void BackendResponseHandler::operator()(const backend::response::ItemUpdated& response) const
{
    if (const auto itemID = backend.getItemID(response.item); itemID.has_value()) {
        if (const auto inventoryComponent = *memory->uiComponentInventory)
            memory->setItemSessionPropertyValue(inventoryComponent, *itemID, "updated", "1");
    }
}

void BackendResponseHandler::operator()(const backend::response::ItemEquipped& response) const
{
    if (const auto itemID = backend.getItemID(response.item); itemID.has_value()) {
        equipItem(*itemID, response.team, response.slot);
    }
}

void BackendResponseHandler::operator()(const backend::response::ItemRemoved& response) const
{
    deleteItem(response.itemID);
}

void BackendResponseHandler::operator()(const backend::response::StickerApplied& response) const
{
    if (const auto itemID = backend.getItemID(response.skinItem); itemID.has_value()) {
        if (const auto skin = response.skinItem->get<inventory::Skin>())
            applySticker(*itemID, skin->stickers[response.stickerSlot].stickerID, response.stickerSlot);
    }
}

void BackendResponseHandler::operator()(const backend::response::StickerScraped& response) const
{
    if (const auto itemID = backend.getItemID(response.skinItem); itemID.has_value()) {
        if (const auto skin = response.skinItem->get<inventory::Skin>())
            updateStickerWear(*itemID, response.stickerSlot, skin->stickers[response.stickerSlot].wear);
    }
}

void BackendResponseHandler::operator()(const backend::response::StickerRemoved& response) const
{
    if (const auto itemID = backend.getItemID(response.skinItem); itemID.has_value()) {
        removeSticker(*itemID, response.stickerSlot);
        initItemCustomizationNotification("sticker_remove", *itemID);
    }
}

void BackendResponseHandler::operator()(const backend::response::StatTrakUpdated& response) const
{
    updateStatTrak(response.itemID, response.newStatTrakValue);
}

void BackendResponseHandler::operator()(const backend::response::ViewerPassActivated& response) const
{
    if (const auto itemID = backend.getItemID(response.createdEventCoin); itemID.has_value())
        initItemCustomizationNotification("ticket_activated", *itemID);
}

void BackendResponseHandler::operator()(const backend::response::NameTagAdded& response) const
{
    if (const auto itemID = backend.getItemID(response.skinItem); itemID.has_value()) {
        if (const auto skin = response.skinItem->get<inventory::Skin>()) {
            updateNameTag(*itemID, skin->nameTag.c_str());
            initItemCustomizationNotification("nametag_add", *itemID);
        }
    }
}

void BackendResponseHandler::operator()(const backend::response::NameTagRemoved& response) const
{
    if (const auto itemID = backend.getItemID(response.skinItem); itemID.has_value())
        updateNameTag(*itemID, "");
}

void BackendResponseHandler::operator()(const backend::response::ContainerOpened& response) const
{
    if (const auto itemID = backend.getItemID(response.receivedItem); itemID.has_value())
        initItemCustomizationNotification("crate_unlock", *itemID);
}

void BackendResponseHandler::operator()(const backend::response::PatchApplied& response) const
{
    if (const auto itemID = backend.getItemID(response.agentItem); itemID.has_value()) {
        if (const auto agent = response.agentItem->get<inventory::Agent>()) {
            applyPatch(*itemID, agent->patches[response.patchSlot].patchID, response.patchSlot);
            initItemCustomizationNotification("patch_apply", *itemID);
        }
    }
}

void BackendResponseHandler::operator()(const backend::response::PatchRemoved& response) const
{
    if (const auto itemID = backend.getItemID(response.agentItem); itemID.has_value()) {
        removePatch(*itemID, response.patchSlot);
        initItemCustomizationNotification("patch_remove", *itemID);
    }
}

void BackendResponseHandler::operator()(const backend::response::SouvenirTokenActivated& response) const
{
    if (const auto itemID = backend.getItemID(response.tournamentCoin); itemID.has_value()) {
        if (const auto tournamentCoin = response.tournamentCoin->get<inventory::TournamentCoin>()) {
            updateSouvenirDropsAwarded(*itemID, tournamentCoin->dropsAwarded);
            initItemCustomizationNotification("ticket_activated", *itemID);
        }
    }
}

void BackendResponseHandler::operator()(const backend::response::GraffitiUnsealed& response) const
{
    if (const auto itemID = backend.getItemID(response.graffitiItem); itemID.has_value()) {
        graffitiUnseal(*itemID);
        initItemCustomizationNotification("graffity_unseal", *itemID);
    }
}

void BackendResponseHandler::operator()(const backend::response::StatTrakSwapped& response) const
{
    const auto sourceItemID = backend.getItemID(response.swapSourceItem);
    if (!sourceItemID.has_value())
        return;

    const auto destinationItemID = backend.getItemID(response.swapDestinationItem);
    if (!destinationItemID.has_value())
        return;

    const auto sourceStatTrak = inventory::getStatTrak(*response.swapSourceItem);
    if (!sourceStatTrak)
        return;

    const auto destinationStatTrak = inventory::getStatTrak(*response.swapDestinationItem);
    if (!destinationStatTrak)
        return;

    updateStatTrak(*sourceItemID, *sourceStatTrak);
    updateStatTrak(*destinationItemID, *destinationStatTrak);

    if (const auto inventoryComponent = *memory->uiComponentInventory) {
        memory->setItemSessionPropertyValue(inventoryComponent, *destinationStatTrak >= *sourceStatTrak ? *sourceItemID : *destinationItemID, "updated", "1");
    }

    initItemCustomizationNotification("stattrack_swap", *sourceStatTrak > *destinationStatTrak ? *sourceItemID : *destinationItemID);
}

void BackendResponseHandler::operator()(const backend::response::TeamGraffitiSelected& response) const
{
    if (const auto itemID = backend.getItemID(response.tournamentCoin); itemID.has_value()) {
        selectTeamGraffiti(*itemID, response.graffitiID);
    }
}

}
