#include <gtest/gtest.h>

#include <Features/Hud/BombPlantAlert/BombPlantAlertTiming.h>

TEST(BombPlantAlertTimingTest, PlantCanFinishExactlyAtRoundEnd)
{
    EXPECT_EQ(bomb_plant_alert_timing::canFinishArmingBeforeRoundEnd(10.0f, 10.0f, false), Optional<bool>{true});
}

TEST(BombPlantAlertTimingTest, PlantMustNotFinishAfterRoundEnd)
{
    EXPECT_EQ(bomb_plant_alert_timing::canFinishArmingBeforeRoundEnd(10.01f, 10.0f, false), Optional<bool>{false});
}

TEST(BombPlantAlertTimingTest, FinishedRoundAlwaysRejectsPlant)
{
    EXPECT_EQ(bomb_plant_alert_timing::canFinishArmingBeforeRoundEnd(9.0f, 10.0f, true), Optional<bool>{false});
}

TEST(BombPlantAlertTimingTest, MissingTimingDataProducesUnknownResult)
{
    EXPECT_EQ(bomb_plant_alert_timing::canFinishArmingBeforeRoundEnd(std::nullopt, 10.0f, false), Optional<bool>{std::nullopt});
    EXPECT_EQ(bomb_plant_alert_timing::canFinishArmingBeforeRoundEnd(9.0f, std::nullopt, false), Optional<bool>{std::nullopt});
}
