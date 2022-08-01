#include <array>

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
    MOCK_METHOD(void, request, (const request::NameStorageUnit&));
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
    ItemMutableIterator createDummyItem()
    {
        static constexpr game_items::Item dummyGameItem{ type, EconRarity::Red, WeaponId::None, 0, {} };
        itemList.emplace_back(dummyGameItem);
        const auto dummyItem = std::prev(itemList.end());
        itemIDMap.add(dummyItemIDs.at(itemList.size() - 1), dummyItem);
        return std::prev(itemList.end());
    }

    testing::StrictMock<MockRequestor> requestor;
    ItemIDMap itemIDMap;
    RequestBuilder<MockRequestorWrapper> requestBuilder{ itemIDMap, MockRequestorWrapper{ requestor } };

    static constexpr auto nonexistentItemID = 1234;
    static constexpr auto dummyItemIDs = std::to_array<std::uint64_t>({ 123, 256, 1024 });

private:
    ItemList itemList;
};

TEST_F(InventoryChanger_Backend_RequestBuilderTest, NameTagRemovalIsNotRequestedWhenItemIdIsInvalid) {
    requestBuilder.removeNameTagFrom(nonexistentItemID);
}

TEST_F(InventoryChanger_Backend_RequestBuilderTest, NameTagRemovalRequestedWhenItemIdIsValid) {
    const auto dummyItem = createDummyItem<ItemType::Skin>();
    EXPECT_CALL(requestor, request(testing::Matcher<const request::RemoveNameTag&>(testing::FieldsAre(dummyItem))));

    requestBuilder.removeNameTagFrom(dummyItemIDs[0]);
}

TEST_F(InventoryChanger_Backend_RequestBuilderTest, NothingIsRequestedWhenIdOfItemToWearStickerOfIsInvalid) {
    requestBuilder.wearStickerOf(nonexistentItemID, 0);
}

class InventoryChanger_Backend_RequestBuilder_StickerTest
    : public InventoryChanger_Backend_RequestBuilderTest,
      public testing::WithParamInterface<std::uint8_t> {};

TEST_P(InventoryChanger_Backend_RequestBuilder_StickerTest, WearingStickerIsRequestedWhenItemIsSkin) {
    const auto skin = createDummyItem<ItemType::Skin>();
    EXPECT_CALL(requestor, request(testing::Matcher<const request::WearSticker&>(testing::FieldsAre(skin, GetParam()))));

    requestBuilder.wearStickerOf(dummyItemIDs[0], GetParam());
}

TEST_P(InventoryChanger_Backend_RequestBuilder_StickerTest, RemovingPatchIsRequestedWhenItemIsAgent) {
    const auto agent = createDummyItem<ItemType::Agent>();
    EXPECT_CALL(requestor, request(testing::Matcher<const request::RemovePatch&>(testing::FieldsAre(agent, GetParam()))));

    requestBuilder.wearStickerOf(dummyItemIDs[0], GetParam());
}

TEST_P(InventoryChanger_Backend_RequestBuilder_StickerTest, StickerIsAppliedToTheCorrectSlot) {
    const auto sticker = createDummyItem<ItemType::Sticker>();
    const auto skin = createDummyItem<ItemType::Skin>();

    EXPECT_CALL(requestor, request(testing::Matcher<const request::ApplySticker&>(testing::FieldsAre(skin, sticker, GetParam()))));

    requestBuilder.setStickerSlot(GetParam());
    requestBuilder.useToolOn(dummyItemIDs[0], dummyItemIDs[1]);
}

TEST_P(InventoryChanger_Backend_RequestBuilder_StickerTest, PatchIsAppliedToTheCorrectSlot) {
    const auto patch = createDummyItem<ItemType::Patch>();
    const auto agent = createDummyItem<ItemType::Agent>();

    EXPECT_CALL(requestor, request(testing::Matcher<const request::ApplyPatch&>(testing::FieldsAre(agent, patch, GetParam()))));

    requestBuilder.setStickerSlot(GetParam());
    requestBuilder.useToolOn(dummyItemIDs[0], dummyItemIDs[1]);
}

INSTANTIATE_TEST_SUITE_P(, InventoryChanger_Backend_RequestBuilder_StickerTest, testing::Values(0, 1, 5));

