#include <string_view>

#include <gtest/gtest.h>

#include <InventoryChanger/GameItems/Storage.h>
#include <InventoryChanger/GameItems/Lookup.h>

namespace game_items
{
namespace
{

enum class ItemType {
    Gloves,
    Skin,
    Patch,
    Sticker,
    Music,
    Graffiti,
    Collectible,
    NameTag,
    Agent,
    Case,
    CaseKey,
    OperationPass,
    StatTrakSwapTool,
    ViewerPass,
    ServiceMedal,
    SouvenirToken,
    TournamentCoin,
    VanillaKnife,
    VanillaSkin
};

Item& addToStorage(Storage& storage, ItemType type, EconRarity rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath)
{
    switch (type) {
    case ItemType::Gloves:
        storage.addPaintKit(0, {}, 0.0f, 0.0f);
        storage.addGlovesWithLastPaintKit(rarity, weaponID, iconPath);
        break;
    case ItemType::Skin:
        storage.addPaintKit(0, {}, 0.0f, 0.0f);
        storage.addSkinWithLastPaintKit(rarity, weaponID, iconPath);
        break;
    case ItemType::Patch:
        storage.addPatch(0, {}, rarity, iconPath);
        break;
    case ItemType::Sticker:
        storage.addSticker(0, {}, rarity, iconPath, 0, {}, 0, false);
        break;
    case ItemType::Music:
        storage.addMusic(0, {}, iconPath);
        break;
    case ItemType::Graffiti:
        storage.addGraffiti(0, {}, rarity, iconPath);
        break;
    case ItemType::Collectible:
        storage.addCollectible(rarity, weaponID, false, iconPath);
        break;
    case ItemType::NameTag:
        storage.addNameTag(rarity, weaponID, iconPath);
        break;
    case ItemType::Agent:
        storage.addAgent(rarity, weaponID, iconPath);
        break;
    case ItemType::Case:
        storage.addCase(rarity, weaponID, 0, iconPath);
        break;
    case ItemType::CaseKey:
        storage.addCaseKey(rarity, weaponID, iconPath);
        break;
    case ItemType::OperationPass:
        storage.addOperationPass(rarity, weaponID, iconPath);
        break;
    case ItemType::StatTrakSwapTool:
        storage.addStatTrakSwapTool(rarity, weaponID, iconPath);
        break;
    case ItemType::ViewerPass:
        storage.addViewerPass(rarity, weaponID, 0, iconPath);
        break;
    case ItemType::ServiceMedal:
        storage.addServiceMedal(rarity, 0, weaponID, iconPath);
        break;
    case ItemType::SouvenirToken:
        storage.addSouvenirToken(rarity, weaponID, 0, iconPath);
        break;
    case ItemType::TournamentCoin:
        storage.addTournamentCoin(rarity, weaponID, 0, 0, iconPath);
        break;
    case ItemType::VanillaKnife:
        storage.addVanillaKnife(weaponID, iconPath);
        break;
    case ItemType::VanillaSkin:
        storage.addVanillaSkin(weaponID, iconPath);
        break;
    default:
        assert(false && "Unhandled item type!");
        break;
    }
    return storage.getItems().back();
}

constexpr bool validateItemType(const Item& item, ItemType desiredType) noexcept
{
    switch (desiredType) {
    case ItemType::Gloves: return item.isGloves();
    case ItemType::Skin: return item.isSkin();
    case ItemType::Patch: return item.isPatch();
    case ItemType::Sticker: return item.isSticker();
    case ItemType::Music: return item.isMusic();
    case ItemType::Graffiti: return item.isGraffiti();
    case ItemType::Collectible: return item.isCollectible();
    case ItemType::NameTag: return item.isNameTag();
    case ItemType::Agent: return item.isAgent();
    case ItemType::Case: return item.isCase();
    case ItemType::CaseKey: return item.isCaseKey();
    case ItemType::OperationPass: return item.isOperationPass();
    case ItemType::StatTrakSwapTool: return item.isStatTrakSwapTool();
    case ItemType::ViewerPass: return item.isViewerPass();
    case ItemType::ServiceMedal: return item.isServiceMedal();
    case ItemType::SouvenirToken: return item.isSouvenirToken();
    case ItemType::TournamentCoin: return item.isTournamentCoin();
    case ItemType::VanillaKnife: return item.isSkin();
    case ItemType::VanillaSkin: return item.isSkin();
    default: return false;
    }
}

TEST(StorageTest, EmptyStorageHasNoItems) {
    ASSERT_TRUE(Storage{}.getItems().empty());
}

class GameItemsStorageTest : public testing::TestWithParam<ItemType> {};

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

TEST_P(GameItemsStorageTest, AddingItemCopiesIconPath) {
    Storage storage;
    const std::string iconPath = "image.png";
    const auto& item = addToStorage(storage, GetParam(), EconRarity::Default, WeaponId::None, 0, iconPath);
    ASSERT_NE(item.getIconPath().data(), iconPath.c_str());
}

const auto typesToTest = testing::Values(
    ItemType::Gloves,
    ItemType::Skin,
    ItemType::Patch,
    ItemType::Sticker,
    ItemType::Music,
    ItemType::Graffiti,
    ItemType::Collectible,
    ItemType::NameTag,
    ItemType::Agent,
    ItemType::Case,
    ItemType::CaseKey,
    ItemType::OperationPass,
    ItemType::StatTrakSwapTool,
    ItemType::ViewerPass,
    ItemType::ServiceMedal,
    ItemType::SouvenirToken,
    ItemType::TournamentCoin,
    ItemType::VanillaKnife,
    ItemType::VanillaSkin
);

INSTANTIATE_TEST_SUITE_P(InventoryChanger, GameItemsStorageTest, typesToTest);

class GameItemsStorageItemRarityTest : public testing::TestWithParam<std::tuple<ItemType, EconRarity>> {};

TEST_P(GameItemsStorageItemRarityTest, AddedItemHasCorrectRarity) {
    Storage storage;
    const auto [type, rarity] = GetParam();
    const auto& item = addToStorage(storage, type, rarity, WeaponId::None, 0, {});
    ASSERT_EQ(item.getRarity(), rarity);
}

const auto typesToTestRarity = testing::Values(
    ItemType::Gloves,
    ItemType::Skin,
    ItemType::Patch,
    ItemType::Sticker,
    ItemType::Graffiti,
    ItemType::Collectible,
    ItemType::NameTag,
    ItemType::Agent,
    ItemType::Case,
    ItemType::CaseKey,
    ItemType::OperationPass,
    ItemType::StatTrakSwapTool,
    ItemType::ViewerPass,
    ItemType::ServiceMedal,
    ItemType::SouvenirToken,
    ItemType::TournamentCoin
);

INSTANTIATE_TEST_SUITE_P(InventoryChanger, GameItemsStorageItemRarityTest,
    testing::Combine(typesToTestRarity, testing::Values(EconRarity::Default, EconRarity::Blue, EconRarity::Gold)));

class GameItemsStorageItemImagePathTest : public testing::TestWithParam<std::tuple<ItemType, std::string_view>> {};

TEST_P(GameItemsStorageItemImagePathTest, AddedItemHasCorrectImagePath) {
    Storage storage;
    const auto [type, imagePath] = GetParam();
    const auto& item = addToStorage(storage, type, EconRarity::Default, WeaponId::None, 0, imagePath);
    ASSERT_EQ(item.getIconPath(), imagePath);
}

INSTANTIATE_TEST_SUITE_P(InventoryChanger, GameItemsStorageItemImagePathTest,
    testing::Combine(typesToTest, testing::Values(std::string_view{}, "image.png")));


using ItemTypeAndWeaponID = std::tuple<ItemType, WeaponId>;

class GameItemsStorageWeaponIdTest : public testing::TestWithParam<ItemTypeAndWeaponID> {};

TEST_P(GameItemsStorageWeaponIdTest, AddedItemHasCorrectWeaponID) {
    Storage storage;
    const auto [type, weaponID] = GetParam();
    const auto& item = addToStorage(storage, type, EconRarity::Default, weaponID, 0, {});
    ASSERT_EQ(item.getWeaponID(), weaponID);
}

const auto typesWithMultipleWeaponIDs = testing::Values(
    ItemType::Gloves,
    ItemType::Skin,
    ItemType::Collectible,
    ItemType::NameTag,
    ItemType::Agent,
    ItemType::Case,
    ItemType::CaseKey,
    ItemType::OperationPass,
    ItemType::StatTrakSwapTool,
    ItemType::ViewerPass,
    ItemType::ServiceMedal,
    ItemType::SouvenirToken,
    ItemType::TournamentCoin,
    ItemType::VanillaKnife,
    ItemType::VanillaSkin
);

INSTANTIATE_TEST_SUITE_P(TypesWithMultipleWeaponIDs, GameItemsStorageWeaponIdTest,
    testing::Combine(typesWithMultipleWeaponIDs,
                     testing::Values(WeaponId::Ak47, WeaponId::Bayonet, WeaponId::GloveLeatherWrap)));

INSTANTIATE_TEST_SUITE_P(TypesWithConstantWeaponID, GameItemsStorageWeaponIdTest,
    testing::Values(
        ItemTypeAndWeaponID{ ItemType::Patch, WeaponId::Patch },
        ItemTypeAndWeaponID{ ItemType::Graffiti, WeaponId::SealedGraffiti },
        ItemTypeAndWeaponID{ ItemType::Sticker, WeaponId::Sticker },
        ItemTypeAndWeaponID{ ItemType::Music, WeaponId::MusicKit }
    )
);

TEST(InventoryChanger_GameItems_StorageTest, FirstAddedItemIsFirstInItemList) {
    Storage storage;
    storage.addNameTag(EconRarity::Gray, WeaponId::NameTag, {});
    storage.addAgent(EconRarity::Pink, WeaponId::None, {});
    ASSERT_TRUE(storage.getItems()[0].isNameTag());
}

}
}
