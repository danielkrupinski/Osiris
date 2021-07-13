#include <random>

#include "../Helpers.h"
#include "Inventory.h"
#include "../Memory.h"
#include "../SDK/Entity.h"
#include "../SDK/ItemSchema.h"

using Inventory::INVALID_DYNAMIC_DATA_IDX;

static std::vector<DynamicSkinData> dynamicSkinData;
static std::vector<DynamicGloveData> dynamicGloveData;
static std::vector<DynamicAgentData> dynamicAgentData;
static std::vector<DynamicMusicData> dynamicMusicData;

class InventoryImpl {
public:
    static constexpr auto BASE_ITEMID = 1152921504606746975;

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

    static void addItem(std::size_t gameItemIndex, std::size_t dynamicDataIdx, bool asUnacknowledged) noexcept
    {
        instance().toAdd.emplace_back(gameItemIndex, dynamicDataIdx, asUnacknowledged);
    }

    static std::uint64_t addItemNow(std::size_t gameItemIndex, std::size_t dynamicDataIdx, bool asUnacknowledged) noexcept
    {
        return instance()._addItem(gameItemIndex, dynamicDataIdx, asUnacknowledged);
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

    std::uint64_t _createSOCItem(const InventoryItem& inventoryItem, bool asUnacknowledged) noexcept
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
        econItem->rarity = item.rarity;
        econItem->quality = 4;
        econItem->weaponId = item.weaponID;

        if (item.isSticker() || item.isPatch() || item.isGraffiti() || item.isSealedGraffiti()) {
            econItem->setStickerID(0, StaticData::paintKits()[item.dataIndex].id);
        } else if (item.isMusic()) {
            econItem->setMusicID(StaticData::paintKits()[item.dataIndex].id);
            const auto& dynamicData = dynamicMusicData[inventoryItem.getDynamicDataIndex()];
            if (dynamicData.statTrak > -1) {
                econItem->setStatTrak(dynamicData.statTrak);
                econItem->setStatTrakType(1);
                econItem->quality = 9;
            }
        } else if (item.isSkin()) {
            econItem->setPaintKit(static_cast<float>(StaticData::paintKits()[item.dataIndex].id));

            const auto& dynamicData = dynamicSkinData[inventoryItem.getDynamicDataIndex()];
            if (dynamicData.isSouvenir) {
                econItem->quality = 12;
            } else {
                if (dynamicData.statTrak > -1) {
                    econItem->setStatTrak(dynamicData.statTrak);
                    econItem->setStatTrakType(0);
                    econItem->quality = 9;
                }
                if (Helpers::isKnife(econItem->weaponId))
                    econItem->quality = 3;
            }

            econItem->setWear(dynamicData.wear);
            econItem->setSeed(static_cast<float>(dynamicData.seed));
            memory->setCustomName(econItem, dynamicData.nameTag.c_str());

            for (std::size_t j = 0; j < dynamicData.stickers.size(); ++j) {
                const auto& sticker = dynamicData.stickers[j];
                if (sticker.stickerID == 0)
                    continue;

                econItem->setStickerID(j, sticker.stickerID);
                econItem->setStickerWear(j, sticker.wear);
            }
        } else if (item.isGlove()) {
            econItem->quality = 3;
            econItem->setPaintKit(static_cast<float>(StaticData::paintKits()[item.dataIndex].id));

            const auto& dynamicData = dynamicGloveData[inventoryItem.getDynamicDataIndex()];
            econItem->setWear(dynamicData.wear);
            econItem->setSeed(static_cast<float>(dynamicData.seed));
        } else if (item.isCollectible()) {
            if (StaticData::collectibles()[item.dataIndex].isOriginal)
                econItem->quality = 1;
        } else if (item.isAgent()) {
            const auto& dynamicData = dynamicAgentData[inventoryItem.getDynamicDataIndex()];
            for (std::size_t j = 0; j < dynamicData.patches.size(); ++j) {
                const auto& patch = dynamicData.patches[j];
                if (patch.patchID == 0)
                    continue;

                econItem->setStickerID(j, patch.patchID);
            }
        }

        baseTypeCache->addObject(econItem);
        memory->addEconItem(localInventory, econItem, false, false, false);

        if (const auto inventoryComponent = *memory->uiComponentInventory) {
            memory->setItemSessionPropertyValue(inventoryComponent, econItem->itemID, "recent", "0");
            memory->setItemSessionPropertyValue(inventoryComponent, econItem->itemID, "updated", "0");
        }

        if (const auto view = memory->findOrCreateEconItemViewForItemID(econItem->itemID))
            memory->clearInventoryImageRGBA(view);

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

        item->markAsDeleted();
    }

    static std::size_t createDefaultDynamicData(std::size_t gameItemIndex) noexcept
    {
        std::size_t index = static_cast<std::size_t>(-1);

        if (const auto& item = StaticData::gameItems()[gameItemIndex]; item.isSkin()) {
            const auto& staticData = StaticData::paintKits()[item.dataIndex];
            DynamicSkinData dynamicData;
            dynamicData.wear = std::lerp(staticData.wearRemapMin, staticData.wearRemapMax, Helpers::random(0.0f, 0.07f));
            dynamicData.seed = Helpers::random(1, 1000);
            dynamicSkinData.push_back(dynamicData);
            index = dynamicSkinData.size() - 1;
        } else if (item.isGlove()) {
            const auto& staticData = StaticData::paintKits()[item.dataIndex];
            DynamicGloveData dynamicData;
            dynamicData.wear = std::lerp(staticData.wearRemapMin, staticData.wearRemapMax, Helpers::random(0.0f, 0.07f));
            dynamicData.seed = Helpers::random(1, 1000);
            dynamicGloveData.push_back(dynamicData);
            index = dynamicGloveData.size() - 1;
        } else if (item.isAgent()) {
            dynamicAgentData.push_back({});
            index = dynamicAgentData.size() - 1;
        } else if (item.isMusic()) {
            dynamicMusicData.push_back({});
            index = dynamicMusicData.size() - 1;
        }

        return index;
    }

    std::uint64_t _addItem(std::size_t gameItemIndex, std::size_t dynamicDataIdx, bool asUnacknowledged) noexcept
    {
        return _createSOCItem(inventory.emplace_back(gameItemIndex, dynamicDataIdx != INVALID_DYNAMIC_DATA_IDX ? dynamicDataIdx : createDefaultDynamicData(gameItemIndex)), asUnacknowledged);
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
        for (const auto [index, dynamicDataIndex, asUnacknowledged] : toAdd)
            _addItem(index, dynamicDataIndex, asUnacknowledged);
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

    std::vector<std::tuple<std::size_t, std::size_t, bool>> toAdd;
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

std::vector<InventoryItem>& Inventory::get() noexcept
{
    return InventoryImpl::get();
}

void Inventory::addItemUnacknowledged(std::size_t gameItemIndex, std::size_t dynamicDataIdx) noexcept
{
    InventoryImpl::addItem(gameItemIndex, dynamicDataIdx, true);
}

void Inventory::addItemAcknowledged(std::size_t gameItemIndex, std::size_t dynamicDataIdx) noexcept
{
    InventoryImpl::addItem(gameItemIndex, dynamicDataIdx, false);
}

std::uint64_t Inventory::addItemNow(std::size_t gameItemIndex, std::size_t dynamicDataIdx, bool asUnacknowledged) noexcept
{
    return InventoryImpl::addItemNow(gameItemIndex, dynamicDataIdx, asUnacknowledged);
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
