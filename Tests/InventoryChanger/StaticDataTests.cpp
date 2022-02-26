#include <algorithm>
#include <iomanip>
#include <ostream>
#include <string_view>

#include <gtest/gtest.h>

#include "../../Osiris/InventoryChanger/StaticData.h"
#include "../../Osiris/SDK/ItemSchema.h"

class GameItemRarityTest : public testing::TestWithParam<EconRarity> {};

TEST(GameItem, CreatedStickerIsSticker) {
    ASSERT_TRUE(StaticData::GameItem::sticker(EconRarity::Default, 0, {}).isSticker());
}

TEST(GameItem, CreatedStickerHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::sticker(EconRarity::Default, 0, {}).weaponID, WeaponId::Sticker);
}

TEST_P(GameItemRarityTest, CreatedStickerHasCorrectRarity) {
    ASSERT_EQ(StaticData::GameItem::sticker(GetParam(), 0, {}).rarity, GetParam());
}

TEST(GameItem, CreatedSkinIsSkin) {
    ASSERT_TRUE(StaticData::GameItem::skin(EconRarity::Default, WeaponId::None, 0, {}).isSkin());
}

TEST(GameItem, CreatedSkinHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::skin(EconRarity::Default, WeaponId::Ak47, 0, {}).weaponID, WeaponId::Ak47);
}

TEST_P(GameItemRarityTest, CreatedSkinHasCorrectRarity) {
    ASSERT_EQ(StaticData::GameItem::skin(GetParam(), WeaponId::Ak47, 0, {}).rarity, GetParam());
}

TEST(GameItem, CreatedGlovesAreGloves) {
    ASSERT_TRUE(StaticData::GameItem::gloves(EconRarity::Default, WeaponId::None, 0, {}).isGloves());
}

TEST(GameItem, CreatedGlovesHaveCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::gloves(EconRarity::Default, WeaponId::GloveMotorcycle, 0, {}).weaponID, WeaponId::GloveMotorcycle);
}

TEST_P(GameItemRarityTest, CreatedGlovesHaveCorrectRarity) {
    ASSERT_EQ(StaticData::GameItem::gloves(GetParam(), WeaponId::GloveMotorcycle, 0, {}).rarity, GetParam());
}

TEST(GameItem, CreatedMusicKitIsMusicKit) {
    ASSERT_TRUE(StaticData::GameItem::musicKit(EconRarity::Default, 0, {}).isMusic());
}

TEST(GameItem, CreatedMusicKitHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::musicKit(EconRarity::Default, 0, {}).weaponID, WeaponId::MusicKit);
}

TEST_P(GameItemRarityTest, CreatedMusicKitHasCorrectRarity) {
    ASSERT_EQ(StaticData::GameItem::musicKit(GetParam(), 0, {}).rarity, GetParam());
}

TEST(GameItem, CreatedCollectibleIsCollectible) {
    ASSERT_TRUE(StaticData::GameItem::collectible(EconRarity::Default, WeaponId::None, 0, {}).isCollectible());
}

TEST(GameItem, CreatedCollectibleHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::collectible(EconRarity::Default, WeaponId::BronzeOperationHydraCoin, 0, {}).weaponID, WeaponId::BronzeOperationHydraCoin);
}

TEST_P(GameItemRarityTest, CreatedCollectibleHasCorrectRarity) {
    ASSERT_EQ(StaticData::GameItem::collectible(GetParam(), WeaponId::BronzeOperationHydraCoin, 0, {}).rarity, GetParam());
}

TEST(GameItem, CreatedNameTagIsNameTag) {
    ASSERT_TRUE(StaticData::GameItem::nameTag(EconRarity::Default, WeaponId::None, 0, {}).isNameTag());
}

TEST(GameItem, CreatedNameTagHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::nameTag(EconRarity::Default, WeaponId::NameTag, 0, {}).weaponID, WeaponId::NameTag);
}

