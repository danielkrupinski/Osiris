#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <CS2/Constants/IconURLs.h>
#include <Features/Hud/BombTimer/BombSiteIconPanel.h>
#include <Mocks/MockImagePanel.h>

class BombSiteIconPanelTest : public testing::Test {
protected:
    testing::StrictMock<MockImagePanel> mockImagePanel;
    BombSiteIconPanel<MockImagePanel&> bombSiteIconPanel{mockImagePanel};
};

TEST_F(BombSiteIconPanelTest, DoesNotSetImageIfUrlIsNullptr) {
    EXPECT_CALL(mockImagePanel, setImageSvg(testing::_)).Times(0);
    bombSiteIconPanel.setIcon(nullptr);
}

class BombSiteIconPanelTestWithParam : public BombSiteIconPanelTest, public testing::WithParamInterface<const char*> {
};

TEST_P(BombSiteIconPanelTestWithParam, SetsCorrectImageUrl) {
    EXPECT_CALL(mockImagePanel, setImageSvg(GetParam()));
    bombSiteIconPanel.setIcon(GetParam());
}

INSTANTIATE_TEST_SUITE_P(, BombSiteIconPanelTestWithParam,
    testing::Values(cs2::kBombSiteAIconUrl, cs2::kBombSiteBIconUrl));
