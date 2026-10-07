#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Visuals/PlayerInfoInWorld/PlayerWeaponIcon/BombIcon/PlayerBombIconPanel.h>
#include <Mocks/MockHookContext.h>
#include <Mocks/MockPanel.h>
#include <Mocks/MockPlayerPawn.h>
#include <Mocks/MockConfig.h>
#include <Mocks/MockC4.h>
#include <Utils/Optional.h>

struct PlayerBombIconPanelTestParam {
    bool bombCarrierIconEnabled{};
    bool bombPlantIconEnabled{};
    bool expectCarriedC4Access{};
    bool isCarryingC4{};
    Optional<bool> isPlantingC4{};
    bool expectChildPanelsAccess{};
    bool expectCarrierIconVisible{};
    bool expectPlantingIconVisible{};
    Visibility expectedVisibility{};    
};

class PlayerBombIconPanelTest : public testing::TestWithParam<std::tuple<cs2::CUIPanel*, PlayerBombIconPanelTestParam>> {
protected:
    PlayerBombIconPanelTest()
    {
        EXPECT_CALL(mockHookContext, config()).WillRepeatedly(testing::ReturnRef(mockConfig));
    }

    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockPanel> mockPanel;
    testing::StrictMock<MockPanel> mockCarrierIconPanel;
    testing::StrictMock<MockPanel> mockPlantingIconPanel;
    testing::StrictMock<MockPlayerPawn> mockPlayerPawn;
    testing::StrictMock<MockConfig> mockConfig;
    testing::StrictMock<MockPanoramaUiPanelChildPanels> mockChildPanels;
    testing::StrictMock<MockC4> mockCarriedC4;
};

TEST_P(PlayerBombIconPanelTest, Update) {
    const auto [panelPointer, params] = GetParam();

    PlayerBombIconPanel<MockHookContext> playerBombIconPanel{mockHookContext, panelPointer};
    EXPECT_CALL(mockHookContext, makePanoramaUiPanel(panelPointer)).WillOnce(testing::ReturnRef(mockPanel));
    EXPECT_CALL(mockPanel, setVisible(params.expectedVisibility == Visibility::Visible));

    mockConfig.expectGetVariable<player_info_vars::BombCarrierIconEnabled>(params.bombCarrierIconEnabled);
    mockConfig.expectGetVariable<player_info_vars::BombPlantIconEnabled>(params.bombPlantIconEnabled);
    
    if (params.expectCarriedC4Access) {
        EXPECT_CALL(mockPlayerPawn, carriedC4()).WillOnce(testing::ReturnRef(mockCarriedC4));
        EXPECT_CALL(mockCarriedC4, operatorBool()).WillRepeatedly(testing::Return(params.isCarryingC4));
        EXPECT_CALL(mockCarriedC4, isBeingPlanted()).Times(testing::AtMost(1)).WillRepeatedly(testing::Return(params.isPlantingC4));
    }

    if (params.expectChildPanelsAccess) {
        EXPECT_CALL(mockPanel, children()).WillOnce(testing::ReturnRef(mockChildPanels));
        EXPECT_CALL(mockChildPanels, operatorSubscript(0)).WillOnce(testing::ReturnRef(mockCarrierIconPanel));
        EXPECT_CALL(mockChildPanels, operatorSubscript(1)).WillOnce(testing::ReturnRef(mockPlantingIconPanel));

        EXPECT_CALL(mockCarrierIconPanel, setVisible(params.expectCarrierIconVisible));
        EXPECT_CALL(mockPlantingIconPanel, setVisible(params.expectPlantingIconVisible));
    }

    EXPECT_EQ(playerBombIconPanel.update(mockPlayerPawn), params.expectedVisibility);
}

INSTANTIATE_TEST_SUITE_P(, PlayerBombIconPanelTest, testing::Combine(
    testing::Values((cs2::CUIPanel*)nullptr, (cs2::CUIPanel*)0x123456789, (cs2::CUIPanel*)0x7FFFFF7FFFFF),
    testing::ValuesIn(std::to_array<PlayerBombIconPanelTestParam>({
        {
            .bombCarrierIconEnabled = false,
            .bombPlantIconEnabled = false,
            .expectCarriedC4Access = false,
            .expectChildPanelsAccess = false,
            .expectedVisibility = Visibility::Hidden
        },
        {
            .bombCarrierIconEnabled = true,
            .bombPlantIconEnabled = false,
            .expectCarriedC4Access = true,
            .isCarryingC4 = false,
            .expectChildPanelsAccess = false,
            .expectedVisibility = Visibility::Hidden
        },
        {
            .bombCarrierIconEnabled = false,
            .bombPlantIconEnabled = true,
            .expectCarriedC4Access = true,
            .isPlantingC4{false},
            .expectChildPanelsAccess = false,
            .expectedVisibility = Visibility::Hidden
        },
        {
            .bombCarrierIconEnabled = false,
            .bombPlantIconEnabled = true,
            .expectCarriedC4Access = true,
            .isPlantingC4{std::nullopt},
            .expectChildPanelsAccess = false,
            .expectedVisibility = Visibility::Hidden
        },
        {
            .bombCarrierIconEnabled = true,
            .bombPlantIconEnabled = true,
            .expectCarriedC4Access = true,
            .isCarryingC4 = false,
            .isPlantingC4{false},
            .expectChildPanelsAccess = false,
            .expectedVisibility = Visibility::Hidden
        },
        {
            .bombCarrierIconEnabled = true,
            .bombPlantIconEnabled = true,
            .expectCarriedC4Access = true,
            .isCarryingC4 = false,
            .isPlantingC4{std::nullopt},
            .expectChildPanelsAccess = false,
            .expectedVisibility = Visibility::Hidden
        },

        {
            .bombCarrierIconEnabled = true,
            .bombPlantIconEnabled = false,
            .expectCarriedC4Access = true,
            .isCarryingC4 = true,
            .expectChildPanelsAccess = true,
            .expectCarrierIconVisible = true,
            .expectPlantingIconVisible = false,
            .expectedVisibility = Visibility::Visible
        },
        {
            .bombCarrierIconEnabled = false,
            .bombPlantIconEnabled = true,
            .expectCarriedC4Access = true,
            .isPlantingC4{true},
            .expectChildPanelsAccess = true,
            .expectCarrierIconVisible = false,
            .expectPlantingIconVisible = true,
            .expectedVisibility = Visibility::Visible
        },
        {
            .bombCarrierIconEnabled = true,
            .bombPlantIconEnabled = true,
            .expectCarriedC4Access = true,
            .isCarryingC4 = true,
            .isPlantingC4{true},
            .expectChildPanelsAccess = true,
            .expectCarrierIconVisible = false,
            .expectPlantingIconVisible = true,
            .expectedVisibility = Visibility::Visible
        },
        {
            .bombCarrierIconEnabled = true,
            .bombPlantIconEnabled = true,
            .expectCarriedC4Access = true,
            .isCarryingC4 = true,
            .isPlantingC4{false},
            .expectChildPanelsAccess = true,
            .expectCarrierIconVisible = true,
            .expectPlantingIconVisible = false,
            .expectedVisibility = Visibility::Visible
        },
        {
            .bombCarrierIconEnabled = true,
            .bombPlantIconEnabled = true,
            .expectCarriedC4Access = true,
            .isCarryingC4 = true,
            .isPlantingC4{std::nullopt},
            .expectChildPanelsAccess = true,
            .expectCarrierIconVisible = true,
            .expectPlantingIconVisible = false,
            .expectedVisibility = Visibility::Visible
        }
    }))
));
