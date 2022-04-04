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
#include "GameItems/Lookup.h"
#include "Inventory/Item.h"
#include "Inventory/Structs.h"

using Inventory::InvalidDynamicDataIdx;
using Inventory::BASE_ITEMID;

static std::vector<inventory::Skin> dynamicSkinData;
static std::vector<inventory::Glove> dynamicGloveData;
static std::vector<inventory::Agent> dynamicAgentData;
static std::vector<inventory::Music> dynamicMusicData;
static std::vector<inventory::SouvenirPackage> dynamicSouvenirPackageData;
static std::vector<inventory::ServiceMedal> dynamicServiceMedalData;
static std::vector<inventory::TournamentCoin> dynamicTournamentCoinData;
static std::vector<inventory::Graffiti> dynamicGraffitiData;

class InventoryImpl {
public:
    struct ToEquip {
        ToEquip(Team team, int slot, std::size_t index) : team{ team }, slot{ slot }, index{ index } {}

        Team team;
        int slot;
        std::size_t index;
    };

    static std::vector<inventory::Item>& get() noexcept
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
        // instance()._deleteItem(itemID);
    }

    static void runFrame() noexcept
    {
        instance()._runFrame();
    }

    static inventory::Item* getItem(std::uint64_t itemID) noexcept
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
    inventory::Item* _getItem(std::uint64_t itemID) noexcept
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

    std::uint64_t _addItem(const game_items::Item& gameItem, std::size_t dynamicDataIdx, bool asUnacknowledged) noexcept
    {
        return 0;// _createSOCItem(inventory.emplace_back(gameItem, dynamicDataIdx != InvalidDynamicDataIdx ? dynamicDataIdx : ItemGenerator::createDefaultDynamicData(gameItem)), asUnacknowledged);
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

        // _deleteItem(itemID);
        return 0;// _createSOCItem(inventory.emplace_back(std::move(itemCopy)), false);
    }

    void _addItems() noexcept
    {
        for (const auto [gameItem, dynamicDataIndex, asUnacknowledged] : toAdd)
            _addItem(gameItem, dynamicDataIndex, asUnacknowledged);
        toAdd.clear();
    }

    void _deleteItems() noexcept
    {

    }

    void _clear() noexcept
    {
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
    std::vector<inventory::Item> inventory;
};

inventory::Skin& Inventory::dynamicSkinData(const inventory::Item& item) noexcept
{
    assert(item.isSkin());
    return ::dynamicSkinData[item.getDynamicDataIndex()];
}

inventory::Glove& Inventory::dynamicGloveData(const inventory::Item& item) noexcept
{
    assert(item.isGlove());
    return ::dynamicGloveData[item.getDynamicDataIndex()];
}

inventory::Agent& Inventory::dynamicAgentData(const inventory::Item& item) noexcept
{
    assert(item.isAgent());
    return ::dynamicAgentData[item.getDynamicDataIndex()];
}

inventory::Music& Inventory::dynamicMusicData(const inventory::Item& item) noexcept
{
    assert(item.isMusic());
    return ::dynamicMusicData[item.getDynamicDataIndex()];
}

inventory::SouvenirPackage& Inventory::dynamicSouvenirPackageData(const inventory::Item& item) noexcept
{
    assert(item.isCase() && StaticData::isSouvenirPackage(item.get()));
    return ::dynamicSouvenirPackageData[item.getDynamicDataIndex()];
}

inventory::ServiceMedal& Inventory::dynamicServiceMedalData(const inventory::Item& item) noexcept
{
    assert(item.isServiceMedal());
    return ::dynamicServiceMedalData[item.getDynamicDataIndex()];
}

inventory::TournamentCoin& Inventory::dynamicTournamentCoinData(const inventory::Item& item) noexcept
{
    assert(item.isTournamentCoin());
    return ::dynamicTournamentCoinData[item.getDynamicDataIndex()];
}

inventory::Graffiti& Inventory::dynamicGraffitiData(const inventory::Item& item) noexcept
{
    assert(item.isGraffiti());
    return ::dynamicGraffitiData[item.getDynamicDataIndex()];
}

void Inventory::runFrame() noexcept
{
    InventoryImpl::runFrame();
}

inventory::Item* Inventory::getItem(std::uint64_t itemID) noexcept
{
    return InventoryImpl::getItem(itemID);
}

std::uint64_t Inventory::recreateItem(std::uint64_t itemID) noexcept
{
    return InventoryImpl::recreateItem(itemID);
}

std::size_t Inventory::getItemIndex(std::uint64_t itemID) noexcept
{
    return InventoryImpl::getItemIndex(itemID);
}
