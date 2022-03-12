#include <string_view>

#include <gtest/gtest.h>

#include "../../../Osiris/InventoryChanger/GameItems/Storage.h"
#include "../../../Osiris/InventoryChanger/GameItems/Lookup.h"

namespace game_items
{
namespace
{

Item& addToStorage(Storage& storage, Item::Type type, EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath)
{
    switch (type) {
    case Item::Type::Gloves:
        storage.addPaintKit(0, {}, 0.0f, 0.0f);
        storage.addGlovesWithLastPaintKit(rarity, weaponID, iconPath);
        break;
    case Item::Type::Skin:
        storage.addPaintKit(0, {}, 0.0f, 0.0f);
        storage.addSkinWithLastPaintKit(rarity, weaponID, iconPath);
        break;
    case Item::Type::Patch:
        storage.addPatch(0, {}, rarity, iconPath);
        break;
    case Item::Type::Sticker:
        storage.addSticker(0, {}, rarity, iconPath, 0, {}, 0, false);
        break;
    case Item::Type::Music:
        storage.addMusic(0, {}, iconPath);
        break;
    case Item::Type::Collectible:
        storage.addCollectible(rarity, weaponID, false, iconPath);
        break;
    case Item::Type::NameTag:
        storage.addNameTag(rarity, weaponID, iconPath);
        break;
    case Item::Type::Agent:
        storage.addAgent(rarity, weaponID, iconPath);
        break;
    case Item::Type::Case:
        storage.addCase(rarity, weaponID, 0, iconPath);
        break;
    case Item::Type::CaseKey:
        storage.addCaseKey(rarity, weaponID, iconPath);
        break;
    case Item::Type::OperationPass:
        storage.addOperationPass(rarity, weaponID, iconPath);
        break;
    case Item::Type::StatTrakSwapTool:
        storage.addStatTrakSwapTool(rarity, weaponID, iconPath);
        break;
    case Item::Type::ViewerPass:
        storage.addViewerPass(rarity, weaponID, 0, iconPath);
        break;
    case Item::Type::ServiceMedal:
        storage.addServiceMedal(rarity, 0, weaponID, iconPath);
        break;
    case Item::Type::SouvenirToken:
        storage.addSouvenirToken(rarity, weaponID, 0, iconPath);
        break;
    case Item::Type::TournamentCoin:
        storage.addTournamentCoin(rarity, weaponID, 0, iconPath);
        break;
    default:
        assert(false && "Unhandled item type!");
        break;
    }
    return storage.getItems().back();
}

constexpr bool validateItemType(const Item& item, Item::Type desiredType) noexcept
{
    switch (desiredType) {
    case Item::Type::Gloves: return item.isGloves();
    case Item::Type::Skin: return item.isSkin();
    case Item::Type::Patch: return item.isPatch();
    case Item::Type::Sticker: return item.isSticker();
    case Item::Type::Music: return item.isMusic();
    case Item::Type::Graffiti: return item.isGraffiti();
    case Item::Type::SealedGraffiti: return item.isSealedGraffiti();
    case Item::Type::Collectible: return item.isCollectible();
    case Item::Type::NameTag: return item.isNameTag();
    case Item::Type::Agent: return item.isAgent();
    case Item::Type::Case: return item.isCase();
    case Item::Type::CaseKey: return item.isCaseKey();
    case Item::Type::OperationPass: return item.isOperationPass();
    case Item::Type::StatTrakSwapTool: return item.isStatTrakSwapTool();
    case Item::Type::ViewerPass: return item.isViewerPass();
    case Item::Type::ServiceMedal: return item.isServiceMedal();
    case Item::Type::SouvenirToken: return item.isSouvenirToken();
    case Item::Type::TournamentCoin: return item.isTournamentCoin();
    default: return false;
    }
}

TEST(StorageTest, EmptyStorageHasNoItems) {
    ASSERT_TRUE(Storage{}.getItems().empty());
}

class GameItemsStorageTest : public testing::TestWithParam<Item::Type> {};

TEST_P(GameItemsStorageTest, AddingItemIncrementsItemCount) {
    Storage storage;
    addToStorage(storage, GetParam(), EconRarity::Default, WeaponId::None, 0, {});
    ASSERT_EQ(storage.getItems().size(), 1);
}

TEST_P(GameItemsStorageTest, AddedItemHasCorrectType) {
    Storage storage;
    const auto& item = addToStorage(storage, GetParam(), EconRarity::Default, WeaponId::None, 0, {});
    ASSERT_TRUE(validateItemType(item, GetParam()));
}

const auto typesToTest = testing::Values(
    Item::Type::Gloves,
    Item::Type::Skin,
    Item::Type::Patch,
    Item::Type::Sticker,
    Item::Type::Music,
    // Item::Type::Graffiti,
    // Item::Type::SealedGraffiti,
    Item::Type::Collectible,
    Item::Type::NameTag,
    Item::Type::Agent,
    Item::Type::Case,
    Item::Type::CaseKey,
    Item::Type::OperationPass,
    Item::Type::StatTrakSwapTool,
    Item::Type::ViewerPass,
    Item::Type::ServiceMedal,
    Item::Type::SouvenirToken,
    Item::Type::TournamentCoin
);

INSTANTIATE_TEST_SUITE_P(InventoryChanger, GameItemsStorageTest, typesToTest);

class GameItemsStorageItemRarityTest : public testing::TestWithParam<std::tuple<Item::Type, EconRarity>> {};

TEST_P(GameItemsStorageItemRarityTest, AddedItemHasCorrectRarity) {
    Storage storage;
    const auto [type, rarity] = GetParam();
    const auto& item = addToStorage(storage, type, rarity, WeaponId::None, 0, {});
    ASSERT_EQ(item.getRarity(), type == Item::Type::Music ? EconRarity::Blue : rarity);
}

INSTANTIATE_TEST_SUITE_P(InventoryChanger, GameItemsStorageItemRarityTest,
    testing::Combine(typesToTest, testing::Values(EconRarity::Default, EconRarity::Blue, EconRarity::Gold)));

class GameItemsStorageItemImagePathTest : public testing::TestWithParam<std::tuple<Item::Type, std::string_view>> {};

TEST_P(GameItemsStorageItemImagePathTest, AddedItemHasCorrectImagePath) {
    Storage storage;
    const auto [type, imagePath] = GetParam();
    const auto& item = addToStorage(storage, type, EconRarity::Default, WeaponId::None, 0, imagePath);
    ASSERT_EQ(item.getIconPath(), imagePath);
}

INSTANTIATE_TEST_SUITE_P(InventoryChanger, GameItemsStorageItemImagePathTest,
    testing::Combine(typesToTest, testing::Values(std::string_view{}, "image.png")));

}
}