TEST_F(InventoryChanger_Backend_RequestBuilderTest, NothingIsRequestedWhenIDsOfItemsAreInvalid) {
    requestBuilder.useToolOn(nonexistentItemID, nonexistentItemID);
}

TEST_F(InventoryChanger_Backend_RequestBuilderTest, NothingIsRequestedWhenDestItemIsNotHandled) {
    createDummyItem<ItemType::Music>();
    requestBuilder.useToolOn(nonexistentItemID, dummyItemIDs[0]);
}

TEST_F(InventoryChanger_Backend_RequestBuilderTest, NothingIsRequestedWhenToolItemIsNotHandled) {
    createDummyItem<ItemType::Music>();
    requestBuilder.useToolOn(dummyItemIDs[0], nonexistentItemID);
}

TEST_F(InventoryChanger_Backend_RequestBuilderTest, NothingIsRequestedWhenItemsAreNotHandled) {
    createDummyItem<ItemType::Music>();
    requestBuilder.useToolOn(dummyItemIDs[0], dummyItemIDs[0]);
}

TEST_F(InventoryChanger_Backend_RequestBuilderTest, OpeningKeylessContainerCanBeRequested) {
    const auto crate = createDummyItem<ItemType::Case>();
    EXPECT_CALL(requestor, request(testing::Matcher<const request::OpenContainer&>(testing::FieldsAre(crate, std::nullopt))));

    requestBuilder.useToolOn(nonexistentItemID, dummyItemIDs[0]);
}

TEST_F(InventoryChanger_Backend_RequestBuilderTest, ClaimingXRayScannedItemFromKeylessContainerCanBeRequested) {
    const auto crate = createDummyItem<ItemType::Case>();
    crate->setState(inventory::Item::State::InXrayScanner);

    EXPECT_CALL(requestor, request(testing::Matcher<const request::ClaimXRayScannedItem&>(testing::FieldsAre(crate, std::nullopt))));

    requestBuilder.useToolOn(nonexistentItemID, dummyItemIDs[0]);
}

TEST_F(InventoryChanger_Backend_RequestBuilderTest, ActivatingOperationPassCanBeRequested) {
    const auto operationPass = createDummyItem<ItemType::OperationPass>();
    EXPECT_CALL(requestor, request(testing::Matcher<const request::ActivateOperationPass&>(testing::FieldsAre(operationPass))));

    requestBuilder.useToolOn(dummyItemIDs[0], nonexistentItemID);
}

TEST_F(InventoryChanger_Backend_RequestBuilderTest, ActivatingViewerPassCanBeRequested) {
    const auto viewerPass = createDummyItem<ItemType::ViewerPass>();
    EXPECT_CALL(requestor, request(testing::Matcher<const request::ActivateViewerPass&>(testing::FieldsAre(viewerPass))));

    requestBuilder.useToolOn(dummyItemIDs[0], nonexistentItemID);
}

TEST_F(InventoryChanger_Backend_RequestBuilderTest, ActivatingSouvenirTokenCanBeRequested) {
    const auto souvenirToken = createDummyItem<ItemType::SouvenirToken>();
    EXPECT_CALL(requestor, request(testing::Matcher<const request::ActivateSouvenirToken&>(testing::FieldsAre(souvenirToken))));

    requestBuilder.useToolOn(dummyItemIDs[0], nonexistentItemID);
}

TEST_F(InventoryChanger_Backend_RequestBuilderTest, UnsealingGraffitiCanBeRequested) {
    const auto graffiti = createDummyItem<ItemType::Graffiti>();
    EXPECT_CALL(requestor, request(testing::Matcher<const request::UnsealGraffiti&>(testing::FieldsAre(graffiti))));

    requestBuilder.useToolOn(dummyItemIDs[0], nonexistentItemID);
}

TEST_F(InventoryChanger_Backend_RequestBuilderTest, UsingStatTrakSwapToolWithoutSettingItemsDoesNotProduceRequest) {
    const auto statTrakSwapTool = createDummyItem<ItemType::StatTrakSwapTool>();
    EXPECT_CALL(requestor, request(testing::An<const request::SwapStatTrak&>())).Times(0);

    requestBuilder.useToolOn(dummyItemIDs[0], nonexistentItemID);
}

