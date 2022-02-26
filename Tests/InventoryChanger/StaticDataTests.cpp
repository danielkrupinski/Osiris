#include <algorithm>
#include <iomanip>
#include <ostream>
#include <string_view>

#include <gtest/gtest.h>

#include "../../Osiris/InventoryChanger/StaticData.h"
#include "../../Osiris/SDK/ItemSchema.h"

class ItemRarityTest : public testing::TestWithParam<EconRarity> {};

TEST(Item, CreatedStickerIsSticker) {
    ASSERT_TRUE(game_items::Item::sticker(EconRarity::Default, 0, {}).isSticker());
}

TEST(Item, CreatedStickerHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::sticker(EconRarity::Default, 0, {}).weaponID, WeaponId::Sticker);
}

TEST_P(ItemRarityTest, CreatedStickerHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::sticker(GetParam(), 0, {}).rarity, GetParam());
}

TEST(Item, CreatedSkinIsSkin) {
    ASSERT_TRUE(game_items::Item::skin(EconRarity::Default, WeaponId::None, 0, {}).isSkin());
}

TEST(Item, CreatedSkinHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::skin(EconRarity::Default, WeaponId::Ak47, 0, {}).weaponID, WeaponId::Ak47);
}

TEST_P(ItemRarityTest, CreatedSkinHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::skin(GetParam(), WeaponId::Ak47, 0, {}).rarity, GetParam());
}

TEST(Item, CreatedGlovesAreGloves) {
    ASSERT_TRUE(game_items::Item::gloves(EconRarity::Default, WeaponId::None, 0, {}).isGloves());
}

TEST(Item, CreatedGlovesHaveCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::gloves(EconRarity::Default, WeaponId::GloveMotorcycle, 0, {}).weaponID, WeaponId::GloveMotorcycle);
}

TEST_P(ItemRarityTest, CreatedGlovesHaveCorrectRarity) {
    ASSERT_EQ(game_items::Item::gloves(GetParam(), WeaponId::GloveMotorcycle, 0, {}).rarity, GetParam());
}

TEST(Item, CreatedMusicKitIsMusicKit) {
    ASSERT_TRUE(game_items::Item::musicKit(EconRarity::Default, 0, {}).isMusic());
}

TEST(Item, CreatedMusicKitHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::musicKit(EconRarity::Default, 0, {}).weaponID, WeaponId::MusicKit);
}

TEST_P(ItemRarityTest, CreatedMusicKitHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::musicKit(GetParam(), 0, {}).rarity, GetParam());
}

TEST(Item, CreatedCollectibleIsCollectible) {
    ASSERT_TRUE(game_items::Item::collectible(EconRarity::Default, WeaponId::None, 0, {}).isCollectible());
}

TEST(Item, CreatedCollectibleHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::collectible(EconRarity::Default, WeaponId::BronzeOperationHydraCoin, 0, {}).weaponID, WeaponId::BronzeOperationHydraCoin);
}

TEST_P(ItemRarityTest, CreatedCollectibleHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::collectible(GetParam(), WeaponId::BronzeOperationHydraCoin, 0, {}).rarity, GetParam());
}

TEST(Item, CreatedNameTagIsNameTag) {
    ASSERT_TRUE(game_items::Item::nameTag(EconRarity::Default, WeaponId::None, 0, {}).isNameTag());
}

TEST(Item, CreatedNameTagHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::nameTag(EconRarity::Default, WeaponId::NameTag, 0, {}).weaponID, WeaponId::NameTag);
}

TEST_P(ItemRarityTest, CreatedNameTagHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::nameTag(GetParam(), WeaponId::NameTag, 0, {}).rarity, GetParam());
}

TEST(Item, CreatedPatchIsPatch) {
    ASSERT_TRUE(game_items::Item::patch(EconRarity::Default, 0, {}).isPatch());
}

TEST(Item, CreatedPatchHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::patch(EconRarity::Default, 0, {}).weaponID, WeaponId::Patch);
}

TEST_P(ItemRarityTest, CreatedPatchHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::patch(GetParam(), 0, {}).rarity, GetParam());
}

TEST(Item, CreatedGraffitiIsGraffiti) {
    ASSERT_TRUE(game_items::Item::graffiti(EconRarity::Default, 0, {}).isGraffiti());
}

TEST(Item, CreatedGraffitiHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::graffiti(EconRarity::Default, 0, {}).weaponID, WeaponId::Graffiti);
}

TEST_P(ItemRarityTest, CreatedGraffitiHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::graffiti(GetParam(), 0, {}).rarity, GetParam());
}

TEST(Item, CreatedSealedGraffitiiIsSealedGraffiti) {
    ASSERT_TRUE(game_items::Item::sealedGraffiti(EconRarity::Default, 0, {}).isSealedGraffiti());
}

TEST(Item, CreatedSealedGraffitiHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::sealedGraffiti(EconRarity::Default, 0, {}).weaponID, WeaponId::SealedGraffiti);
}

TEST_P(ItemRarityTest, CreatedSealedGraffitiHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::sealedGraffiti(GetParam(), 0, {}).rarity, GetParam());
}

