#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Hud/BombTimer/BombTimerTextPanel.h>
#include <Mocks/MockLabelPanel.h>

struct BombTimerTextPanelTestParam {
    float timeToExplosion;
    const char* expectedString;
};

class BombTimerTextPanelTest : public testing::TestWithParam<BombTimerTextPanelTestParam> {
protected:
    testing::StrictMock<MockLabelPanel> mockLabelPanel;
    BombTimerTextPanel<MockLabelPanel&> bombTimerTextPanel{mockLabelPanel};
};

TEST_P(BombTimerTextPanelTest, TimeToExplosionIsCorrectlySetOnPanel) {
    EXPECT_CALL(mockLabelPanel, setText(testing::StrEq(GetParam().expectedString)));
    bombTimerTextPanel.setTimeToExplosion(GetParam().timeToExplosion);
}

INSTANTIATE_TEST_SUITE_P(, BombTimerTextPanelTest, testing::Values(
    BombTimerTextPanelTestParam{.timeToExplosion = 999.9f, .expectedString = "999.9"},
    BombTimerTextPanelTestParam{.timeToExplosion = 40.0f, .expectedString = "40.0"},
    BombTimerTextPanelTestParam{.timeToExplosion = 11.0f, .expectedString = "11.0"},
    BombTimerTextPanelTestParam{.timeToExplosion = 10.9999f, .expectedString = "10.9"},
    BombTimerTextPanelTestParam{.timeToExplosion = 0.2f, .expectedString = "0.2"},
    BombTimerTextPanelTestParam{.timeToExplosion = 0.19999f, .expectedString = "0.1"},
    BombTimerTextPanelTestParam{.timeToExplosion = 0.0f, .expectedString = "0.0"}
));
