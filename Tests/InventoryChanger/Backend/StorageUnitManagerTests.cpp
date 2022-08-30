#include <array>
#include <functional>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <InventoryChanger/Backend/Item.h>
#include <InventoryChanger/Backend/StorageUnitManager.h>
#include <InventoryChanger/GameItems/Item.h>

#include <SDK/ItemSchema.h>

namespace inventory_changer::backend
{
namespace
{

template <std::size_t N>
struct StorageUnitWithItems {
    ItemIterator storageUnit;
    std::array<ItemIterator, N> items;
};

class InventoryChanger_Backend_StorageUnitManagerTest : public testing::Test {
protected:
    [[nodiscard]] ItemIterator createDummyItem()
    {
        static constexpr game_items::Item sticker{ game_items::Item::Type::Sticker, EconRarity::Red, WeaponId::None, 0, {} };
        itemList.emplace_back(sticker);
        return std::prev(itemList.end());
    }

    [[nodiscard]] ItemIterator createDummyStorageUnit()
    {
        static constexpr game_items::Item storageUnit{ game_items::Item::Type::StorageUnit, EconRarity::Red, WeaponId::None, 0, {} };
        itemList.emplace_back(storageUnit);
        return std::prev(itemList.end());
    }

    template <std::size_t N>
    [[nodiscard]] StorageUnitWithItems<N> createStorageUnitWithItems()
    {
        StorageUnitWithItems<N> storageUnitWithItems;
        storageUnitWithItems.storageUnit = createDummyStorageUnit();

        std::generate(storageUnitWithItems.items.begin(), storageUnitWithItems.items.end(), [this, &storageUnit = storageUnitWithItems.storageUnit](){
            const auto item = createDummyItem();
            storageUnitManager.addItemToStorageUnit(item, storageUnit);
            return item;
        });
        
        return storageUnitWithItems;
    }

    StorageUnitManager storageUnitManager;

private:
    ItemList itemList;
};

TEST_F(InventoryChanger_Backend_StorageUnitManagerTest, TryingToAddStorageUnitToStorageUnitResultsInDebugAssertion) {
    EXPECT_DEBUG_DEATH(storageUnitManager.addItemToStorageUnit(createDummyStorageUnit(), createDummyStorageUnit()), "");
}

TEST_F(InventoryChanger_Backend_StorageUnitManagerTest, TryingToAddItemToItemResultsInDebugAssertion) {
    EXPECT_DEBUG_DEATH(storageUnitManager.addItemToStorageUnit(createDummyItem(), createDummyItem()), "");
}

TEST_F(InventoryChanger_Backend_StorageUnitManagerTest, TryingToAddStorageUnitToItemResultsInDebugAssertion) {
    EXPECT_DEBUG_DEATH(storageUnitManager.addItemToStorageUnit(createDummyStorageUnit(), createDummyItem()), "");
}

TEST_F(InventoryChanger_Backend_StorageUnitManagerTest, NewItemCanBeAddedToNewStorageUnit) {
    EXPECT_TRUE(storageUnitManager.addItemToStorageUnit(createDummyItem(), createDummyStorageUnit()));
}

TEST_F(InventoryChanger_Backend_StorageUnitManagerTest, SameItemCannotBeAddedToMultipleStorageUnits) {
    const auto item = createDummyItem();
    storageUnitManager.addItemToStorageUnit(item, createDummyStorageUnit());
    EXPECT_FALSE(storageUnitManager.addItemToStorageUnit(item, createDummyStorageUnit()));
}

TEST_F(InventoryChanger_Backend_StorageUnitManagerTest, DifferentItemCanBeAddedToTheSameStorageUnit) {
    const auto storageUnit = createDummyStorageUnit();
    storageUnitManager.addItemToStorageUnit(createDummyItem(), storageUnit);
    EXPECT_TRUE(storageUnitManager.addItemToStorageUnit(createDummyItem(), storageUnit));
}

TEST_F(InventoryChanger_Backend_StorageUnitManagerTest, SameItemCannotBeAddedToTheSameStorageUnitMoreThanOnce) {
    const auto item = createDummyItem();
    const auto storageUnit = createDummyStorageUnit();
    storageUnitManager.addItemToStorageUnit(item, storageUnit);
    EXPECT_FALSE(storageUnitManager.addItemToStorageUnit(item, storageUnit));
}

TEST_F(InventoryChanger_Backend_StorageUnitManagerTest, TryingToRemoveStorageUnitFromStorageUnitResultsInDebugAssertion) {
    EXPECT_DEBUG_DEATH(storageUnitManager.removeItemFromStorageUnit(createDummyStorageUnit(), createDummyStorageUnit()), "");
}

TEST_F(InventoryChanger_Backend_StorageUnitManagerTest, TryingToRemoveItemFromItemResultsInDebugAssertion) {
    EXPECT_DEBUG_DEATH(storageUnitManager.removeItemFromStorageUnit(createDummyItem(), createDummyItem()), "");
}

TEST_F(InventoryChanger_Backend_StorageUnitManagerTest, TryingToRemoveStorageUnitFromItemResultsInDebugAssertion) {
    EXPECT_DEBUG_DEATH(storageUnitManager.removeItemFromStorageUnit(createDummyStorageUnit(), createDummyItem()), "");
}

TEST_F(InventoryChanger_Backend_StorageUnitManagerTest, ItemIsNotRemovedIfNotInStorageUnit) {
    EXPECT_FALSE(storageUnitManager.removeItemFromStorageUnit(createDummyItem(), createDummyStorageUnit()));
}

TEST_F(InventoryChanger_Backend_StorageUnitManagerTest, ItemIsNotRemovedIfStorageUnitDoesNotMatch) {
    const auto item = createDummyItem();
    storageUnitManager.addItemToStorageUnit(item, createDummyStorageUnit());
    EXPECT_FALSE(storageUnitManager.removeItemFromStorageUnit(item, createDummyStorageUnit()));
}

TEST_F(InventoryChanger_Backend_StorageUnitManagerTest, ItemCanBeRemovedFromStorageUnit) {
    const auto item = createDummyItem();
    const auto storageUnit = createDummyStorageUnit();
    storageUnitManager.addItemToStorageUnit(item, storageUnit);
    EXPECT_TRUE(storageUnitManager.removeItemFromStorageUnit(item, storageUnit));
}

struct StorageUnitIdProjection {
    explicit StorageUnitIdProjection(const std::unordered_map<ItemIterator, std::uint32_t>& storageUnitIDs)
        : storageUnitIDs{ storageUnitIDs } {}

