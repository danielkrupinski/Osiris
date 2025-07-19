#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Visuals/OutlineGlow/PlayerOutlineGlow/PlayerOutlineGlow.h>
#include <Mocks/MockConfig.h>
#include <Mocks/MockHookContext.h>
#include <Mocks/MockPlayerController.h>
#include <Mocks/MockPlayerPawn.h>

class PlayerOutlineGlowTest : public testing::Test {
protected:
    PlayerOutlineGlowTest()
    {
        EXPECT_CALL(mockHookContext, config()).WillRepeatedly(testing::ReturnRef(mockConfig));
    }

    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockConfig> mockConfig;
    testing::StrictMock<MockPlayerPawn> mockPlayerPawn;
    testing::StrictMock<MockPlayerController> mockPlayerController;

    PlayerOutlineGlow<MockHookContext> playerOutlineGlow{mockHookContext};
};

struct PlayerOutlineGlowConditionTestParam {
    bool enabled{true};
    bool onlyEnemies{true};
    std::optional<bool> isAlive{true};
    Optional<int> health{100};
    bool isControlledByLocalPlayer{false};
    bool isTTorCT{true};
    std::optional<bool> isEnemy{true};
    bool expectPlayerPawnAccess{true};
    bool expectEnemyCheck{true};
    bool expectGlowApplied{true};
};

class PlayerOutlineGlowConditionTest : public PlayerOutlineGlowTest, public testing::WithParamInterface<PlayerOutlineGlowConditionTestParam> {
};

TEST_P(PlayerOutlineGlowConditionTest, GlowIsAppliedWhenExpected) {
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<PlayerOutlineGlowEnabled>())).WillOnce(testing::Return(GetParam().enabled));
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<PlayerOutlineGlowOnlyEnemies>())).WillRepeatedly(testing::Return(GetParam().onlyEnemies));

    if (GetParam().expectPlayerPawnAccess) {
        EXPECT_CALL(mockPlayerPawn, isAlive()).WillRepeatedly(testing::Return(GetParam().isAlive));
        EXPECT_CALL(mockPlayerPawn, health()).WillRepeatedly(testing::Return(GetParam().health));
        EXPECT_CALL(mockPlayerPawn, isControlledByLocalPlayer()).WillRepeatedly(testing::Return(GetParam().isControlledByLocalPlayer));
        EXPECT_CALL(mockPlayerPawn, isTTorCT()).WillRepeatedly(testing::Return(GetParam().isTTorCT));

        if (GetParam().expectEnemyCheck)
            EXPECT_CALL(mockPlayerPawn, isEnemy()).WillRepeatedly(testing::Return(GetParam().isEnemy));
    }

    EXPECT_EQ(playerOutlineGlow.shouldApplyGlow(EntityTypeInfo{}, mockPlayerPawn), GetParam().expectGlowApplied);
}

INSTANTIATE_TEST_SUITE_P(Disabled, PlayerOutlineGlowConditionTest, testing::Values(
    PlayerOutlineGlowConditionTestParam
    {
        .enabled = false,
        .expectPlayerPawnAccess = false,
        .expectEnemyCheck = false,
        .expectGlowApplied = false 
    }
));

INSTANTIATE_TEST_SUITE_P(OnlyEnemies, PlayerOutlineGlowConditionTest, testing::ValuesIn(
    std::to_array<PlayerOutlineGlowConditionTestParam>({
        {
            .enabled = true,
            .onlyEnemies = true,
            .isEnemy{true},
            .expectEnemyCheck = true,
            .expectGlowApplied = true
        },
        {
            .enabled = true,
            .onlyEnemies = true,
            .isEnemy{false},
            .expectEnemyCheck = true,
            .expectGlowApplied = false
        },
        {
            .enabled = true,
            .onlyEnemies = true,
            .isEnemy{std::nullopt},
            .expectEnemyCheck = true,
            .expectGlowApplied = true
        }
    })
));

