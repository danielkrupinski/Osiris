#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <InventoryChanger/Backend/Item.h>
#include <InventoryChanger/Backend/XRayScanner.h>
#include <InventoryChanger/GameItems/Item.h>

namespace inventory_changer::backend
{
namespace
{

class InventoryChanger_Backend_XRayScanner_EmptyScannerTest : public testing::Test {
protected:
    InventoryChanger_Backend_XRayScanner_EmptyScannerTest()
    {
        for (const auto& item : dummyItems)
            itemList.emplace_back(item);
    }

    [[nodiscard]] ItemIterator getReward() const { return getItem(0); }
    [[nodiscard]] ItemIterator getAlternateReward() const { return getItem(1); }
    [[nodiscard]] ItemIterator getCrate() const { return getItem(2); }
    [[nodiscard]] ItemIterator getAlternateCrate() const { return getItem(3); }
    [[nodiscard]] ItemIterator getItemNotInScanner() const { return getItem(4); }

    XRayScanner scanner;

private:
    [[nodiscard]] ItemIterator getItem(std::size_t n) const
    {
        return std::next(itemList.begin(), n);
    }

    static constexpr auto dummyItems = std::to_array<game_items::Item>({
        { game_items::Item::Type::Skin, EconRarity::Red, WeaponId::Ak47, 0, {} },
        { game_items::Item::Type::Sticker, EconRarity::Red, WeaponId::Sticker, 0, {} },
        { game_items::Item::Type::Crate, EconRarity::Gray, WeaponId::OperationBravoCase, 0, {} },
        { game_items::Item::Type::Crate, EconRarity::Gray, WeaponId::OperationVanguardCase, 0, {} },
        { game_items::Item::Type::Gloves, EconRarity::Red, WeaponId::GloveStuddedBrokenfang, 0, {} }
    });

    ItemList itemList;
};

TEST_F(InventoryChanger_Backend_XRayScanner_EmptyScannerTest, HasNoItemsByDefault) {
    ASSERT_FALSE(scanner.getItems().has_value());
}

TEST_F(InventoryChanger_Backend_XRayScanner_EmptyScannerTest, StaysEmptyOnRemovalOfItemNotInScanner) {
    scanner.onItemRemoval(getItemNotInScanner());
    ASSERT_FALSE(scanner.getItems().has_value());
}

class InventoryChanger_Backend_XRayScanner_NonEmptyScannerTest : public InventoryChanger_Backend_XRayScanner_EmptyScannerTest {
protected:
    InventoryChanger_Backend_XRayScanner_NonEmptyScannerTest() : InventoryChanger_Backend_XRayScanner_EmptyScannerTest()
    {
        scanner.storeItems(XRayScanner::Items{ getReward(), getCrate() });
    }
};

TEST_F(InventoryChanger_Backend_XRayScanner_NonEmptyScannerTest, ItemsAreStoredCorrectly) {
    ASSERT_THAT(scanner.getItems().value(), testing::FieldsAre(getReward(), getCrate()));
}

TEST_F(InventoryChanger_Backend_XRayScanner_NonEmptyScannerTest, DifferentItemsCanBeStored) {
    scanner.storeItems(XRayScanner::Items{ getAlternateReward(), getAlternateCrate() });
    ASSERT_THAT(scanner.getItems().value(), testing::FieldsAre(getAlternateReward(), getAlternateCrate()));
}

TEST_F(InventoryChanger_Backend_XRayScanner_NonEmptyScannerTest, IsClearedOnRewardRemoval) {
    scanner.onItemRemoval(getReward());
    ASSERT_FALSE(scanner.getItems().has_value());
}

TEST_F(InventoryChanger_Backend_XRayScanner_NonEmptyScannerTest, IsClearedOnCrateRemoval) {
    scanner.onItemRemoval(getCrate());
    ASSERT_FALSE(scanner.getItems().has_value());
}

TEST_F(InventoryChanger_Backend_XRayScanner_NonEmptyScannerTest, IsClearedOnRemovalOfItemWhichIsBothRewardAndCrate) {
    scanner.storeItems(XRayScanner::Items{ getAlternateCrate(), getAlternateCrate() });
    scanner.onItemRemoval(getAlternateCrate());
    ASSERT_FALSE(scanner.getItems().has_value());
}

TEST_F(InventoryChanger_Backend_XRayScanner_NonEmptyScannerTest, IsNotClearedOnRemovalOfItemNotInScanner) {
    scanner.onItemRemoval(getItemNotInScanner());
    ASSERT_THAT(scanner.getItems().value(), testing::FieldsAre(getReward(), getCrate()));
}

}
}