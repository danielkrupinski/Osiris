#include <gtest/gtest.h>

#include <InventoryChanger/GameItems/Lookup.h>
#include <InventoryChanger/GameItems/Storage.h>

#include <CSGO/ItemSchema.h>

namespace inventory_changer::game_items
{
namespace
{

class InventoryChanger_GameItems_Lookup_EmptyStorageTest : public testing::Test {
protected:
    Lookup lookup{ Storage{} };
};

TEST_F(InventoryChanger_GameItems_Lookup_EmptyStorageTest, ItemIsNotFound) {    
    ASSERT_FALSE(lookup.findItem(WeaponId::BronzeOperationHydraCoin) != nullptr);
}

TEST_F(InventoryChanger_GameItems_Lookup_EmptyStorageTest, ItemWithPaintkitIsNotFound) {
    ASSERT_FALSE(lookup.findItem(WeaponId::Ak47, 14) != nullptr);
}

TEST_F(InventoryChanger_GameItems_Lookup_EmptyStorageTest, MusicIsNotFound) {
    ASSERT_FALSE(lookup.findMusic(3) != nullptr);
}

TEST_F(InventoryChanger_GameItems_Lookup_EmptyStorageTest, StickerIsNotFound) {
    ASSERT_FALSE(lookup.findSticker(csgo::StickerId::BlacksiteFoil) != nullptr);
}

TEST_F(InventoryChanger_GameItems_Lookup_EmptyStorageTest, GraffitiIsNotFound) {
    ASSERT_FALSE(lookup.findGraffiti(3) != nullptr);
}

TEST_F(InventoryChanger_GameItems_Lookup_EmptyStorageTest, PatchIsNotFound) {
    ASSERT_FALSE(lookup.findPatch(3) != nullptr);
}

TEST(InventoryChanger_GameItems_LookupTest, ItemNotFoundIfNotInStorage) {
    Storage storage;
    storage.addServiceMedal(EconRarity::Red, 0, WeaponId::Berlin2019BronzeCoin, {});
    Lookup lookup{ std::move(storage) };
    ASSERT_FALSE(lookup.findItem(WeaponId::Katowice2019BronzeCoin) != nullptr);
}

TEST(InventoryChanger_GameItems_LookupTest, ItemWithStickerAsWeaponIdIsNotAssumedToBeSticker) {
    Storage storage;
    storage.addServiceMedal(EconRarity::Red, 0, WeaponId::Sticker, {});
    Lookup lookup{ std::move(storage) };
    ASSERT_FALSE(lookup.findSticker(csgo::StickerId::Default) != nullptr);
}

TEST(InventoryChanger_GameItems_LookupTest, ItemWithMusicKitAsWeaponIdIsNotAssumedToBeMusic) {
    Storage storage;
    storage.addServiceMedal(EconRarity::Red, 0, WeaponId::MusicKit, {});
    Lookup lookup{ std::move(storage) };
    ASSERT_FALSE(lookup.findMusic(3) != nullptr);
}

TEST(InventoryChanger_GameItems_LookupTest, ItemWithSealedGraffitiAsWeaponIdIsNotAssumedToBeGraffiti) {
    Storage storage;
    storage.addServiceMedal(EconRarity::Red, 0, WeaponId::SealedGraffiti, {});
    Lookup lookup{ std::move(storage) };
    ASSERT_FALSE(lookup.findGraffiti(3) != nullptr);
}

TEST(InventoryChanger_GameItems_LookupTest, ItemWithPatchAsWeaponIdIsNotAssumedToBePatch) {
    Storage storage;
    storage.addServiceMedal(EconRarity::Red, 0, WeaponId::Patch, {});
    Lookup lookup{ std::move(storage) };
    ASSERT_FALSE(lookup.findPatch(21) != nullptr);
}

}
}