INSTANTIATE_TEST_SUITE_P(NotOnlyEnemies, PlayerOutlineGlowConditionTest, testing::ValuesIn(
    std::to_array<PlayerOutlineGlowConditionTestParam>({
        {
            .enabled = true,
            .onlyEnemies = false,
            .isEnemy{true},
            .expectEnemyCheck = false,
            .expectGlowApplied = true
        },
        {
            .enabled = true,
            .onlyEnemies = false,
            .isEnemy{false},
            .expectEnemyCheck = false,
            .expectGlowApplied = true
        },
        {
            .enabled = true,
            .onlyEnemies = false,
            .isEnemy{std::nullopt},
            .expectEnemyCheck = false,
            .expectGlowApplied = true
        }
    })
));

INSTANTIATE_TEST_SUITE_P(IsAlive, PlayerOutlineGlowConditionTest, testing::ValuesIn(
    std::to_array<PlayerOutlineGlowConditionTestParam>({
        {
            .enabled = true,
            .isAlive{true},
            .expectGlowApplied = true
        },
        {
            .enabled = true,
            .isAlive{false},
            .expectGlowApplied = false
        },
        {
            .enabled = true,
            .isAlive{std::nullopt},
            .expectGlowApplied = true
        }
    })
));

INSTANTIATE_TEST_SUITE_P(Health, PlayerOutlineGlowConditionTest, testing::ValuesIn(
    std::to_array<PlayerOutlineGlowConditionTestParam>({
        {
            .enabled = true,
            .health{100},
            .expectGlowApplied = true
        },
        {
            .enabled = true,
            .health{120},
            .expectGlowApplied = true
        },
        {
            .enabled = true,
            .health{0},
            .expectGlowApplied = false
        },
        {
            .enabled = true,
            .health{std::nullopt},
            .expectGlowApplied = true
        }
    })
));

INSTANTIATE_TEST_SUITE_P(IsControlledByLocalPlayer, PlayerOutlineGlowConditionTest, testing::ValuesIn(
    std::to_array<PlayerOutlineGlowConditionTestParam>({
        {
            .enabled = true,
            .isControlledByLocalPlayer = false,
            .expectGlowApplied = true
        },
        {
            .enabled = true,
            .isControlledByLocalPlayer = true,
            .expectGlowApplied = false
        }
    })
));

INSTANTIATE_TEST_SUITE_P(IsTTorCT, PlayerOutlineGlowConditionTest, testing::ValuesIn(
    std::to_array<PlayerOutlineGlowConditionTestParam>({
        {
            .enabled = true,
            .isTTorCT = true,
            .expectGlowApplied = true
        },
        {
            .enabled = true,
            .isTTorCT = false,
            .expectGlowApplied = false
        }
    })
));

struct PlayerOutlineGlowColorTestParam {
    PlayerOutlineGlowColorType colorMode{};
    Optional<int> health{100};
    Optional<cs2::CCSPlayerController::m_iCompTeammateColor> playerColorIndex{};
    TeamNumber teamNumber{};
    Optional<bool> hasImmunity{};
    Optional<color::Hue> expectedHue{};
    std::uint8_t expectedColorAlpha{outline_glow_params::kGlowAlpha};
};

class PlayerOutlineGlowColorTest : public PlayerOutlineGlowTest, public testing::WithParamInterface<PlayerOutlineGlowColorTestParam> {
};