TEST(Item, CreatedAgentIsAgent) {
    ASSERT_TRUE(game_items::Item::agent(EconRarity::Default, WeaponId::None, 0, {}).isAgent());
}

TEST(Item, CreatedAgentHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::agent(EconRarity::Default, WeaponId::None, 0, {}).weaponID, WeaponId::None);
}

TEST_P(ItemRarityTest, CreatedAgentHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::agent(GetParam(), WeaponId::None, 0, {}).rarity, GetParam());
}

TEST(Item, CreatedCaseIsCase) {
    ASSERT_TRUE(game_items::Item::crate(EconRarity::Default, WeaponId::None, 0, {}).isCase());
}

TEST(Item, CreatedCaseHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::crate(EconRarity::Default, WeaponId::None, 0, {}).weaponID, WeaponId::None);
}

TEST_P(ItemRarityTest, CreatedCaseHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::crate(GetParam(), WeaponId::None, 0, {}).rarity, GetParam());
}

TEST(Item, CreatedCaseKeyIsCaseKey) {
    ASSERT_TRUE(game_items::Item::caseKey(EconRarity::Default, WeaponId::None, 0, {}).isCaseKey());
}

TEST(Item, CreatedCaseKeyHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::caseKey(EconRarity::Default, WeaponId::None, 0, {}).weaponID, WeaponId::None);
}

TEST_P(ItemRarityTest, CreatedCaseKeyHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::caseKey(GetParam(), WeaponId::None, 0, {}).rarity, GetParam());
}

TEST(Item, CreatedOperationPassIsOperationPass) {
    ASSERT_TRUE(game_items::Item::operationPass(EconRarity::Default, WeaponId::None, 0, {}).isOperationPass());
}

TEST(Item, CreatedOperationPassHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::operationPass(EconRarity::Default, WeaponId::None, 0, {}).weaponID, WeaponId::None);
}

TEST_P(ItemRarityTest, CreatedOperationPassHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::operationPass(GetParam(), WeaponId::None, 0, {}).rarity, GetParam());
}

TEST(Item, CreatedStatTrakSwapToolIsStatTrakSwapTool) {
    ASSERT_TRUE(game_items::Item::statTrakSwapTool(EconRarity::Default, WeaponId::None, 0, {}).isStatTrakSwapTool());
}

TEST(Item, CreatedStatTrakSwapToolHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::statTrakSwapTool(EconRarity::Default, WeaponId::None, 0, {}).weaponID, WeaponId::None);
}

TEST_P(ItemRarityTest, CreatedStatTrakSwapToolHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::statTrakSwapTool(GetParam(), WeaponId::None, 0, {}).rarity, GetParam());
}

TEST(Item, CreatedViewerPassIsViewerPass) {
    ASSERT_TRUE(game_items::Item::viewerPass(EconRarity::Default, WeaponId::None, 0, {}).isViewerPass());
}

TEST(Item, CreatedViewerPassHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::viewerPass(EconRarity::Default, WeaponId::None, 0, {}).weaponID, WeaponId::None);
}

TEST_P(ItemRarityTest, CreatedViewerPassHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::viewerPass(GetParam(), WeaponId::None, 0, {}).rarity, GetParam());
}

TEST(Item, CreatedServiceMedalIsServiceMedal) {
    ASSERT_TRUE(game_items::Item::serviceMedal(EconRarity::Default, WeaponId::None, 0, {}).isServiceMedal());
}

TEST(Item, CreatedServiceMedalHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::serviceMedal(EconRarity::Default, WeaponId::None, 0, {}).weaponID, WeaponId::None);
}

TEST_P(ItemRarityTest, CreatedServiceMedalHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::serviceMedal(GetParam(), WeaponId::None, 0, {}).rarity, GetParam());
}

TEST(Item, CreatedSouvenirTokenIsSouvenirToken) {
    ASSERT_TRUE(game_items::Item::souvenirToken(EconRarity::Default, WeaponId::None, 0, {}).isSouvenirToken());
}

TEST(Item, CreatedSouvenirTokenHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::souvenirToken(EconRarity::Default, WeaponId::None, 0, {}).weaponID, WeaponId::None);
}

TEST_P(ItemRarityTest, CreatedSouvenirTokenHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::souvenirToken(GetParam(), WeaponId::None, 0, {}).rarity, GetParam());
}

TEST(Item, CreatedTournamentCoinIsTournamentCoin) {
    ASSERT_TRUE(game_items::Item::tournamentCoin(EconRarity::Default, WeaponId::None, 0, {}).isTournamentCoin());
}

TEST(Item, CreatedTournamentCoinHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::tournamentCoin(EconRarity::Default, WeaponId::None, 0, {}).weaponID, WeaponId::None);
}

TEST_P(ItemRarityTest, CreatedTournamentCoinHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::tournamentCoin(GetParam(), WeaponId::None, 0, {}).rarity, GetParam());
}

INSTANTIATE_TEST_SUITE_P(, ItemRarityTest, testing::Values(EconRarity::Default, EconRarity::Red));

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