TEST_F(InventoryChanger_Backend_RequestBuilderTest, UsingStatTrakSwapToolWithOneItemSetDoesNotProduceRequest) {
    createDummyItem<ItemType::Skin>();
    createDummyItem<ItemType::StatTrakSwapTool>();
    EXPECT_CALL(requestor, request(testing::An<const request::SwapStatTrak&>())).Times(0);

    requestBuilder.setStatTrakSwapItems(dummyItemIDs[0], nonexistentItemID);
    requestBuilder.useToolOn(dummyItemIDs[1], nonexistentItemID);
}

TEST_F(InventoryChanger_Backend_RequestBuilderTest, UsingStatTrakSwapToolWithBothItemsSetProducesRequest) {
    const auto skin1 = createDummyItem<ItemType::Skin>();
    const auto skin2 = createDummyItem<ItemType::Skin>();
    const auto statTrakSwapTool = createDummyItem<ItemType::StatTrakSwapTool>();

    EXPECT_CALL(requestor, request(testing::Matcher<const request::SwapStatTrak&>(testing::FieldsAre(skin1, skin2, statTrakSwapTool))));

    requestBuilder.setStatTrakSwapItems(dummyItemIDs[0], dummyItemIDs[1]);
    requestBuilder.useToolOn(dummyItemIDs[2], nonexistentItemID);
}

TEST_F(InventoryChanger_Backend_RequestBuilderTest, UsingCrateKeyOnCrateProducesRequest) {
    const auto key = createDummyItem<ItemType::CaseKey>();
    const auto crate = createDummyItem<ItemType::Case>();

    EXPECT_CALL(requestor, request(testing::Matcher<const request::OpenContainer&>(testing::FieldsAre(crate, key))));

    requestBuilder.useToolOn(dummyItemIDs[0], dummyItemIDs[1]);
}

TEST_F(InventoryChanger_Backend_RequestBuilderTest, UsingCrateKeyOnHiddenCrateProducesXRayClaimRequest) {
    const auto key = createDummyItem<ItemType::CaseKey>();
    const auto crate = createDummyItem<ItemType::Case>();
    crate->setState(inventory::Item::State::InXrayScanner);

    EXPECT_CALL(requestor, request(testing::Matcher<const request::ClaimXRayScannedItem&>(testing::FieldsAre(crate, key))));

    requestBuilder.useToolOn(dummyItemIDs[0], dummyItemIDs[1]);
}

TEST_F(InventoryChanger_Backend_RequestBuilderTest, UsingStickerNotOnSkinDoesNotProduceRequest) {
    createDummyItem<ItemType::Sticker>();
    createDummyItem<ItemType::Music>();
    EXPECT_CALL(requestor, request(testing::An<const request::ApplySticker&>())).Times(0);

    requestBuilder.useToolOn(dummyItemIDs[0], dummyItemIDs[1]);
}

TEST_F(InventoryChanger_Backend_RequestBuilderTest, UsingPatchNotOnAgentDoesNotProduceRequest) {
    createDummyItem<ItemType::Patch>();
    createDummyItem<ItemType::Music>();
    EXPECT_CALL(requestor, request(testing::An<const request::ApplyPatch&>())).Times(0);

    requestBuilder.useToolOn(dummyItemIDs[0], dummyItemIDs[1]);
}

TEST_F(InventoryChanger_Backend_RequestBuilderTest, UsingNameTagNotOnSkinDoesNotProduceRequest) {
    createDummyItem<ItemType::NameTag>();
    createDummyItem<ItemType::Music>();
    EXPECT_CALL(requestor, request(testing::An<const request::AddNameTag&>())).Times(0);

    requestBuilder.useToolOn(dummyItemIDs[0], dummyItemIDs[1]);
}

TEST_F(InventoryChanger_Backend_RequestBuilderTest, UsingCrateKeyNotOnCrateDoesNotProduceRequest) {
    createDummyItem<ItemType::CaseKey>();
    createDummyItem<ItemType::Music>();

    EXPECT_CALL(requestor, request(testing::An<const request::OpenContainer&>())).Times(0);
    EXPECT_CALL(requestor, request(testing::An<const request::ClaimXRayScannedItem&>())).Times(0);

    requestBuilder.useToolOn(dummyItemIDs[0], dummyItemIDs[1]);
}

}
}