TEST_P(PlayerOutlineGlowColorTest, CorrectGlowColorIsApplied) {
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<PlayerOutlineGlowColorMode>())).WillOnce(testing::Return(GetParam().colorMode));

    EXPECT_CALL(mockPlayerPawn, health()).WillRepeatedly(testing::Return(GetParam().health));
    EXPECT_CALL(mockPlayerPawn, hasImmunity()).WillRepeatedly(testing::Return(GetParam().hasImmunity));
    EXPECT_CALL(mockPlayerPawn, playerController()).WillRepeatedly(testing::ReturnRef(mockPlayerController));
    EXPECT_CALL(mockPlayerPawn, teamNumber()).WillRepeatedly(testing::Return(GetParam().teamNumber));
    EXPECT_CALL(mockPlayerController, playerColorIndex()).WillRepeatedly(testing::Return(GetParam().playerColorIndex));

    const auto hue = playerOutlineGlow.getGlowHue(EntityTypeInfo{}, mockPlayerPawn);
    EXPECT_EQ(hue.hasValue(), GetParam().expectedHue.hasValue());
    if (hue.hasValue() && GetParam().expectedHue.hasValue())
        EXPECT_FLOAT_EQ(hue.value(), GetParam().expectedHue.value());
    EXPECT_EQ(playerOutlineGlow.getGlowColorAlpha(mockPlayerPawn), GetParam().expectedColorAlpha);
}

INSTANTIATE_TEST_SUITE_P(HealthBased, PlayerOutlineGlowColorTest, testing::ValuesIn(
    std::to_array<PlayerOutlineGlowColorTestParam>({
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{100},
            .hasImmunity{std::nullopt},
            .expectedHue{color::kGreenHue}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{120},
            .hasImmunity{std::nullopt},
            .expectedHue{color::kGreenHue}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{75},
            .hasImmunity{std::nullopt},
            .expectedHue{color::Hue{0.2491582f}}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{25},
            .hasImmunity{std::nullopt},
            .expectedHue{color::Hue{0.08080808f}}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{1},
            .hasImmunity{std::nullopt},
            .expectedHue{color::kRedHue}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{std::nullopt},
            .hasImmunity{std::nullopt},
            .expectedHue{}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{100},
            .hasImmunity{false},
            .expectedHue{color::kGreenHue}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{120},
            .hasImmunity{false},
            .expectedHue{color::kGreenHue}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{75},
            .hasImmunity{false},
            .expectedHue{color::Hue{0.2491582f}}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{25},
            .hasImmunity{false},
            .expectedHue{color::Hue{0.08080808f}}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{1},
            .hasImmunity{false},
            .expectedHue{color::kRedHue}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{std::nullopt},
            .hasImmunity{false},
            .expectedHue{}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{100},
            .hasImmunity{true},
            .expectedHue{color::kGreenHue},
            .expectedColorAlpha = outline_glow_params::kImmunePlayerGlowAlpha
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{120},
            .hasImmunity{true},
            .expectedHue{color::kGreenHue},
            .expectedColorAlpha = outline_glow_params::kImmunePlayerGlowAlpha
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{75},
            .hasImmunity{true},
            .expectedHue{color::Hue{0.2491582f}},
            .expectedColorAlpha = outline_glow_params::kImmunePlayerGlowAlpha
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{25},
            .hasImmunity{true},
            .expectedHue{color::Hue{0.08080808f}},
            .expectedColorAlpha = outline_glow_params::kImmunePlayerGlowAlpha
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{1},
            .hasImmunity{true},
            .expectedHue{color::kRedHue},
            .expectedColorAlpha = outline_glow_params::kImmunePlayerGlowAlpha
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{std::nullopt},
            .hasImmunity{true},
            .expectedHue{},
            .expectedColorAlpha = outline_glow_params::kImmunePlayerGlowAlpha
        }
    })
));

