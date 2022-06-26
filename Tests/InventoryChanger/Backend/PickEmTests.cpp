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
    pickEm.pick({ 0, 0, 0 }, TournamentTeam::ENCE);
    ASSERT_EQ(pickEm.getPicks().size(), 1);
}

TEST(InventoryChanger_Backend_PickEmTest, NoneTeamIsReturnedWhenNoPickInPosition) {
    PickEm pickEm;
    pickEm.pick({ 1, 2, 3 }, TournamentTeam::ENCE);
    ASSERT_EQ(pickEm.getPickedTeam({ 3, 4, 3 }), TournamentTeam::None);
}

TEST(InventoryChanger_Backend_PickEmTest, HasNoPicksAfterClear) {
    PickEm pickEm;
    pickEm.pick({ 0, 0, 0 }, TournamentTeam::NinjasInPyjamas);
    pickEm.pick({ 1, 2, 3 }, TournamentTeam::FaZeClan);
    pickEm.pick({ 4, 5, 6 }, TournamentTeam::FURIA);
    pickEm.clear();
    ASSERT_TRUE(pickEm.getPicks().empty());
}

class InventoryChanger_Backend_PickEm_PickTeamTest : public testing::TestWithParam<TournamentTeam> {};

TEST_P(InventoryChanger_Backend_PickEm_PickTeamTest, NewlyPlacedPickHasCorrectTeam) {
    PickEm pickEm;
    pickEm.pick({ 0, 0, 0 }, GetParam());
    ASSERT_EQ(pickEm.getPickedTeam({ 0, 0, 0 }), GetParam());
}

INSTANTIATE_TEST_SUITE_P(, InventoryChanger_Backend_PickEm_PickTeamTest,
    testing::Values(TournamentTeam::ENCE, TournamentTeam::FaZeClan));

TEST(InventoryChanger_Backend_PickEmTest, PickingInAlreadyPickedPositionUpdatesTeam) {
    PickEm pickEm;
    pickEm.pick({ 1, 2, 3 }, TournamentTeam::NatusVincere);
    pickEm.pick({ 1, 2, 3 }, TournamentTeam::FaZeClan);
    ASSERT_EQ(pickEm.getPickedTeam({ 1, 2, 3 }), TournamentTeam::FaZeClan);
}

}
}
