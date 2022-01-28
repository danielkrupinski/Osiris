#include <gtest/gtest.h>

#include "../../Osiris/InventoryChanger/GameItemStorage.h"

TEST(GameItemStorage, AddedGlovesHasCorrectType) {
    GameItemStorage storage;
    storage.addGloves(0, WeaponId::GloveStuddedBloodhound, 0, "C:\\");
    ASSERT_EQ(storage.get()[0].type, Type::Glove);
}

TEST(GameItemStorage, AddedGlovesHasCorrectRarity) {
    GameItemStorage storage;
    storage.addGloves(1, WeaponId::GloveStuddedBloodhound, 0, "C:\\");
    ASSERT_EQ(storage.get()[0].rarity, 1);
}

TEST(GameItemStorage, AddedGlovesHasCorrectWeaponID) {
    GameItemStorage storage;
    storage.addGloves(1, WeaponId::GloveStuddedBloodhound, 0, "C:\\");
    ASSERT_EQ(storage.get()[0].weaponID, WeaponId::GloveStuddedBloodhound);
}

TEST(GameItemStorage, AddedGlovesHasCorrectDataIndex) {
    GameItemStorage storage;
    storage.addGloves(1, WeaponId::GloveStuddedBloodhound, 123, "C:\\");
    ASSERT_EQ(storage.get()[0].dataIndex, 123);
}

TEST(GameItemStorage, AddedGlovesHasCorrectIconPath) {
    GameItemStorage storage;
    storage.addGloves(1, WeaponId::GloveStuddedBloodhound, 123, "D:\\");
    ASSERT_EQ(storage.get()[0].iconPath, "D:\\");
}

TEST(GameItemStorage, AddedSkinHasCorrectType) {
    GameItemStorage storage;
    storage.addSkin(0, WeaponId::Ak47, 0, "C:\\");
    ASSERT_EQ(storage.get()[0].type, Type::Skin);
}

TEST(GameItemStorage, AddedSkinHasCorrectRarity) {
    GameItemStorage storage;
    storage.addSkin(1, WeaponId::Ak47, 0, "C:\\");
    ASSERT_EQ(storage.get()[0].rarity, 1);
}

TEST(GameItemStorage, AddedSkinHasCorrectWeaponID) {
    GameItemStorage storage;
    storage.addSkin(1, WeaponId::Deagle, 0, "C:\\");
    ASSERT_EQ(storage.get()[0].weaponID, WeaponId::Deagle);
}

TEST(GameItemStorage, AddedSkinHasCorrectDataIndex) {
    GameItemStorage storage;
    storage.addSkin(1, WeaponId::Ak47, 123, "C:\\");
    ASSERT_EQ(storage.get()[0].dataIndex, 123);
}

TEST(GameItemStorage, AddedSkinHasCorrectIconPath) {
    GameItemStorage storage;
    storage.addSkin(1, WeaponId::Ak47, 123, "D:\\");
    ASSERT_EQ(storage.get()[0].iconPath, "D:\\");
}
