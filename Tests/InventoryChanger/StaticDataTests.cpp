#include <algorithm>
#include <iomanip>
#include <ostream>
#include <string_view>

#include <gtest/gtest.h>

#include "../../Osiris/InventoryChanger/StaticData.h"

TEST(GameItem, CreatedStickerIsSticker) {
    ASSERT_TRUE(StaticData::GameItem::sticker(0, 0, {}).isSticker());
}

TEST(GameItem, CreatedStickerHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::sticker(0, 0, {}).weaponID, WeaponId::Sticker);
}

TEST(GameItem, CreatedSkinIsSkin) {
    ASSERT_TRUE(StaticData::GameItem::skin(0, WeaponId::None, 0, {}).isSkin());
}

TEST(GameItem, CreatedSkinHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::skin(0, WeaponId::Ak47, 0, {}).weaponID, WeaponId::Ak47);
}

TEST(GameItem, CreatedGlovesAreGloves) {
    ASSERT_TRUE(StaticData::GameItem::gloves(0, WeaponId::None, 0, {}).isGlove());
}

TEST(GameItem, CreatedGlovesHaveCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::gloves(0, WeaponId::GloveMotorcycle, 0, {}).weaponID, WeaponId::GloveMotorcycle);
}

TEST(GameItem, CreatedMusicKitIsMusicKit) {
    ASSERT_TRUE(StaticData::GameItem::musicKit(0, 0, {}).isMusic());
}

TEST(GameItem, CreatedMusicKitHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::musicKit(0, 0, {}).weaponID, WeaponId::MusicKit);
}

TEST(GameItem, CreatedCollectibleIsCollectible) {
    ASSERT_TRUE(StaticData::GameItem::collectible(0, WeaponId::None, 0, {}).isCollectible());
}

TEST(GameItem, CreatedCollectibleHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::collectible(0, WeaponId::BronzeOperationHydraCoin, 0, {}).weaponID, WeaponId::BronzeOperationHydraCoin);
}

TEST(GameItem, CreatedNameTagIsNameTag) {
    ASSERT_TRUE(StaticData::GameItem::nameTag(0, WeaponId::None, 0, {}).isNameTag());
}

TEST(GameItem, CreatedNameTagHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::nameTag(0, WeaponId::NameTag, 0, {}).weaponID, WeaponId::NameTag);
}

TEST(GameItem, CreatedPatchIsPatch) {
    ASSERT_TRUE(StaticData::GameItem::patch(0, 0, {}).isPatch());
}

TEST(GameItem, CreatedPatchHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::patch(0, 0, {}).weaponID, WeaponId::Patch);
}

TEST(GameItem, CreatedGraffitiIsGraffiti) {
    ASSERT_TRUE(StaticData::GameItem::graffiti(0, 0, {}).isGraffiti());
}

TEST(GameItem, CreatedGraffitiHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::graffiti(0, 0, {}).weaponID, WeaponId::Graffiti);
}

TEST(GameItem, CreatedSealedGraffitiiIsSealedGraffiti) {
    ASSERT_TRUE(StaticData::GameItem::sealedGraffiti(0, 0, {}).isSealedGraffiti());
}

TEST(GameItem, CreatedSealedGraffitiHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::sealedGraffiti(0, 0, {}).weaponID, WeaponId::SealedGraffiti);
}

TEST(GameItem, CreatedAgentIsAgent) {
    ASSERT_TRUE(StaticData::GameItem::agent(0, WeaponId::None, 0, {}).isAgent());
}

TEST(GameItem, CreatedAgentHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::agent(0, WeaponId::None, 0, {}).weaponID, WeaponId::None);
}

TEST(GameItem, CreatedCaseIsCase) {
    ASSERT_TRUE(StaticData::GameItem::case_(0, WeaponId::None, 0, {}).isCase());
}

TEST(GameItem, CreatedCaseHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::case_(0, WeaponId::None, 0, {}).weaponID, WeaponId::None);
}

TEST(GameItem, CreatedCaseKeyIsCaseKey) {
    ASSERT_TRUE(StaticData::GameItem::caseKey(0, WeaponId::None, 0, {}).isCaseKey());
}

TEST(GameItem, CreatedCaseKeyHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::caseKey(0, WeaponId::None, 0, {}).weaponID, WeaponId::None);
}

TEST(GameItem, CreatedOperationPassIsOperationPass) {
    ASSERT_TRUE(StaticData::GameItem::operationPass(0, WeaponId::None, 0, {}).isOperationPass());
}

TEST(GameItem, CreatedOperationPassHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::operationPass(0, WeaponId::None, 0, {}).weaponID, WeaponId::None);
}

TEST(GameItem, CreatedStatTrakSwapToolIsStatTrakSwapTool) {
    ASSERT_TRUE(StaticData::GameItem::statTrakSwapTool(0, WeaponId::None, 0, {}).isStatTrakSwapTool());
}

TEST(GameItem, CreatedStatTrakSwapToolHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::statTrakSwapTool(0, WeaponId::None, 0, {}).weaponID, WeaponId::None);
}

TEST(GameItem, CreatedViewerPassIsViewerPass) {
    ASSERT_TRUE(StaticData::GameItem::viewerPass(0, WeaponId::None, 0, {}).isViewerPass());
}

TEST(GameItem, CreatedViewerPassHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::viewerPass(0, WeaponId::None, 0, {}).weaponID, WeaponId::None);
}

TEST(GameItem, CreatedServiceMedalIsServiceMedal) {
    ASSERT_TRUE(StaticData::GameItem::serviceMedal(0, WeaponId::None, 0, {}).isServiceMedal());
}

TEST(GameItem, CreatedServiceMedalHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::serviceMedal(0, WeaponId::None, 0, {}).weaponID, WeaponId::None);
}

TEST(GameItem, CreatedSouvenirTokenIsSouvenirToken) {
    ASSERT_TRUE(StaticData::GameItem::souvenirToken(0, WeaponId::None, 0, {}).isSouvenirToken());
}

TEST(GameItem, CreatedSouvenirTokenHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::souvenirToken(0, WeaponId::None, 0, {}).weaponID, WeaponId::None);
}

TEST(GameItem, CreatedTournamentCoinIsTournamentCoin) {
    ASSERT_TRUE(StaticData::GameItem::tournamentCoin(0, WeaponId::None, 0, {}).isTournamentCoin());
}

TEST(GameItem, CreatedTournamentCoinHasCorrectWeaponID) {
    ASSERT_EQ(StaticData::GameItem::tournamentCoin(0, WeaponId::None, 0, {}).weaponID, WeaponId::None);
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
