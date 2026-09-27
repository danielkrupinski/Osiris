#include <gtest/gtest.h>

#include <Features/Hud/BombPlantAlert/BombPlantAlertTimer.h>

TEST(BombPlantAlertTimerTest, PositiveSubTenthValueDoesNotDisplayAsZero)
{
    EXPECT_EQ(bomb_plant_alert_timer::remainingTenths(0.01f), 1);
    EXPECT_EQ(bomb_plant_alert_timer::remainingTenths(0.05f), 1);
    EXPECT_EQ(bomb_plant_alert_timer::remainingTenths(0.09f), 1);
}

TEST(BombPlantAlertTimerTest, PositiveValuesAreTruncatedToTenths)
{
    EXPECT_EQ(bomb_plant_alert_timer::remainingTenths(0.1f), 1);
    EXPECT_EQ(bomb_plant_alert_timer::remainingTenths(0.19f), 1);
    EXPECT_EQ(bomb_plant_alert_timer::remainingTenths(3.0f), 30);
    EXPECT_EQ(bomb_plant_alert_timer::remainingTenths(5.55f), 55);
}

TEST(BombPlantAlertTimerTest, NonPositiveValuesAreNotClamped)
{
    EXPECT_EQ(bomb_plant_alert_timer::remainingTenths(0.0f), 0);
    EXPECT_EQ(bomb_plant_alert_timer::remainingTenths(-0.1f), -1);
}