INSTANTIATE_TEST_SUITE_P(PlayerOrTeamColorValidColorIndex, PlayerOutlineGlowColorTest, testing::ValuesIn(
    std::to_array<PlayerOutlineGlowColorTestParam>({
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{0},
            .hasImmunity{std::nullopt},
            .expectedHue{outline_glow_params::kPlayerBlueHue.toHueFloat()}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{1},
            .hasImmunity{std::nullopt},
            .expectedHue{outline_glow_params::kPlayerGreenHue.toHueFloat()}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{2},
            .hasImmunity{std::nullopt},
            .expectedHue{outline_glow_params::kPlayerYellowHue.toHueFloat()}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{3},
            .hasImmunity{std::nullopt},
            .expectedHue{outline_glow_params::kPlayerOrangeHue.toHueFloat()}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{4},
            .hasImmunity{std::nullopt},
            .expectedHue{outline_glow_params::kPlayerPurpleHue.toHueFloat()}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{0},
            .hasImmunity{false},
            .expectedHue{outline_glow_params::kPlayerBlueHue.toHueFloat()}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{1},
            .hasImmunity{false},
            .expectedHue{outline_glow_params::kPlayerGreenHue.toHueFloat()}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{2},
            .hasImmunity{false},
            .expectedHue{outline_glow_params::kPlayerYellowHue.toHueFloat()}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{3},
            .hasImmunity{false},
            .expectedHue{outline_glow_params::kPlayerOrangeHue.toHueFloat()}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{4},
            .hasImmunity{false},
            .expectedHue{outline_glow_params::kPlayerPurpleHue.toHueFloat()}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{0},
            .hasImmunity{true},
            .expectedHue{outline_glow_params::kPlayerBlueHue.toHueFloat()},
            .expectedColorAlpha = outline_glow_params::kImmunePlayerGlowAlpha
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{1},
            .hasImmunity{true},
            .expectedHue{outline_glow_params::kPlayerGreenHue.toHueFloat()},
            .expectedColorAlpha = outline_glow_params::kImmunePlayerGlowAlpha
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{2},
            .hasImmunity{true},
            .expectedHue{outline_glow_params::kPlayerYellowHue.toHueFloat()},
            .expectedColorAlpha = outline_glow_params::kImmunePlayerGlowAlpha
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{3},
            .hasImmunity{true},
            .expectedHue{outline_glow_params::kPlayerOrangeHue.toHueFloat()},
            .expectedColorAlpha = outline_glow_params::kImmunePlayerGlowAlpha
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{4},
            .hasImmunity{true},
            .expectedHue{outline_glow_params::kPlayerPurpleHue.toHueFloat()},
            .expectedColorAlpha = outline_glow_params::kImmunePlayerGlowAlpha
        }
    })
));

INSTANTIATE_TEST_SUITE_P(PlayerOrTeamColorInvalidColorIndex, PlayerOutlineGlowColorTest, testing::ValuesIn(
    std::to_array<PlayerOutlineGlowColorTestParam>({
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{-1},
            .teamNumber = TeamNumber::TT,
            .hasImmunity{std::nullopt},
            .expectedHue{outline_glow_params::kTeamTHue.toHueFloat()}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{5},
            .teamNumber = TeamNumber::TT,
            .hasImmunity{std::nullopt},
            .expectedHue{outline_glow_params::kTeamTHue.toHueFloat()}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{-1},
            .teamNumber = TeamNumber::CT,
            .hasImmunity{std::nullopt},
            .expectedHue{outline_glow_params::kTeamCTHue.toHueFloat()}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{5},
            .teamNumber = TeamNumber::CT,
            .hasImmunity{std::nullopt},
            .expectedHue{outline_glow_params::kTeamCTHue.toHueFloat()}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{-1},
            .teamNumber = TeamNumber{},
            .hasImmunity{std::nullopt},
            .expectedHue{}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{5},
            .teamNumber = TeamNumber{},
            .hasImmunity{std::nullopt},
            .expectedHue{}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{-1},
            .teamNumber = TeamNumber::TT,
            .hasImmunity{false},
            .expectedHue{outline_glow_params::kTeamTHue.toHueFloat()}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{5},
            .teamNumber = TeamNumber::TT,
            .hasImmunity{false},
            .expectedHue{outline_glow_params::kTeamTHue.toHueFloat()}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{-1},
            .teamNumber = TeamNumber::CT,
            .hasImmunity{false},
            .expectedHue{outline_glow_params::kTeamCTHue.toHueFloat()}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{5},
            .teamNumber = TeamNumber::CT,
            .hasImmunity{false},
            .expectedHue{outline_glow_params::kTeamCTHue.toHueFloat()}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{-1},
            .teamNumber = TeamNumber{},
            .hasImmunity{false},
            .expectedHue{}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{5},
            .teamNumber = TeamNumber{},
            .hasImmunity{false},
            .expectedHue{}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{-1},
            .teamNumber = TeamNumber::TT,
            .hasImmunity{true},
            .expectedHue{outline_glow_params::kTeamTHue.toHueFloat()},
            .expectedColorAlpha = outline_glow_params::kImmunePlayerGlowAlpha
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{5},
            .teamNumber = TeamNumber::TT,
            .hasImmunity{true},
            .expectedHue{outline_glow_params::kTeamTHue.toHueFloat()},
            .expectedColorAlpha = outline_glow_params::kImmunePlayerGlowAlpha
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{-1},
            .teamNumber = TeamNumber::CT,
            .hasImmunity{true},
            .expectedHue{outline_glow_params::kTeamCTHue.toHueFloat()},
            .expectedColorAlpha = outline_glow_params::kImmunePlayerGlowAlpha
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{5},
            .teamNumber = TeamNumber::CT,
            .hasImmunity{true},
            .expectedHue{outline_glow_params::kTeamCTHue.toHueFloat()},
            .expectedColorAlpha = outline_glow_params::kImmunePlayerGlowAlpha
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{-1},
            .teamNumber = TeamNumber{},
            .hasImmunity{true},
            .expectedHue{},
            .expectedColorAlpha = outline_glow_params::kImmunePlayerGlowAlpha
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{5},
            .teamNumber = TeamNumber{},
            .hasImmunity{true},
            .expectedHue{},
            .expectedColorAlpha = outline_glow_params::kImmunePlayerGlowAlpha
        }
    })
));

