#include <limits>

#include <gtest/gtest.h>

#include <InventoryChanger/GameItems/Item.h>
#include <CSGO/ItemSchema.h>

namespace inventory_changer::game_items
{
namespace
{

class RarityTest : public testing::TestWithParam<EconRarity> {};

TEST_P(RarityTest, CreatedItemHasCorrectRarity) {
    const auto item = Item{ Item::Type::Skin, GetParam(), WeaponId{}, 0, {} };
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
    const auto item = Item{ Item::Type::Skin, EconRarity::Blue, WeaponId{}, GetParam(), {} };
    ASSERT_EQ(item.getDataIndex(), GetParam());
}

INSTANTIATE_TEST_SUITE_P(GameItemTest, DataIndexTest, testing::Values(0, 123, std::numeric_limits<std::size_t>::max()));

class IconPathTest : public testing::TestWithParam<std::string_view> {};

TEST_P(IconPathTest, CreatedItemHasCorrectIconPath) {
    const auto item = Item{ Item::Type::Skin, EconRarity::Blue, WeaponId{}, 0, GetParam() };
    ASSERT_EQ(item.getIconPath(), GetParam());
}

INSTANTIATE_TEST_SUITE_P(GameItemTest, IconPathTest, testing::Values(std::string_view{}, "image.png"));

[[nodiscard]] constexpr Item createItemOfType(Item::Type type) noexcept
{
    return Item{ type, EconRarity::Blue, WeaponId{}, 0, {} };
}

class TypeTest : public testing::TestWithParam<Item::Type> {};

TEST_P(TypeTest, CreatedItemIsSticker) {
    ASSERT_EQ(createItemOfType(GetParam()).isSticker(), GetParam() == Item::Type::Sticker);
}

TEST_P(TypeTest, CreatedItemIsSkin) {
    ASSERT_EQ(createItemOfType(GetParam()).isSkin(), GetParam() == Item::Type::Skin);
}

TEST_P(TypeTest, CreatedItemIsGloves) {
    ASSERT_EQ(createItemOfType(GetParam()).isGloves(), GetParam() == Item::Type::Gloves);
}

TEST_P(TypeTest, CreatedItemIsMusic) {
    ASSERT_EQ(createItemOfType(GetParam()).isMusic(), GetParam() == Item::Type::Music);
}

TEST_P(TypeTest, CreatedItemIsCollectible) {
    ASSERT_EQ(createItemOfType(GetParam()).isCollectible(), GetParam() == Item::Type::Collectible);
}

TEST_P(TypeTest, CreatedItemIsNameTag) {
    ASSERT_EQ(createItemOfType(GetParam()).isNameTag(), GetParam() == Item::Type::NameTag);
}

TEST_P(TypeTest, CreatedItemIsPatch) {
    ASSERT_EQ(createItemOfType(GetParam()).isPatch(), GetParam() == Item::Type::Patch);
}

TEST_P(TypeTest, CreatedItemIsGraffiti) {
    ASSERT_EQ(createItemOfType(GetParam()).isGraffiti(), GetParam() == Item::Type::Graffiti);
}

TEST_P(TypeTest, CreatedItemIsAgent) {
    ASSERT_EQ(createItemOfType(GetParam()).isAgent(), GetParam() == Item::Type::Agent);
}

TEST_P(TypeTest, CreatedItemIsCrate) {
    ASSERT_EQ(createItemOfType(GetParam()).isCrate(), GetParam() == Item::Type::Crate);
}

TEST_P(TypeTest, CreatedItemIsCaseKey) {
    ASSERT_EQ(createItemOfType(GetParam()).isCaseKey(), GetParam() == Item::Type::CrateKey);
}

TEST_P(TypeTest, CreatedItemIsOperationPass) {
    ASSERT_EQ(createItemOfType(GetParam()).isOperationPass(), GetParam() == Item::Type::OperationPass);
}

TEST_P(TypeTest, CreatedItemIsStatTrakSwapTool) {
    ASSERT_EQ(createItemOfType(GetParam()).isStatTrakSwapTool(), GetParam() == Item::Type::StatTrakSwapTool);
}

TEST_P(TypeTest, CreatedItemIsViewerPass) {
    ASSERT_EQ(createItemOfType(GetParam()).isViewerPass(), GetParam() == Item::Type::ViewerPass);
}

TEST_P(TypeTest, CreatedItemIsServiceMedal) {
    ASSERT_EQ(createItemOfType(GetParam()).isServiceMedal(), GetParam() == Item::Type::ServiceMedal);
}

TEST_P(TypeTest, CreatedItemIsSouvenirToken) {
    ASSERT_EQ(createItemOfType(GetParam()).isSouvenirToken(), GetParam() == Item::Type::SouvenirToken);
}

TEST_P(TypeTest, CreatedItemIsTournamentCoin) {
    ASSERT_EQ(createItemOfType(GetParam()).isTournamentCoin(), GetParam() == Item::Type::TournamentCoin);
}

INSTANTIATE_TEST_SUITE_P(GameItemTest, TypeTest,
    testing::Values(
        Item::Type::Gloves,
        Item::Type::Skin,
        Item::Type::Patch,
        Item::Type::Sticker,
        Item::Type::Music,
        Item::Type::Graffiti,
        Item::Type::Collectible,
        Item::Type::NameTag,
        Item::Type::Agent,
        Item::Type::Crate,
        Item::Type::CrateKey,
        Item::Type::OperationPass,
        Item::Type::StatTrakSwapTool,
        Item::Type::ViewerPass,
        Item::Type::ServiceMedal,
        Item::Type::SouvenirToken,
        Item::Type::TournamentCoin
    )
);

}
}