TEST_P(GameItemRarityTest, CreatedNameTagHasCorrectRarity) {
    ASSERT_EQ(StaticData::GameItem::nameTag(GetParam(), WeaponId::NameTag, 0, {}).rarity, GetParam());
}

TEST(GameItem, CreatedPatchIsPatch) {
    ASSERT_TRUE(StaticData::GameItem::patch(EconRarity::Default, 0, {}).isPatch());
}

TEST(GameItem, CreatedPatchHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::patch(EconRarity::Default, 0, {}).weaponID, WeaponId::Patch);
}

TEST_P(GameItemRarityTest, CreatedPatchHasCorrectRarity) {
    ASSERT_EQ(StaticData::GameItem::patch(GetParam(), 0, {}).rarity, GetParam());
}

TEST(GameItem, CreatedGraffitiIsGraffiti) {
    ASSERT_TRUE(StaticData::GameItem::graffiti(EconRarity::Default, 0, {}).isGraffiti());
}

TEST(GameItem, CreatedGraffitiHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::graffiti(EconRarity::Default, 0, {}).weaponID, WeaponId::Graffiti);
}

TEST_P(GameItemRarityTest, CreatedGraffitiHasCorrectRarity) {
    ASSERT_EQ(StaticData::GameItem::graffiti(GetParam(), 0, {}).rarity, GetParam());
}

TEST(GameItem, CreatedSealedGraffitiiIsSealedGraffiti) {
    ASSERT_TRUE(StaticData::GameItem::sealedGraffiti(EconRarity::Default, 0, {}).isSealedGraffiti());
}

TEST(GameItem, CreatedSealedGraffitiHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::sealedGraffiti(EconRarity::Default, 0, {}).weaponID, WeaponId::SealedGraffiti);
}

TEST_P(GameItemRarityTest, CreatedSealedGraffitiHasCorrectRarity) {
    ASSERT_EQ(StaticData::GameItem::sealedGraffiti(GetParam(), 0, {}).rarity, GetParam());
}

TEST(GameItem, CreatedAgentIsAgent) {
    ASSERT_TRUE(StaticData::GameItem::agent(EconRarity::Default, WeaponId::None, 0, {}).isAgent());
}

TEST(GameItem, CreatedAgentHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::agent(EconRarity::Default, WeaponId::None, 0, {}).weaponID, WeaponId::None);
}

TEST_P(GameItemRarityTest, CreatedAgentHasCorrectRarity) {
    ASSERT_EQ(StaticData::GameItem::agent(GetParam(), WeaponId::None, 0, {}).rarity, GetParam());
}

TEST(GameItem, CreatedCaseIsCase) {
    ASSERT_TRUE(StaticData::GameItem::crate(EconRarity::Default, WeaponId::None, 0, {}).isCase());
}

TEST(GameItem, CreatedCaseHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::crate(EconRarity::Default, WeaponId::None, 0, {}).weaponID, WeaponId::None);
}

TEST_P(GameItemRarityTest, CreatedCaseHasCorrectRarity) {
    ASSERT_EQ(StaticData::GameItem::crate(GetParam(), WeaponId::None, 0, {}).rarity, GetParam());
}

TEST(GameItem, CreatedCaseKeyIsCaseKey) {
    ASSERT_TRUE(StaticData::GameItem::caseKey(EconRarity::Default, WeaponId::None, 0, {}).isCaseKey());
}

TEST(GameItem, CreatedCaseKeyHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::caseKey(EconRarity::Default, WeaponId::None, 0, {}).weaponID, WeaponId::None);
}

TEST_P(GameItemRarityTest, CreatedCaseKeyHasCorrectRarity) {
    ASSERT_EQ(StaticData::GameItem::caseKey(GetParam(), WeaponId::None, 0, {}).rarity, GetParam());
}

TEST(GameItem, CreatedOperationPassIsOperationPass) {
    ASSERT_TRUE(StaticData::GameItem::operationPass(EconRarity::Default, WeaponId::None, 0, {}).isOperationPass());
}

