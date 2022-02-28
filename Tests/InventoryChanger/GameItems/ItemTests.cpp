#include <gtest/gtest.h>

#include "../../../Osiris/InventoryChanger/GameItems/Item.h"
#include "../../../Osiris/SDK/ItemSchema.h"

class ItemRarityTest : public testing::TestWithParam<EconRarity> {};

TEST(Item, CreatedStickerIsSticker) {
    ASSERT_TRUE(game_items::Item::sticker(EconRarity::Default, 0, {}).isSticker());
}

TEST(Item, CreatedStickerHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::sticker(EconRarity::Default, 0, {}).getWeaponID(), WeaponId::Sticker);
}

TEST_P(ItemRarityTest, CreatedStickerHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::sticker(GetParam(), 0, {}).getRarity(), GetParam());
}

TEST(Item, CreatedSkinIsSkin) {
    ASSERT_TRUE(game_items::Item::skin(EconRarity::Default, WeaponId::None, 0, {}).isSkin());
}

TEST(Item, CreatedSkinHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::skin(EconRarity::Default, WeaponId::Ak47, 0, {}).getWeaponID(), WeaponId::Ak47);
}

TEST_P(ItemRarityTest, CreatedSkinHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::skin(GetParam(), WeaponId::Ak47, 0, {}).getRarity(), GetParam());
}

TEST(Item, CreatedGlovesAreGloves) {
    ASSERT_TRUE(game_items::Item::gloves(EconRarity::Default, WeaponId::None, 0, {}).isGloves());
}

TEST(Item, CreatedGlovesHaveCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::gloves(EconRarity::Default, WeaponId::GloveMotorcycle, 0, {}).getWeaponID(), WeaponId::GloveMotorcycle);
}

TEST_P(ItemRarityTest, CreatedGlovesHaveCorrectRarity) {
    ASSERT_EQ(game_items::Item::gloves(GetParam(), WeaponId::GloveMotorcycle, 0, {}).getRarity(), GetParam());
}

TEST(Item, CreatedMusicKitIsMusicKit) {
    ASSERT_TRUE(game_items::Item::musicKit(EconRarity::Default, 0, {}).isMusic());
}

TEST(Item, CreatedMusicKitHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::musicKit(EconRarity::Default, 0, {}).getWeaponID(), WeaponId::MusicKit);
}

TEST_P(ItemRarityTest, CreatedMusicKitHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::musicKit(GetParam(), 0, {}).getRarity(), GetParam());
}

TEST(Item, CreatedCollectibleIsCollectible) {
    ASSERT_TRUE(game_items::Item::collectible(EconRarity::Default, WeaponId::None, 0, {}).isCollectible());
}

TEST(Item, CreatedCollectibleHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::collectible(EconRarity::Default, WeaponId::BronzeOperationHydraCoin, 0, {}).getWeaponID(), WeaponId::BronzeOperationHydraCoin);
}

TEST_P(ItemRarityTest, CreatedCollectibleHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::collectible(GetParam(), WeaponId::BronzeOperationHydraCoin, 0, {}).getRarity(), GetParam());
}

TEST(Item, CreatedNameTagIsNameTag) {
    ASSERT_TRUE(game_items::Item::nameTag(EconRarity::Default, WeaponId::None, 0, {}).isNameTag());
}

TEST(Item, CreatedNameTagHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::nameTag(EconRarity::Default, WeaponId::NameTag, 0, {}).getWeaponID(), WeaponId::NameTag);
}

TEST_P(ItemRarityTest, CreatedNameTagHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::nameTag(GetParam(), WeaponId::NameTag, 0, {}).getRarity(), GetParam());
}

TEST(Item, CreatedPatchIsPatch) {
    ASSERT_TRUE(game_items::Item::patch(EconRarity::Default, 0, {}).isPatch());
}

TEST(Item, CreatedPatchHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::patch(EconRarity::Default, 0, {}).getWeaponID(), WeaponId::Patch);
}

TEST_P(ItemRarityTest, CreatedPatchHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::patch(GetParam(), 0, {}).getRarity(), GetParam());
}

TEST(Item, CreatedGraffitiIsGraffiti) {
    ASSERT_TRUE(game_items::Item::graffiti(EconRarity::Default, 0, {}).isGraffiti());
}

TEST(Item, CreatedGraffitiHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::graffiti(EconRarity::Default, 0, {}).getWeaponID(), WeaponId::Graffiti);
}

TEST_P(ItemRarityTest, CreatedGraffitiHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::graffiti(GetParam(), 0, {}).getRarity(), GetParam());
}

TEST(Item, CreatedSealedGraffitiiIsSealedGraffiti) {
    ASSERT_TRUE(game_items::Item::sealedGraffiti(EconRarity::Default, 0, {}).isSealedGraffiti());
}

TEST(Item, CreatedSealedGraffitiHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::sealedGraffiti(EconRarity::Default, 0, {}).getWeaponID(), WeaponId::SealedGraffiti);
}

TEST_P(ItemRarityTest, CreatedSealedGraffitiHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::sealedGraffiti(GetParam(), 0, {}).getRarity(), GetParam());
}

