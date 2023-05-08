#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <InventoryChanger/Backend/Item.h>
#include <InventoryChanger/Backend/Request/ItemModificationHandler.h>
#include <InventoryChanger/Backend/Request/ItemModificationHandler.h>
#include <InventoryChanger/Backend/Response/Response.h>

namespace inventory_changer::backend
{
namespace
{

struct MockResponseAccumulator {
    MOCK_METHOD(void, accumulate, (const Response& response), (const));

    void operator()(const Response& response) const
    {
        accumulate(response);
    }
};

class InventoryChanger_Backend_ItemModificationHandlerTest : public testing::Test {
protected:

    template <typename VariantProperties, game_items::Item::Type Type = game_items::Item::Type::Agent>
    [[nodiscard]] ItemIterator createDummyItem()
    {
        static constexpr game_items::Item dummyGameItem{ Type, EconRarity::Red, WeaponId{}, 0, {} };
        itemList.emplace_back(dummyGameItem, VariantProperties{});
        return std::prev(itemList.end());
    }

    ItemList itemList;
    MockResponseAccumulator responseAccumulator;
    ItemModificationHandler<const MockResponseAccumulator&> itemModificationHandler{ ItemConstRemover{ itemList }, responseAccumulator };
};

TEST_F(InventoryChanger_Backend_ItemModificationHandlerTest, TryingToSelectTeamGraffitiNotOnTournamentCoinResultsInDebugAssertion) {
    const auto dummyItem = createDummyItem<inventory::Item::VariantProperties, game_items::Item::Type::Agent>();
    EXPECT_DEBUG_DEATH(itemModificationHandler.selectTeamGraffiti(dummyItem, 123), "");
}

TEST_F(InventoryChanger_Backend_ItemModificationHandlerTest, UpdateStatTrakDoesNothingForItemsWithoutStatTrak) {
    const auto dummyItem = createDummyItem<inventory::Gloves>();

    EXPECT_CALL(responseAccumulator, accumulate(testing::VariantWith<response::StatTrakUpdated>(testing::_))).Times(0);
    itemModificationHandler.updateStatTrak(dummyItem, 12345);
}

class InventoryChanger_Backend_ItemModificationHandler_UpdateStatTrakTest
    : public InventoryChanger_Backend_ItemModificationHandlerTest, public testing::WithParamInterface<int> {};

TEST_P(InventoryChanger_Backend_ItemModificationHandler_UpdateStatTrakTest, UpdatingStatTrakWorksForSkins) {
    const auto dummyItem = createDummyItem<inventory::Skin>();

    EXPECT_CALL(responseAccumulator, accumulate(testing::VariantWith<response::StatTrakUpdated>(testing::FieldsAre(dummyItem, GetParam()))));
    itemModificationHandler.updateStatTrak(dummyItem, GetParam());

    const auto skin = get<inventory::Skin>(*dummyItem);
    ASSERT_TRUE(skin != nullptr);
    EXPECT_EQ(skin->statTrak, GetParam());
}

TEST_P(InventoryChanger_Backend_ItemModificationHandler_UpdateStatTrakTest, UpdatingStatTrakWorksForMusicKits) {
    const auto dummyItem = createDummyItem<inventory::Music>();

    EXPECT_CALL(responseAccumulator, accumulate(testing::VariantWith<response::StatTrakUpdated>(testing::FieldsAre(dummyItem, GetParam()))));
    itemModificationHandler.updateStatTrak(dummyItem, GetParam());

    const auto music = get<inventory::Music>(*dummyItem);
    ASSERT_TRUE(music != nullptr);
    EXPECT_EQ(music->statTrak, GetParam());
}

INSTANTIATE_TEST_SUITE_P(, InventoryChanger_Backend_ItemModificationHandler_UpdateStatTrakTest,
    testing::Values(-1, 0, 100, 10'000'000));

class InventoryChanger_Backend_ItemModificationHandler_SelectTeamGraffitiTest
    : public InventoryChanger_Backend_ItemModificationHandlerTest, public testing::WithParamInterface<std::uint16_t> {};

TEST_P(InventoryChanger_Backend_ItemModificationHandler_SelectTeamGraffitiTest, SelectingTeamGraffitiWorksForTournamentCoins) {
    const auto dummyItem = createDummyItem<inventory::Item::VariantProperties, game_items::Item::Type::TournamentCoin>();

    EXPECT_CALL(responseAccumulator, accumulate(testing::VariantWith<response::TeamGraffitiSelected>(testing::FieldsAre(dummyItem, GetParam()))));
    itemModificationHandler.selectTeamGraffiti(dummyItem, GetParam());
}

INSTANTIATE_TEST_SUITE_P(, InventoryChanger_Backend_ItemModificationHandler_SelectTeamGraffitiTest,
    testing::Values(0, 1000, 50'000));

}
}