TEST(GameItem, CreatedOperationPassHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::operationPass(EconRarity::Default, WeaponId::None, 0, {}).weaponID, WeaponId::None);
}

TEST_P(GameItemRarityTest, CreatedOperationPassHasCorrectRarity) {
    ASSERT_EQ(StaticData::GameItem::operationPass(GetParam(), WeaponId::None, 0, {}).rarity, GetParam());
}

TEST(GameItem, CreatedStatTrakSwapToolIsStatTrakSwapTool) {
    ASSERT_TRUE(StaticData::GameItem::statTrakSwapTool(EconRarity::Default, WeaponId::None, 0, {}).isStatTrakSwapTool());
}

TEST(GameItem, CreatedStatTrakSwapToolHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::statTrakSwapTool(EconRarity::Default, WeaponId::None, 0, {}).weaponID, WeaponId::None);
}

TEST_P(GameItemRarityTest, CreatedStatTrakSwapToolHasCorrectRarity) {
    ASSERT_EQ(StaticData::GameItem::statTrakSwapTool(GetParam(), WeaponId::None, 0, {}).rarity, GetParam());
}

TEST(GameItem, CreatedViewerPassIsViewerPass) {
    ASSERT_TRUE(StaticData::GameItem::viewerPass(EconRarity::Default, WeaponId::None, 0, {}).isViewerPass());
}

TEST(GameItem, CreatedViewerPassHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::viewerPass(EconRarity::Default, WeaponId::None, 0, {}).weaponID, WeaponId::None);
}

TEST_P(GameItemRarityTest, CreatedViewerPassHasCorrectRarity) {
    ASSERT_EQ(StaticData::GameItem::viewerPass(GetParam(), WeaponId::None, 0, {}).rarity, GetParam());
}

TEST(GameItem, CreatedServiceMedalIsServiceMedal) {
    ASSERT_TRUE(StaticData::GameItem::serviceMedal(EconRarity::Default, WeaponId::None, 0, {}).isServiceMedal());
}

TEST(GameItem, CreatedServiceMedalHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::serviceMedal(EconRarity::Default, WeaponId::None, 0, {}).weaponID, WeaponId::None);
}

TEST_P(GameItemRarityTest, CreatedServiceMedalHasCorrectRarity) {
    ASSERT_EQ(StaticData::GameItem::serviceMedal(GetParam(), WeaponId::None, 0, {}).rarity, GetParam());
}

TEST(GameItem, CreatedSouvenirTokenIsSouvenirToken) {
    ASSERT_TRUE(StaticData::GameItem::souvenirToken(EconRarity::Default, WeaponId::None, 0, {}).isSouvenirToken());
}

TEST(GameItem, CreatedSouvenirTokenHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::souvenirToken(EconRarity::Default, WeaponId::None, 0, {}).weaponID, WeaponId::None);
}

TEST_P(GameItemRarityTest, CreatedSouvenirTokenHasCorrectRarity) {
    ASSERT_EQ(StaticData::GameItem::souvenirToken(GetParam(), WeaponId::None, 0, {}).rarity, GetParam());
}

TEST(GameItem, CreatedTournamentCoinIsTournamentCoin) {
    ASSERT_TRUE(StaticData::GameItem::tournamentCoin(EconRarity::Default, WeaponId::None, 0, {}).isTournamentCoin());
}

TEST(GameItem, CreatedTournamentCoinHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::tournamentCoin(EconRarity::Default, WeaponId::None, 0, {}).weaponID, WeaponId::None);
}

TEST_P(GameItemRarityTest, CreatedTournamentCoinHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::tournamentCoin(GetParam(), WeaponId::None, 0, {}).rarity, GetParam());
}

INSTANTIATE_TEST_SUITE_P(, GameItemRarityTest, testing::Values(EconRarity::Default, EconRarity::Red));

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
