#include <algorithm>
#include <iomanip>
#include <ostream>
#include <string_view>

#include <gtest/gtest.h>

#include "../../Osiris/InventoryChanger/GameItemStorage.h"
#include "../../Osiris/InventoryChanger/StaticData.h"

TEST(GameItemStorage, AddedGlovesHasCorrectType) {
    GameItemStorage storage;
    storage.addGloves(0, WeaponId::GloveStuddedBloodhound, 0, "C:\\");
    ASSERT_EQ(storage.get(0).type, Type::Glove);
}

TEST(GameItemStorage, AddedGlovesHasCorrectRarity) {
    GameItemStorage storage;
    storage.addGloves(1, WeaponId::GloveStuddedBloodhound, 0, "C:\\");
    ASSERT_EQ(storage.get(0).rarity, 1);
}

TEST(GameItemStorage, AddedGlovesHasCorrectWeaponID) {
    GameItemStorage storage;
    storage.addGloves(1, WeaponId::GloveStuddedBloodhound, 0, "C:\\");
    ASSERT_EQ(storage.get(0).weaponID, WeaponId::GloveStuddedBloodhound);
}

TEST(GameItemStorage, AddedGlovesHasCorrectDataIndex) {
    GameItemStorage storage;
    storage.addGloves(1, WeaponId::GloveStuddedBloodhound, 123, "C:\\");
    ASSERT_EQ(storage.get(0).dataIndex, 123);
}

TEST(GameItemStorage, AddedGlovesHasCorrectIconPath) {
    GameItemStorage storage;
    storage.addGloves(1, WeaponId::GloveStuddedBloodhound, 123, "D:\\");
    ASSERT_EQ(storage.get(0).iconPath, "D:\\");
}

TEST(GameItemStorage, AddedSkinHasCorrectType) {
    GameItemStorage storage;
    storage.addSkin(0, WeaponId::Ak47, 0, "C:\\");
    ASSERT_EQ(storage.get(0).type, Type::Skin);
}

TEST(GameItemStorage, AddedSkinHasCorrectRarity) {
    GameItemStorage storage;
    storage.addSkin(1, WeaponId::Ak47, 0, "C:\\");
    ASSERT_EQ(storage.get(0).rarity, 1);
}

TEST(GameItemStorage, AddedSkinHasCorrectWeaponID) {
    GameItemStorage storage;
    storage.addSkin(1, WeaponId::Deagle, 0, "C:\\");
    ASSERT_EQ(storage.get(0).weaponID, WeaponId::Deagle);
}

TEST(GameItemStorage, AddedSkinHasCorrectDataIndex) {
    GameItemStorage storage;
    storage.addSkin(1, WeaponId::Ak47, 123, "C:\\");
    ASSERT_EQ(storage.get(0).dataIndex, 123);
}

TEST(GameItemStorage, AddedSkinHasCorrectIconPath) {
    GameItemStorage storage;
    storage.addSkin(1, WeaponId::Ak47, 123, "D:\\");
    ASSERT_EQ(storage.get(0).iconPath, "D:\\");
}

TEST(GameItemStorage, EmptyStorageHasNoUniqueWeaponIDs) {
    GameItemStorage storage;
    ASSERT_TRUE(storage.getUniqueWeaponIDs().empty());
}

TEST(GameItemStorage, HasOneUniqueWeaponIdAfterAddingFirstItem) {
    GameItemStorage storage;
    storage.addSkin(2, WeaponId::Bayonet, 0, "");
    ASSERT_EQ(storage.getUniqueWeaponIDs().size(), 1);
}

TEST(GameItemStorage, HasCorrectNumberOfUniqueWeaponIDs) {
    GameItemStorage storage;
    storage.addSkin(2, WeaponId::Bayonet, 0, "");
    storage.addOperationPass(1, WeaponId::OperationHydraPass, "");
    ASSERT_EQ(storage.getUniqueWeaponIDs().size(), 2);
}

TEST(GameItemStorage, UniqueWeaponIDsAreUnique) {
    GameItemStorage storage;
    storage.addSkin(2, WeaponId::M9Bayonet, 0, "");
    storage.addSkin(3, WeaponId::Ak47, 0, "");
    storage.addSkin(2, WeaponId::M9Bayonet, 0, "");
    storage.addSkin(4, WeaponId::Ak47, 0, "");
    ASSERT_EQ(storage.getUniqueWeaponIDs().size(), 2);
}

using StaticData::TournamentMap;

struct TournamentMapTestParam {
    std::string_view lootListName;
    TournamentMap expectedMap;

    friend std::ostream& operator<<(std::ostream& os, const TournamentMapTestParam& param)
    {
        return os << std::quoted(param.lootListName);
    }
};

class GetTournamentMapOfSouvenirPackageTest : public testing::TestWithParam<TournamentMapTestParam> {};

TEST_P(GetTournamentMapOfSouvenirPackageTest, ReturnsExpectedValue) {
    ASSERT_EQ(static_cast<std::uint32_t>(StaticData::getTournamentMapOfSouvenirPackage(GetParam().lootListName)),
              static_cast<std::uint32_t>(GetParam().expectedMap));
}

#define TournamentMapTestParams(name, map) \
TournamentMapTestParam{ name, map }, \
TournamentMapTestParam{ name "_", TournamentMap::None }

INSTANTIATE_TEST_SUITE_P(
    GetTournamentMapOfSouvenirPackageTests,
    GetTournamentMapOfSouvenirPackageTest,
    ::testing::Values(
        TournamentMapTestParam{ {}, TournamentMap::None },
        TournamentMapTestParams("", TournamentMap::None),
        TournamentMapTestParams("de_ancient", TournamentMap::Ancient),
        TournamentMapTestParams("de_cache", TournamentMap::Cache),
        TournamentMapTestParams("de_cbble", TournamentMap::Cobblestone),
        TournamentMapTestParams("de_dust2", TournamentMap::Dust2),
        TournamentMapTestParams("de_inferno", TournamentMap::Inferno),
        TournamentMapTestParams("de_mirage", TournamentMap::Mirage),
        TournamentMapTestParams("de_nuke", TournamentMap::Nuke),
        TournamentMapTestParams("de_overpass", TournamentMap::Overpass),
        TournamentMapTestParams("de_train", TournamentMap::Train),
        TournamentMapTestParams("de_vertigo", TournamentMap::Vertigo)
));
