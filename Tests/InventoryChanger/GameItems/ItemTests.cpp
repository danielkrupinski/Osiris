#include <limits>

#include <gtest/gtest.h>

#include "../../../Osiris/InventoryChanger/GameItems/Item.h"
#include "../../../Osiris/SDK/ItemSchema.h"

namespace game_items
{

class RarityTest : public testing::TestWithParam<EconRarity> {};

TEST_P(RarityTest, CreatedItemHasCorrectRarity) {
    const auto item = Item{ Item::Type::Skin, GetParam(), WeaponId::None, 0, {} };
    ASSERT_EQ(item.getRarity(), GetParam());
}

INSTANTIATE_TEST_SUITE_P(GameItemTest, RarityTest, testing::Values(EconRarity::Default, EconRarity::Blue, EconRarity::Gold));

class WeaponIdTest : public testing::TestWithParam<WeaponId> {};

TEST_P(WeaponIdTest, CreatedItemHasCorrectWeaponID) {
    const auto item = Item{ Item::Type::Skin, EconRarity::Blue, GetParam(), 0, {} };
    ASSERT_EQ(item.getWeaponID(), GetParam());
}

INSTANTIATE_TEST_SUITE_P(GameItemTest, WeaponIdTest, testing::Values(WeaponId::Deagle, WeaponId::Ak47));

class DataIndexTest : public testing::TestWithParam<std::size_t> {};

TEST_P(DataIndexTest, CreatedItemHasCorrectDataIndex) {
    const auto item = Item{ Item::Type::Skin, EconRarity::Blue, WeaponId::None, GetParam(), {} };
    ASSERT_EQ(item.getDataIndex(), GetParam());
}

INSTANTIATE_TEST_SUITE_P(GameItemTest, DataIndexTest, testing::Values(0, 123, std::numeric_limits<std::size_t>::max()));

class IconPathTest : public testing::TestWithParam<std::string_view> {};

TEST_P(IconPathTest, CreatedItemHasCorrectIconPath) {
    const auto item = Item{ Item::Type::Skin, EconRarity::Blue, WeaponId::None, 0, GetParam() };
    ASSERT_EQ(item.getIconPath(), GetParam());
}

INSTANTIATE_TEST_SUITE_P(GameItemTest, IconPathTest, testing::Values(std::string_view{}, "image.png"));

[[nodiscard]] constexpr Item createItemOfType(Item::Type type) noexcept
{
    return Item{ type, EconRarity::Blue, WeaponId::None, 0, {} };
}

TEST(GameItemTest, CreatedStickerIsSticker) {
    ASSERT_TRUE(createItemOfType(Item::Type::Sticker).isSticker());
}

TEST(GameItemTest, CreatedSkinIsSkin) {
    ASSERT_TRUE(createItemOfType(Item::Type::Skin).isSkin());
}

TEST(GameItemTest, CreatedGlovesIsGloves) {
    ASSERT_TRUE(createItemOfType(Item::Type::Gloves).isGloves());
}

TEST(GameItemTest, CreatedMusicIsMusic) {
    ASSERT_TRUE(createItemOfType(Item::Type::Music).isMusic());
}

TEST(GameItemTest, CreatedCollectibleIsCollectible) {
    ASSERT_TRUE(createItemOfType(Item::Type::Collectible).isCollectible());
}

TEST(GameItemTest, CreatedNameTagIsNameTag) {
    ASSERT_TRUE(createItemOfType(Item::Type::NameTag).isNameTag());
}

TEST(GameItemTest, CreatedPatchIsPatch) {
    ASSERT_TRUE(createItemOfType(Item::Type::Patch).isPatch());
}

TEST(GameItemTest, CreatedGraffitiIsGraffiti) {
    ASSERT_TRUE(createItemOfType(Item::Type::Graffiti).isGraffiti());
}

TEST(GameItemTest, CreatedSealedGraffitiIsSealedGraffiti) {
    ASSERT_TRUE(createItemOfType(Item::Type::SealedGraffiti).isSealedGraffiti());
}

TEST(GameItemTest, CreatedAgentIsAgent) {
    ASSERT_TRUE(createItemOfType(Item::Type::Agent).isAgent());
}

TEST(GameItemTest, CreatedCaseIsCase) {
    ASSERT_TRUE(createItemOfType(Item::Type::Case).isCase());
}

TEST(GameItemTest, CreatedCaseKeyIsCaseKey) {
    ASSERT_TRUE(createItemOfType(Item::Type::CaseKey).isCaseKey());
}

TEST(GameItemTest, CreatedOperationPassIsOperationPass) {
    ASSERT_TRUE(createItemOfType(Item::Type::OperationPass).isOperationPass());
}

TEST(GameItemTest, CreatedStatTrakSwapToolIsStatTrakSwapTool) {
    ASSERT_TRUE(createItemOfType(Item::Type::StatTrakSwapTool).isStatTrakSwapTool());
}

TEST(GameItemTest, CreatedViewerPassIsViewerPass) {
    ASSERT_TRUE(createItemOfType(Item::Type::ViewerPass).isViewerPass());
}

TEST(GameItemTest, CreatedServiceMedalIsServiceMedal) {
    ASSERT_TRUE(createItemOfType(Item::Type::ServiceMedal).isServiceMedal());
}

TEST(GameItemTest, CreatedSouvenirTokenIsSouvenirToken) {
    ASSERT_TRUE(createItemOfType(Item::Type::SouvenirToken).isSouvenirToken());
}

TEST(GameItemTest, CreatedTournamentCoinIsTournamentCoin) {
    ASSERT_TRUE(createItemOfType(Item::Type::TournamentCoin).isTournamentCoin());
}

}
