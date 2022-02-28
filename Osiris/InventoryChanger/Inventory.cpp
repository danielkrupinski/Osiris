#include <algorithm>
#include <cmath>
#include <utility>

#include "../Helpers.h"
#include "Inventory.h"
#include "ItemGenerator.h"
#include "../Memory.h"
#include "../SDK/EconItemView.h"
#include "../SDK/Entity.h"
#include "../SDK/ItemSchema.h"

using Inventory::InvalidDynamicDataIdx;
using Inventory::BASE_ITEMID;

static std::vector<DynamicSkinData> dynamicSkinData;
static std::vector<DynamicGloveData> dynamicGloveData;
static std::vector<DynamicAgentData> dynamicAgentData;
static std::vector<DynamicMusicData> dynamicMusicData;
static std::vector<DynamicSouvenirPackageData> dynamicSouvenirPackageData;
static std::vector<DynamicServiceMedalData> dynamicServiceMedalData;
static std::vector<DynamicTournamentCoinData> dynamicTournamentCoinData;

class InventoryImpl {
public:
    struct ToEquip {
        ToEquip(Team team, int slot, std::size_t index) : team{ team }, slot{ slot }, index{ index } {}

        Team team;
        int slot;
        std::size_t index;
    };

    static std::vector<InventoryItem>& get() noexcept
    {
        return instance().inventory;
    }

    static void addItem(const game_items::Item& gameItem, std::size_t dynamicDataIdx, bool asUnacknowledged) noexcept
    {
        instance().toAdd.emplace_back(gameItem, dynamicDataIdx, asUnacknowledged);
    }

    static std::uint64_t addItemNow(const game_items::Item& gameItem, std::size_t dynamicDataIdx, bool asUnacknowledged) noexcept
    {
        return instance()._addItem(gameItem, dynamicDataIdx, asUnacknowledged);
    }

    static void deleteItemNow(std::uint64_t itemID) noexcept
    {
        instance()._deleteItem(itemID);
    }

    static void runFrame() noexcept
    {
        instance()._runFrame();
    }

    static InventoryItem* getItem(std::uint64_t itemID) noexcept
    {
        return instance()._getItem(itemID);
    }

    static std::uint64_t recreateItem(std::uint64_t itemID) noexcept
    {
        return instance()._recreateItem(itemID);
    }

    static void clear() noexcept
    {
        instance()._clear();
    }

    static void equipItem(Team team, int slot, std::size_t index) noexcept
    {
        instance().toEquip.emplace_back(team, slot, index);
    }

    static std::size_t getItemIndex(std::uint64_t itemID) noexcept
    {
        return instance()._getItemIndex(itemID);
    }
private:
    InventoryItem* _getItem(std::uint64_t itemID) noexcept
    {
        if (itemID >= BASE_ITEMID && static_cast<std::size_t>(itemID - BASE_ITEMID) < inventory.size())
            return &inventory[static_cast<std::size_t>(itemID - BASE_ITEMID)];
        return nullptr;
    }

    std::size_t _getItemIndex(std::uint64_t itemID) noexcept
    {
        assert(_getItem(itemID) != nullptr);
        return static_cast<std::size_t>(itemID - BASE_ITEMID - std::count_if(inventory.begin(), inventory.begin() + static_cast<std::size_t>(itemID - BASE_ITEMID), [](const auto& item) { return item.isDeleted(); }));
    }