TEST(Item, CreatedAgentIsAgent) {
    ASSERT_TRUE(game_items::Item::agent(EconRarity::Default, WeaponId::None, 0, {}).isAgent());
}

TEST(Item, CreatedAgentHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::agent(EconRarity::Default, WeaponId::None, 0, {}).getWeaponID(), WeaponId::None);
}

TEST_P(ItemRarityTest, CreatedAgentHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::agent(GetParam(), WeaponId::None, 0, {}).getRarity(), GetParam());
}

TEST(Item, CreatedCaseIsCase) {
    ASSERT_TRUE(game_items::Item::crate(EconRarity::Default, WeaponId::None, 0, {}).isCase());
}

TEST(Item, CreatedCaseHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::crate(EconRarity::Default, WeaponId::None, 0, {}).getWeaponID(), WeaponId::None);
}

TEST_P(ItemRarityTest, CreatedCaseHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::crate(GetParam(), WeaponId::None, 0, {}).getRarity(), GetParam());
}

TEST(Item, CreatedCaseKeyIsCaseKey) {
    ASSERT_TRUE(game_items::Item::caseKey(EconRarity::Default, WeaponId::None, 0, {}).isCaseKey());
}

TEST(Item, CreatedCaseKeyHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::caseKey(EconRarity::Default, WeaponId::None, 0, {}).getWeaponID(), WeaponId::None);
}

TEST_P(ItemRarityTest, CreatedCaseKeyHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::caseKey(GetParam(), WeaponId::None, 0, {}).getRarity(), GetParam());
}

TEST(Item, CreatedOperationPassIsOperationPass) {
    ASSERT_TRUE(game_items::Item::operationPass(EconRarity::Default, WeaponId::None, 0, {}).isOperationPass());
}

TEST(Item, CreatedOperationPassHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::operationPass(EconRarity::Default, WeaponId::None, 0, {}).getWeaponID(), WeaponId::None);
}

TEST_P(ItemRarityTest, CreatedOperationPassHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::operationPass(GetParam(), WeaponId::None, 0, {}).getRarity(), GetParam());
}

TEST(Item, CreatedStatTrakSwapToolIsStatTrakSwapTool) {
    ASSERT_TRUE(game_items::Item::statTrakSwapTool(EconRarity::Default, WeaponId::None, 0, {}).isStatTrakSwapTool());
}

TEST(Item, CreatedStatTrakSwapToolHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::statTrakSwapTool(EconRarity::Default, WeaponId::None, 0, {}).getWeaponID(), WeaponId::None);
}

TEST_P(ItemRarityTest, CreatedStatTrakSwapToolHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::statTrakSwapTool(GetParam(), WeaponId::None, 0, {}).getRarity(), GetParam());
}

TEST(Item, CreatedViewerPassIsViewerPass) {
    ASSERT_TRUE(game_items::Item::viewerPass(EconRarity::Default, WeaponId::None, 0, {}).isViewerPass());
}

TEST(Item, CreatedViewerPassHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::viewerPass(EconRarity::Default, WeaponId::None, 0, {}).getWeaponID(), WeaponId::None);
}

TEST_P(ItemRarityTest, CreatedViewerPassHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::viewerPass(GetParam(), WeaponId::None, 0, {}).getRarity(), GetParam());
}

TEST(Item, CreatedServiceMedalIsServiceMedal) {
    ASSERT_TRUE(game_items::Item::serviceMedal(EconRarity::Default, WeaponId::None, 0, {}).isServiceMedal());
}

TEST(Item, CreatedServiceMedalHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::serviceMedal(EconRarity::Default, WeaponId::None, 0, {}).getWeaponID(), WeaponId::None);
}

TEST_P(ItemRarityTest, CreatedServiceMedalHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::serviceMedal(GetParam(), WeaponId::None, 0, {}).getRarity(), GetParam());
}

TEST(Item, CreatedSouvenirTokenIsSouvenirToken) {
    ASSERT_TRUE(game_items::Item::souvenirToken(EconRarity::Default, WeaponId::None, 0, {}).isSouvenirToken());
}

TEST(Item, CreatedSouvenirTokenHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::souvenirToken(EconRarity::Default, WeaponId::None, 0, {}).getWeaponID(), WeaponId::None);
}

TEST_P(ItemRarityTest, CreatedSouvenirTokenHasCorrectRarity) {
    ASSERT_EQ(game_items::Item::souvenirToken(GetParam(), WeaponId::None, 0, {}).getRarity(), GetParam());
}

TEST(Item, CreatedTournamentCoinIsTournamentCoin) {
    ASSERT_TRUE(game_items::Item::tournamentCoin(EconRarity::Default, WeaponId::None, 0, {}).isTournamentCoin());
}

TEST(Item, CreatedTournamentCoinHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::tournamentCoin(EconRarity::Default, WeaponId::None, 0, {}).getWeaponID(), WeaponId::None);
}

TEST_P(ItemRarityTest, CreatedTournamentCoinHasCorrectWeaponID) {
    ASSERT_EQ(game_items::Item::tournamentCoin(GetParam(), WeaponId::None, 0, {}).getRarity(), GetParam());
}

INSTANTIATE_TEST_SUITE_P(, ItemRarityTest, testing::Values(EconRarity::Default, EconRarity::Red));