INSTANTIATE_TEST_SUITE_P(TeamColor, PlayerOutlineGlowColorTest, testing::ValuesIn(
    std::to_array<PlayerOutlineGlowColorTestParam>({
        {
            .colorMode = PlayerOutlineGlowColorType::TeamColor,
            .teamNumber = TeamNumber::TT,
            .hasImmunity{std::nullopt},
            .expectedHue{outline_glow_params::kTeamTHue.toHueFloat()}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::TeamColor,
            .teamNumber = TeamNumber::CT,
            .hasImmunity{std::nullopt},
            .expectedHue{outline_glow_params::kTeamCTHue.toHueFloat()}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::TeamColor,
            .teamNumber = TeamNumber{},
            .hasImmunity{std::nullopt},
            .expectedHue{}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::TeamColor,
            .teamNumber = TeamNumber::TT,
            .hasImmunity{false},
            .expectedHue{outline_glow_params::kTeamTHue.toHueFloat()}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::TeamColor,
            .teamNumber = TeamNumber::CT,
            .hasImmunity{false},
            .expectedHue{outline_glow_params::kTeamCTHue.toHueFloat()}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::TeamColor,
            .teamNumber = TeamNumber{},
            .hasImmunity{false},
            .expectedHue{}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::TeamColor,
            .teamNumber = TeamNumber::TT,
            .hasImmunity{true},
            .expectedHue{outline_glow_params::kTeamTHue.toHueFloat()},
            .expectedColorAlpha = outline_glow_params::kImmunePlayerGlowAlpha
        },
        {
            .colorMode = PlayerOutlineGlowColorType::TeamColor,
            .teamNumber = TeamNumber::CT,
            .hasImmunity{true},
            .expectedHue{outline_glow_params::kTeamCTHue.toHueFloat()},
            .expectedColorAlpha = outline_glow_params::kImmunePlayerGlowAlpha
        },
        {
            .colorMode = PlayerOutlineGlowColorType::TeamColor,
            .teamNumber = TeamNumber{},
            .hasImmunity{true},
            .expectedHue{},
            .expectedColorAlpha = outline_glow_params::kImmunePlayerGlowAlpha
        }
    })
));
