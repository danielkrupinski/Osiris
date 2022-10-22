#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <InventoryChanger/Backend/PickEm.h>
#include <InventoryChanger/Backend/Request/PickEmHandler.h>

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

class InventoryChanger_Backend_PickEmHandlerTest : public testing::Test {
protected:
    PickEm pickEm;
    MockResponseAccumulator responseAccumulator;
    PickEmHandler<const MockResponseAccumulator&> pickEmHandler{ pickEm, responseAccumulator };
};

TEST_F(InventoryChanger_Backend_PickEmHandlerTest, PickEmIsClearedCorrectly) {
    EXPECT_CALL(responseAccumulator, accumulate(testing::VariantWith<response::PickEmUpdated>(testing::_)));

    pickEm.pick(PickEm::PickPosition{ csgo::Tournament::EleagueAtlanta2017, 1, 2 }, csgo::TournamentTeam::VirtusPro);
    pickEmHandler.clearPicks();
    EXPECT_THAT(pickEm.getPicks(), testing::IsEmpty());
}

class InventoryChanger_Backend_PickEmHandler_PickTeamTest
    : public InventoryChanger_Backend_PickEmHandlerTest, public testing::WithParamInterface<std::pair<PickEm::PickPosition, csgo::TournamentTeam>> {};

TEST_P(InventoryChanger_Backend_PickEmHandler_PickTeamTest, TeamIsPickedCorrectly) {
    EXPECT_CALL(responseAccumulator, accumulate(testing::VariantWith<response::PickEmUpdated>(testing::_)));
    const auto [pickPosition, team] = GetParam();

    pickEmHandler.pickSticker(pickPosition, team);
    EXPECT_EQ(pickEm.getPickedTeam(pickPosition), team);
}

INSTANTIATE_TEST_SUITE_P(, InventoryChanger_Backend_PickEmHandler_PickTeamTest, testing::Values(
    std::make_pair(PickEm::PickPosition{ csgo::Tournament::EleagueAtlanta2017, 1, 2 }, csgo::TournamentTeam::VirtusPro),
    std::make_pair(PickEm::PickPosition{ csgo::Tournament::PglStockholm2021, 3, 4 }, csgo::TournamentTeam::G2Esports)
));

}
}
