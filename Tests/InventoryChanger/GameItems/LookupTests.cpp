#include <gtest/gtest.h>

#include <InventoryChanger/GameItems/Lookup.h>
#include <InventoryChanger/GameItems/Storage.h>

namespace game_items
{
namespace
{

class InventoryChanger_GameItems_Lookup_EmptyStorageTest : public testing::Test {
protected:
    Lookup lookup{ Storage{} };
};

TEST_F(InventoryChanger_GameItems_Lookup_EmptyStorageTest, ItemIsNotFound) {    
    ASSERT_FALSE(lookup.findItem(WeaponId::BronzeOperationHydraCoin).has_value());
}

TEST_F(InventoryChanger_GameItems_Lookup_EmptyStorageTest, ItemWithPaintkitIsNotFound) {
    ASSERT_FALSE(lookup.findItem(WeaponId::Ak47, 14).has_value());
}

TEST_F(InventoryChanger_GameItems_Lookup_EmptyStorageTest, MusicIsNotFound) {
    ASSERT_FALSE(lookup.findMusic(3).has_value());
}

TEST_F(InventoryChanger_GameItems_Lookup_EmptyStorageTest, StickerIsNotFound) {
    ASSERT_FALSE(lookup.findSticker(3).has_value());
}

TEST_F(InventoryChanger_GameItems_Lookup_EmptyStorageTest, GraffitiIsNotFound) {
    ASSERT_FALSE(lookup.findGraffiti(3).has_value());
}

TEST_F(InventoryChanger_GameItems_Lookup_EmptyStorageTest, PatchIsNotFound) {
    ASSERT_FALSE(lookup.findPatch(3).has_value());
}

TEST(InventoryChanger_GameItems_LookupTest, ItemNotFoundIfNotInStorage) {
    Storage storage;
    storage.addTournamentCoin(EconRarity::Red, WeaponId::Berlin2019BronzeCoin, 0, {});
    Lookup lookup{ std::move(storage) };
    ASSERT_FALSE(lookup.findItem(WeaponId::Katowice2019BronzeCoin).has_value());
}

}
}
