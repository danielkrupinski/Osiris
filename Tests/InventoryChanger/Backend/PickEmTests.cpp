#include <gtest/gtest.h>

#include <InventoryChanger/Backend/PickEm.h>

namespace inventory_changer::backend
{
namespace
{

TEST(InventoryChanger_Backend_PickEmTest, HasNoPicksWhenConstructed) {
    ASSERT_TRUE(PickEm{}.getPicks().empty());
}

TEST(InventoryChanger_Backend_PickEmTest, RemainsEmptyWhenClearedWhileEmpty) {
    PickEm pickEm;
    pickEm.clear();
    ASSERT_TRUE(pickEm.getPicks().empty());
}

TEST(InventoryChanger_Backend_PickEmTest, PlacingNewPickIncreasesNumberOfPicks) {
    PickEm pickEm;
    pickEm.pick({ csgo::Tournament::PglAntwerp2022, 0, 0 }, csgo::TournamentTeam::ENCE);
    ASSERT_EQ(pickEm.getPicks().size(), 1);
}

TEST(InventoryChanger_Backend_PickEmTest, NoneTeamIsReturnedWhenNoPickInPosition) {
    PickEm pickEm;
    pickEm.pick({ csgo::Tournament::PglAntwerp2022, 2, 3 }, csgo::TournamentTeam::ENCE);
    ASSERT_EQ(pickEm.getPickedTeam({ csgo::Tournament::PglAntwerp2022, 4, 3 }), csgo::TournamentTeam::None);
}

TEST(InventoryChanger_Backend_PickEmTest, HasNoPicksAfterClear) {
    PickEm pickEm;
    pickEm.pick({ csgo::Tournament::PglAntwerp2022, 0, 0 }, csgo::TournamentTeam::NinjasInPyjamas);
    pickEm.pick({ csgo::Tournament::PglAntwerp2022, 2, 3 }, csgo::TournamentTeam::FaZeClan);
    pickEm.pick({ csgo::Tournament::PglAntwerp2022, 5, 6 }, csgo::TournamentTeam::FURIA);
    pickEm.clear();
    ASSERT_TRUE(pickEm.getPicks().empty());
}

class InventoryChanger_Backend_PickEm_PickTeamTest : public testing::TestWithParam<csgo::TournamentTeam> {};

TEST_P(InventoryChanger_Backend_PickEm_PickTeamTest, NewlyPlacedPickHasCorrectTeam) {
    PickEm pickEm;
    pickEm.pick({ csgo::Tournament::IemKatowice2019, 0, 0 }, GetParam());
    ASSERT_EQ(pickEm.getPickedTeam({ csgo::Tournament::IemKatowice2019, 0, 0 }), GetParam());
}

INSTANTIATE_TEST_SUITE_P(, InventoryChanger_Backend_PickEm_PickTeamTest,
    testing::Values(csgo::TournamentTeam::ENCE, csgo::TournamentTeam::FaZeClan));

TEST(InventoryChanger_Backend_PickEmTest, PickingInAlreadyPickedPositionUpdatesTeam) {
    PickEm pickEm;
    pickEm.pick({ csgo::Tournament::PglAntwerp2022, 2, 3 }, csgo::TournamentTeam::NatusVincere);
    pickEm.pick({ csgo::Tournament::PglAntwerp2022, 2, 3 }, csgo::TournamentTeam::FaZeClan);
    ASSERT_EQ(pickEm.getPickedTeam({ csgo::Tournament::PglAntwerp2022, 2, 3 }), csgo::TournamentTeam::FaZeClan);
}

}
}
