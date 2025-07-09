#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <CS2/Classes/Color.h>
#include <CS2/Constants/ColorConstants.h>
#include <Features/Hud/DefusingAlert/DefusingCountdownTextPanel.h>
#include <Mocks/MockLabelPanel.h>
#include <Utils/Optional.h>

class DefusingCountdownTextPanelTest : public testing::Test {
protected:
    testing::StrictMock<MockLabelPanel> mockLabelPanel;
    testing::StrictMock<MockUiPanel> mockUiPanel;
    DefusingCountdownTextPanel<MockLabelPanel&> defusingCountdownTextPanel{mockLabelPanel};
};

struct DefusingCountdownTextPanelTestTimeToDefuseEndParam {
    float timeToDefuseEnd{};
    const char* expectedString{};
};

class DefusingCountdownTextPanelTestWithTimeToDefuseEndParam : public DefusingCountdownTextPanelTest, public testing::WithParamInterface<DefusingCountdownTextPanelTestTimeToDefuseEndParam> {
};

TEST_P(DefusingCountdownTextPanelTestWithTimeToDefuseEndParam, TimeToDefuseEndIsCorrectlySet) {
    EXPECT_CALL(mockLabelPanel, setText(testing::StrEq(GetParam().expectedString)));
    EXPECT_THAT(defusingCountdownTextPanel.setTimeToDefuseEnd(GetParam().timeToDefuseEnd), testing::Ref(defusingCountdownTextPanel));
}

INSTANTIATE_TEST_SUITE_P(, DefusingCountdownTextPanelTestWithTimeToDefuseEndParam, testing::Values(
    DefusingCountdownTextPanelTestTimeToDefuseEndParam{.timeToDefuseEnd = 0.0f, .expectedString = "0.0"},
    DefusingCountdownTextPanelTestTimeToDefuseEndParam{.timeToDefuseEnd = 0.1999f, .expectedString = "0.1"},
    DefusingCountdownTextPanelTestTimeToDefuseEndParam{.timeToDefuseEnd = 2.9999f, .expectedString = "2.9"},
    DefusingCountdownTextPanelTestTimeToDefuseEndParam{.timeToDefuseEnd = 10.0f, .expectedString = "10.0"}
));

struct DefusingCountdownTextPanelTestCanBeDefusedParam {
    Optional<bool> canBeDefused{};
    cs2::Color expectedColor{cs2::kColorBlack};
};

class DefusingCountdownTextPanelTestWithCanBeDefusedParam : public DefusingCountdownTextPanelTest, public testing::WithParamInterface<DefusingCountdownTextPanelTestCanBeDefusedParam> {
};

TEST_P(DefusingCountdownTextPanelTestWithCanBeDefusedParam, PanelColorIsSetDependingOnWhetherDefuseCanSucceed) {
    EXPECT_CALL(mockLabelPanel, uiPanel()).WillOnce(testing::ReturnRef(mockUiPanel));
    EXPECT_CALL(mockUiPanel, setColor(GetParam().expectedColor));
    EXPECT_THAT(defusingCountdownTextPanel.setCanBeDefused(GetParam().canBeDefused), testing::Ref(defusingCountdownTextPanel));
}

INSTANTIATE_TEST_SUITE_P(, DefusingCountdownTextPanelTestWithCanBeDefusedParam, testing::Values(
    DefusingCountdownTextPanelTestCanBeDefusedParam{.canBeDefused{std::nullopt}, .expectedColor{cs2::kColorWhite}},
    DefusingCountdownTextPanelTestCanBeDefusedParam{.canBeDefused{true}, .expectedColor{cs2::kColorGreen}},
    DefusingCountdownTextPanelTestCanBeDefusedParam{.canBeDefused{false}, .expectedColor{cs2::kColorRed}}
));
