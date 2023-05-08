#include <optional>

#include <gtest/gtest.h>

#include <InventoryChanger/Inventory/Item.h>
#include <InventoryChanger/Backend/Item.h>
#include <InventoryChanger/Backend/Response/ItemInResponse.h>
#include <InventoryChanger/Backend/Response/Response.h>

namespace inventory_changer::backend
{
namespace
{

template <typename ResponseType>
[[nodiscard]] ResponseType makeResponse(ItemIterator item)
{
    if constexpr (std::is_same_v<ResponseType, response::ItemAdded>)
        return response::ItemAdded{ item, false };
    else if constexpr (std::is_same_v<ResponseType, response::ItemMovedToFront>)
        return response::ItemMovedToFront{ item };
    else if constexpr (std::is_same_v<ResponseType, response::ItemUpdated>)
        return response::ItemUpdated{ item };
    else if constexpr (std::is_same_v<ResponseType, response::ItemEquipped>)
        return response::ItemEquipped{ item, 0, csgo::Team::None };
    else if constexpr (std::is_same_v<ResponseType, response::StickerApplied>)
        return response::StickerApplied{ item, 0 };
    else if constexpr (std::is_same_v<ResponseType, response::StickerScraped>)
        return response::StickerScraped{ item, 0 };
    else if constexpr (std::is_same_v<ResponseType, response::StickerRemoved>)
        return response::StickerRemoved{ item, 0 };
    else if constexpr (std::is_same_v<ResponseType, response::ViewerPassActivated>)
        return response::ViewerPassActivated{ item };
    else if constexpr (std::is_same_v<ResponseType, response::NameTagAdded>)
        return response::NameTagAdded{ item };
    else if constexpr (std::is_same_v<ResponseType, response::NameTagRemoved>)
        return response::NameTagRemoved{ item };
    else if constexpr (std::is_same_v<ResponseType, response::ContainerOpened>)
        return response::ContainerOpened{ item };
    else if constexpr (std::is_same_v<ResponseType, response::PatchApplied>)
        return response::PatchApplied{ item, 0 };
    else if constexpr (std::is_same_v<ResponseType, response::PatchRemoved>)
        return response::PatchRemoved{ item, 0 };
    else if constexpr (std::is_same_v<ResponseType, response::SouvenirTokenActivated>)
        return response::SouvenirTokenActivated{ item };
    else if constexpr (std::is_same_v<ResponseType, response::GraffitiUnsealed>)
        return response::GraffitiUnsealed{ item };
    else if constexpr (std::is_same_v<ResponseType, response::TeamGraffitiSelected>)
        return response::TeamGraffitiSelected{ item, 0 };
    else if constexpr (std::is_same_v<ResponseType, response::StatTrakSwapped>)
        return response::StatTrakSwapped{ item };
    else if constexpr (std::is_same_v<ResponseType, response::StatTrakUpdated>)
        return response::StatTrakUpdated{ item, 123 };
    else if constexpr (std::is_same_v<ResponseType, response::ItemHidden>)
        return response::ItemHidden{ item };
    else if constexpr (std::is_same_v<ResponseType, response::XRayItemClaimed>)
        return response::XRayItemClaimed{ item };
    else if constexpr (std::is_same_v<ResponseType, response::XRayScannerUsed>)
        return response::XRayScannerUsed{ item };
    else
        static_assert(!std::is_same_v<ResponseType, ResponseType>, "Not implemented!");
}

constexpr game_items::Item gameItem{ game_items::Item::Type::Agent, EconRarity::Pink, WeaponId{}, 0, {} };

[[nodiscard]] static ItemList createItemList()
{
    ItemList itemList;
    itemList.emplace_back(gameItem);
    itemList.emplace_back(gameItem);
    itemList.emplace_back(gameItem);
    return itemList;
}

const ItemList itemList{ createItemList() };
const ItemIterator item1{ itemList.begin() };
const ItemIterator item2{ std::next(item1) };
const ItemIterator item3{ std::next(item2) };

TEST(InventoryChanger_Backend_ItemInResponse_NeverInResponseTest, ItemRemovedResponseNeverContainsItem) {
    ASSERT_FALSE(ItemInResponse{ item1 }(response::ItemRemoved{ ItemId{} }));
}

TEST(InventoryChanger_Backend_ItemInResponse_NeverInResponseTest, ResponseContainsItemReturnsFalseForItemRemoved) {
    ASSERT_FALSE(responseContainsItem(response::ItemRemoved{ ItemId{} }, item1));
}

TEST(InventoryChanger_Backend_ItemInResponse_NeverInResponseTest, PickEmUpdatedResponseNeverContainsItem) {
    ASSERT_FALSE(ItemInResponse{ item1 }(response::PickEmUpdated{}));
}

TEST(InventoryChanger_Backend_ItemInResponse_NeverInResponseTest, ResponseContainsItemReturnsFalseForPickEmUpdated) {
    ASSERT_FALSE(responseContainsItem(response::PickEmUpdated{}, item1));
}

template <typename ResponseType>
class InventoryChanger_Backend_ItemInResponseTest : public testing::Test {};

using ResponseTypes = testing::Types<
    response::ContainerOpened,
    response::GraffitiUnsealed,
    response::ItemAdded,
    response::ItemEquipped,
    response::ItemHidden,
    response::ItemMovedToFront,
    response::ItemUpdated,
    response::NameTagAdded,
    response::NameTagRemoved,
    response::PatchApplied,
    response::PatchRemoved,
    response::SouvenirTokenActivated,
    response::StatTrakSwapped,
    response::StatTrakUpdated,
    response::StickerApplied,
    response::StickerRemoved,
    response::StickerScraped,
    response::TeamGraffitiSelected,
    response::ViewerPassActivated,
    response::XRayItemClaimed,
    response::XRayScannerUsed
>;

TYPED_TEST_SUITE(InventoryChanger_Backend_ItemInResponseTest, ResponseTypes);

TYPED_TEST(InventoryChanger_Backend_ItemInResponseTest, ItemIsInResponse) {
    ASSERT_TRUE(ItemInResponse{ item1 }(makeResponse<TypeParam>(item1)));
}

TYPED_TEST(InventoryChanger_Backend_ItemInResponseTest, ItemIsNotInResponse) {
    ASSERT_FALSE(ItemInResponse{ item1 }(makeResponse<TypeParam>(item2)));
}

TYPED_TEST(InventoryChanger_Backend_ItemInResponseTest, ResponseContainsItemReturnsTrueIfItemIsInResponse) {
    ASSERT_TRUE(responseContainsItem(makeResponse<TypeParam>(item1), item1));
}

TYPED_TEST(InventoryChanger_Backend_ItemInResponseTest, ResponseContainsItemReturnsFalseIfItemIsNotInResponse) {
    ASSERT_FALSE(responseContainsItem(makeResponse<TypeParam>(item1), item2));
}

}
}
