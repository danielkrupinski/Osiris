#include <list>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <InventoryChanger/Backend/Item.h>
#include <InventoryChanger/Backend/ItemIDMap.h>
#include <InventoryChanger/Backend/Request/RequestBuilder.h>

namespace inventory_changer::backend
{
namespace
{

struct MockRequestor {
    MOCK_METHOD(void, request, (const request::RemoveNameTag&));
    MOCK_METHOD(void, request, (const request::WearSticker&));
    MOCK_METHOD(void, request, (const request::RemovePatch&));
    MOCK_METHOD(void, request, (const request::OpenContainer&));
    MOCK_METHOD(void, request, (const request::PerformXRayScan&));
    MOCK_METHOD(void, request, (const request::ClaimXRayScannedItem&));
    MOCK_METHOD(void, request, (const request::SwapStatTrak&));
    MOCK_METHOD(void, request, (const request::ActivateOperationPass&));
    MOCK_METHOD(void, request, (const request::ActivateViewerPass&));
    MOCK_METHOD(void, request, (const request::ActivateSouvenirToken&));
    MOCK_METHOD(void, request, (const request::UnsealGraffiti&));
    MOCK_METHOD(void, request, (const request::ApplySticker&));
    MOCK_METHOD(void, request, (const request::ApplyPatch&));
    MOCK_METHOD(void, request, (const request::AddNameTag&));
};

struct MockRequestorWrapper {
    MockRequestorWrapper(MockRequestor& requestor) : requestor{ requestor } {}

    template <typename RequestType, typename... Args>
    void request(Args&&... args)
    {
        requestor.request(RequestType{ std::forward<Args>(args)... });
    }

private:
    MockRequestor& requestor;
};

class InventoryChanger_Backend_RequestBuilderTest : public testing::Test {
protected:
    using ItemType = game_items::Item::Type;

    template <ItemType type>
    [[nodiscard]] ItemIterator createDummyItem()
    {
        static constexpr game_items::Item dummyGameItem{ type, EconRarity::Red, WeaponId::None, 0, {} };
        itemList.emplace_back(dummyGameItem);
        return std::prev(itemList.end());
    }

    testing::StrictMock<MockRequestor> requestor;
    ItemIDMap itemIDMap;
    RequestBuilder<MockRequestorWrapper> requestBuilder{ itemIDMap, MockRequestorWrapper{ requestor } };

private:
    ItemList itemList;
};

TEST_F(InventoryChanger_Backend_RequestBuilderTest, NameTagRemovalIsNotRequestedWhenItemIdIsInvalid) {
    constexpr auto nonexistentItemID = 1234;
    requestBuilder.removeNameTagFrom(nonexistentItemID);
}

TEST_F(InventoryChanger_Backend_RequestBuilderTest, NameTagRemovalRequestedWhenItemIdIsValid) {
    const auto dummyItem = createDummyItem<ItemType::Skin>();
    constexpr auto dummyItemID = 123;

    EXPECT_CALL(requestor, request(testing::Matcher<const request::RemoveNameTag&>(testing::FieldsAre(dummyItem))));
    itemIDMap.add(dummyItemID, dummyItem);

    requestBuilder.removeNameTagFrom(dummyItemID);
}

TEST_F(InventoryChanger_Backend_RequestBuilderTest, NothingIsRequestedWhenIdOfItemToWearStickerOfIsInvalid) {
    constexpr auto nonexistentItemID = 1234;
    requestBuilder.wearStickerOf(nonexistentItemID, 0);
}

class InventoryChanger_Backend_RequestBuilder_WearStickerTest
    : public InventoryChanger_Backend_RequestBuilderTest,
      public testing::WithParamInterface<std::uint8_t> {};

TEST_P(InventoryChanger_Backend_RequestBuilder_WearStickerTest, WearingStickerIsRequestedWhenItemIsSkin) {
    const auto skin = createDummyItem<ItemType::Skin>();
    constexpr auto dummyItemID = 123;

    EXPECT_CALL(requestor, request(testing::Matcher<const request::WearSticker&>(testing::FieldsAre(skin, GetParam()))));
    itemIDMap.add(dummyItemID, skin);

    requestBuilder.wearStickerOf(dummyItemID, GetParam());
}

TEST_P(InventoryChanger_Backend_RequestBuilder_WearStickerTest, RemovingPatchIsRequestedWhenItemIsAgent) {
    const auto agent = createDummyItem<ItemType::Agent>();
    constexpr auto dummyItemID = 123;

    EXPECT_CALL(requestor, request(testing::Matcher<const request::RemovePatch&>(testing::FieldsAre(agent, GetParam()))));
    itemIDMap.add(dummyItemID, agent);

    requestBuilder.wearStickerOf(dummyItemID, GetParam());
}

INSTANTIATE_TEST_SUITE_P(, InventoryChanger_Backend_RequestBuilder_WearStickerTest, testing::Values(0, 1));

TEST_F(InventoryChanger_Backend_RequestBuilderTest, NothingIsRequestedWhenIDsOfItemsAreInvalid) {
    constexpr auto nonexistentItemID = 1234;
    requestBuilder.useToolOn(nonexistentItemID, nonexistentItemID);
}

TEST_F(InventoryChanger_Backend_RequestBuilderTest, OpeningKeylessContainerCanBeRequested) {
    const auto crate = createDummyItem<ItemType::Case>();
    constexpr auto dummyItemID = 123;

    EXPECT_CALL(requestor, request(testing::Matcher<const request::OpenContainer&>(testing::FieldsAre(crate, std::nullopt))));
    itemIDMap.add(dummyItemID, crate);

    constexpr auto nonexistentItemID = 1234;
    requestBuilder.useToolOn(nonexistentItemID, dummyItemID);
}

}
}
