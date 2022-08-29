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

}
}