    [[nodiscard]] std::uint32_t operator()(ItemIterator item) const
    {
        return storageUnitIDs.at(item);
    }

private:
    const std::unordered_map<ItemIterator, std::uint32_t>& storageUnitIDs;
};

class InventoryChanger_Backend_StorageUnitManager_StorageUnitIDsTest
    : public InventoryChanger_Backend_StorageUnitManagerTest {
protected:
    std::array<StorageUnitWithItems<4>, 2> storageUnitItems{ createStorageUnitWithItems<4>(), createStorageUnitWithItems<4>() };
    std::unordered_map<ItemIterator, std::uint32_t> storageUnitIDs = storageUnitManager.getStorageUnitIDs();
    StorageUnitIdProjection getID{ storageUnitIDs };
};

TEST_F(InventoryChanger_Backend_StorageUnitManager_StorageUnitIDsTest, DifferentStorageUnitsHaveDifferentIDs) {
    EXPECT_NE(getID(storageUnitItems[0].storageUnit), getID(storageUnitItems[1].storageUnit));
}

TEST_F(InventoryChanger_Backend_StorageUnitManager_StorageUnitIDsTest, ItemsInSameStorageUnitHaveSameID) {
    EXPECT_EQ(std::ranges::adjacent_find(storageUnitItems[0].items, std::not_equal_to{}, getID), storageUnitItems[0].items.end());
}

TEST_F(InventoryChanger_Backend_StorageUnitManager_StorageUnitIDsTest, ItemsFromDifferentStorageUnitsHaveDifferentIDs) {
    EXPECT_TRUE(std::ranges::equal(storageUnitItems[0].items, storageUnitItems[1].items, std::not_equal_to{}, getID, getID));
}

TEST_F(InventoryChanger_Backend_StorageUnitManager_StorageUnitIDsTest, ItemsHaveSameIdAsTheirStorageUnit) {
    EXPECT_EQ(getID(storageUnitItems[0].storageUnit), getID(storageUnitItems[0].items[0]));
}

}
}