    static void initSkinEconItem(const InventoryItem& inventoryItem, EconItem& econItem) noexcept
    {
        assert(inventoryItem.isSkin());

        const auto paintKit = StaticData::getPaintKit(inventoryItem.get()).id;
        econItem.setPaintKit(static_cast<float>(paintKit));

        const auto& dynamicData = dynamicSkinData[inventoryItem.getDynamicDataIndex()];
        const auto isMP5LabRats = Helpers::isMP5LabRats(inventoryItem.get().getWeaponID(), paintKit);
        if (dynamicData.isSouvenir() || isMP5LabRats) {
            econItem.quality = 12;
        } else {
            if (dynamicData.statTrak > -1) {
                econItem.setStatTrak(dynamicData.statTrak);
                econItem.setStatTrakType(0);
                econItem.quality = 9;
            }
            if (Helpers::isKnife(econItem.weaponId))
                econItem.quality = 3;
        }

        if (isMP5LabRats) {
            econItem.setSpecialEventID(1);
        } else {
            if (dynamicData.tournamentID != 0)
                econItem.setTournamentID(dynamicData.tournamentID);

            if (dynamicData.tournamentStage != TournamentStage{ 0 }) {
                econItem.setTournamentStage(static_cast<int>(dynamicData.tournamentStage));
                econItem.setTournamentTeam1(static_cast<int>(dynamicData.tournamentTeam1));
                econItem.setTournamentTeam2(static_cast<int>(dynamicData.tournamentTeam2));
                if (dynamicData.proPlayer != static_cast<ProPlayer>(0))
                    econItem.setTournamentPlayer(static_cast<int>(dynamicData.proPlayer));
            }
        }

        econItem.setWear(dynamicData.wear);
        econItem.setSeed(static_cast<float>(dynamicData.seed));
        memory->setCustomName(&econItem, dynamicData.nameTag.c_str());

        for (std::size_t j = 0; j < dynamicData.stickers.size(); ++j) {
            const auto& sticker = dynamicData.stickers[j];
            if (sticker.stickerID == 0)
                continue;

            econItem.setStickerID(j, sticker.stickerID);
            econItem.setStickerWear(j, sticker.wear);
        }
    }

    std::uint64_t _createSOCItem(const InventoryItem& inventoryItem, bool asUnacknowledged) const noexcept
    {
        const auto localInventory = memory->inventoryManager->getLocalInventory();
        if (!localInventory)
            return 0;

        const auto baseTypeCache = localInventory->getItemBaseTypeCache();
        if (!baseTypeCache)
            return 0;

        static const auto baseInvID = localInventory->getHighestIDs().second;

        const auto econItem = memory->createEconItemSharedObject();
        econItem->itemID = BASE_ITEMID + inventory.size() - 1;
        econItem->originalID = 0;
        econItem->accountID = localInventory->getAccountID();
        econItem->inventory = asUnacknowledged ? 0 : baseInvID + inventory.size();

        const auto& item = inventoryItem.get();
        econItem->rarity = static_cast<std::uint16_t>(item.getRarity());
        econItem->quality = 4;
        econItem->weaponId = item.getWeaponID();

        if (item.isSticker()) {
            econItem->setStickerID(0, StaticData::getStickerID(item));
        } else if (item.isPatch()) {
            econItem->setStickerID(0, StaticData::getPatchID(item));
        } else if (item.isGraffiti()) {
            econItem->setStickerID(0, StaticData::getGraffitiID(item));
        } else if (item.isSealedGraffiti()) {
            econItem->setStickerID(0, StaticData::getSealedGraffitiID(item));
        } else if (item.isMusic()) {
            econItem->setMusicID(StaticData::getMusicID(item));
            const auto& dynamicData = dynamicMusicData[inventoryItem.getDynamicDataIndex()];
            if (dynamicData.statTrak > -1) {
                econItem->setStatTrak(dynamicData.statTrak);
                econItem->setStatTrakType(1);
                econItem->quality = 9;
            }
        } else if (item.isSkin()) {
            initSkinEconItem(inventoryItem, *econItem);
        } else if (item.isGloves()) {
            econItem->quality = 3;
            econItem->setPaintKit(static_cast<float>(StaticData::getPaintKit(item).id));

            const auto& dynamicData = dynamicGloveData[inventoryItem.getDynamicDataIndex()];
            econItem->setWear(dynamicData.wear);
            econItem->setSeed(static_cast<float>(dynamicData.seed));
        } else if (item.isCollectible()) {
            if (StaticData::isCollectibleGenuine(item))
                econItem->quality = 1;
        } else if (item.isAgent()) {
            const auto& dynamicData = dynamicAgentData[inventoryItem.getDynamicDataIndex()];
            for (std::size_t j = 0; j < dynamicData.patches.size(); ++j) {
                const auto& patch = dynamicData.patches[j];
                if (patch.patchID == 0)
                    continue;

                econItem->setStickerID(j, patch.patchID);
            }
        } else if (item.isServiceMedal()) {
            if (const auto& dynamicData = dynamicServiceMedalData[inventoryItem.getDynamicDataIndex()]; dynamicData.issueDateTimestamp != 0)
                econItem->setIssueDate(dynamicData.issueDateTimestamp);
        } else if (item.isTournamentCoin()) {
            econItem->setDropsAwarded(dynamicTournamentCoinData[inventoryItem.getDynamicDataIndex()].dropsAwarded);
            econItem->setDropsRedeemed(0);
        } else if (item.isCase() && StaticData::getCase(item).isSouvenirPackage()) {
            if (const auto& dynamicData = dynamicSouvenirPackageData[inventoryItem.getDynamicDataIndex()]; dynamicData.tournamentStage != TournamentStage{ 0 }) {
                econItem->setTournamentStage(static_cast<int>(dynamicData.tournamentStage));
                econItem->setTournamentTeam1(static_cast<int>(dynamicData.tournamentTeam1));
                econItem->setTournamentTeam2(static_cast<int>(dynamicData.tournamentTeam2));
                if (dynamicData.proPlayer != static_cast<ProPlayer>(0))
                    econItem->setTournamentPlayer(static_cast<int>(dynamicData.proPlayer));
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

    void _deleteItem(std::uint64_t itemID) noexcept
    {
        const auto item = _getItem(itemID);
        if (!item)
            return;

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
        item->markAsDeleted();
    }

    std::uint64_t _addItem(const game_items::Item& gameItem, std::size_t dynamicDataIdx, bool asUnacknowledged) noexcept
    {
        return _createSOCItem(inventory.emplace_back(gameItem, dynamicDataIdx != InvalidDynamicDataIdx ? dynamicDataIdx : ItemGenerator::createDefaultDynamicData(gameItem)), asUnacknowledged);
    }

    std::uint64_t _recreateItem(std::uint64_t itemID) noexcept
    {
        const auto item = _getItem(itemID);
        if (!item)
            return 0;

        auto itemCopy = *item;

        if (const auto localInventory = memory->inventoryManager->getLocalInventory()) {
            if (const auto view = memory->findOrCreateEconItemViewForItemID(itemID)) {
                if (const auto econItem = memory->getSOCData(view)) {
                    if (const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(econItem->weaponId)) {
                        if (const auto slotCT = def->getLoadoutSlot(Team::CT); localInventory->getItemInLoadout(Team::CT, slotCT) == view)
                            toEquip.emplace_back(Team::CT, slotCT, inventory.size());
                        if (const auto slotTT = def->getLoadoutSlot(Team::TT); localInventory->getItemInLoadout(Team::TT, slotTT) == view)
                            toEquip.emplace_back(Team::TT, slotTT, inventory.size());
                    }
                }
            }
        }

        _deleteItem(itemID);
        return _createSOCItem(inventory.emplace_back(std::move(itemCopy)), false);
    }

    void _addItems() noexcept
    {
        for (const auto [gameItem, dynamicDataIndex, asUnacknowledged] : toAdd)
            _addItem(gameItem, dynamicDataIndex, asUnacknowledged);
        toAdd.clear();
    }

    void _deleteItems() noexcept
    {
        for (std::size_t i = 0; i < inventory.size(); ++i) {
            if (inventory[i].shouldDelete()) {
                _deleteItem(BASE_ITEMID + i);
                inventory[i].markAsDeleted();
            }
        }
    }

    void _clear() noexcept
    {
        for (std::size_t i = 0; i < inventory.size(); ++i)
            _deleteItem(BASE_ITEMID + i);

        inventory.clear();
        dynamicSkinData.clear();
        dynamicGloveData.clear();
        dynamicAgentData.clear();
        dynamicMusicData.clear();
    }

    void _equipItems() noexcept
    {
        for (const auto& item : toEquip)
            memory->inventoryManager->equipItemInSlot(item.team, item.slot, item.index + BASE_ITEMID);
        toEquip.clear();
    }

    void _runFrame() noexcept
    {
        _deleteItems();
        _addItems();
        _equipItems();
    }

    static InventoryImpl& instance() noexcept
    {
        static InventoryImpl inventory;
        return inventory;
    }

    std::vector<std::tuple<std::reference_wrapper<const game_items::Item>, std::size_t, bool>> toAdd;
    std::vector<ToEquip> toEquip;
    std::vector<InventoryItem> inventory;
};

DynamicSkinData& Inventory::dynamicSkinData(std::size_t index) noexcept
{
    return ::dynamicSkinData[index];
}

DynamicGloveData& Inventory::dynamicGloveData(std::size_t index) noexcept
{
    return ::dynamicGloveData[index];
}

DynamicAgentData& Inventory::dynamicAgentData(std::size_t index) noexcept
{
    return ::dynamicAgentData[index];
}

DynamicMusicData& Inventory::dynamicMusicData(std::size_t index) noexcept
{
    return ::dynamicMusicData[index];
}

DynamicSouvenirPackageData& Inventory::dynamicSouvenirPackageData(std::size_t index) noexcept
{
    return ::dynamicSouvenirPackageData[index];
}

DynamicServiceMedalData& Inventory::dynamicServiceMedalData(std::size_t index) noexcept
{
    return ::dynamicServiceMedalData[index];
}

DynamicTournamentCoinData& Inventory::dynamicTournamentCoinData(std::size_t index) noexcept
{
    return ::dynamicTournamentCoinData[index];
}

std::size_t Inventory::emplaceDynamicData(DynamicSkinData&& data) noexcept
{
    ::dynamicSkinData.push_back(std::move(data));
    return ::dynamicSkinData.size() - 1;
}

std::size_t Inventory::emplaceDynamicData(DynamicGloveData&& data) noexcept
{
    ::dynamicGloveData.push_back(std::move(data));
    return ::dynamicGloveData.size() - 1;
}

std::size_t Inventory::emplaceDynamicData(DynamicAgentData&& data) noexcept
{
    ::dynamicAgentData.push_back(std::move(data));
    return ::dynamicAgentData.size() - 1;
}

std::size_t Inventory::emplaceDynamicData(DynamicMusicData&& data) noexcept
{
    ::dynamicMusicData.push_back(std::move(data));
    return ::dynamicMusicData.size() - 1;
}

std::size_t Inventory::emplaceDynamicData(DynamicSouvenirPackageData&& data) noexcept
{
    ::dynamicSouvenirPackageData.push_back(std::move(data));
    return ::dynamicSouvenirPackageData.size() - 1;
}

std::size_t Inventory::emplaceDynamicData(DynamicServiceMedalData&& data) noexcept
{
    ::dynamicServiceMedalData.push_back(std::move(data));
    return ::dynamicServiceMedalData.size() - 1;
}

std::size_t Inventory::emplaceDynamicData(DynamicTournamentCoinData&& data) noexcept
{
    ::dynamicTournamentCoinData.push_back(std::move(data));
    return ::dynamicTournamentCoinData.size() - 1;
}

std::vector<InventoryItem>& Inventory::get() noexcept
{
    return InventoryImpl::get();
}

void Inventory::addItemUnacknowledged(const game_items::Item& gameItem, std::size_t dynamicDataIdx) noexcept
{
    InventoryImpl::addItem(gameItem, dynamicDataIdx, true);
}

void Inventory::addItemAcknowledged(const game_items::Item& gameItem, std::size_t dynamicDataIdx) noexcept
{
    InventoryImpl::addItem(gameItem, dynamicDataIdx, false);
}

std::uint64_t Inventory::addItemNow(const game_items::Item& gameItem, std::size_t dynamicDataIdx, bool asUnacknowledged) noexcept
{
    return InventoryImpl::addItemNow(gameItem, dynamicDataIdx, asUnacknowledged);
}

void Inventory::deleteItemNow(std::uint64_t itemID) noexcept
{
    InventoryImpl::deleteItemNow(itemID);
}

void Inventory::runFrame() noexcept
{
    InventoryImpl::runFrame();
}

InventoryItem* Inventory::getItem(std::uint64_t itemID) noexcept
{
    return InventoryImpl::getItem(itemID);
}

std::uint64_t Inventory::recreateItem(std::uint64_t itemID) noexcept
{
    return InventoryImpl::recreateItem(itemID);
}

void Inventory::clear() noexcept
{
    InventoryImpl::clear();
}

void Inventory::equipItem(Team team, int slot, std::size_t index) noexcept
{
    InventoryImpl::equipItem(team, slot, index);
}

std::size_t Inventory::getItemIndex(std::uint64_t itemID) noexcept
{
    return InventoryImpl::getItemIndex(itemID);
}
