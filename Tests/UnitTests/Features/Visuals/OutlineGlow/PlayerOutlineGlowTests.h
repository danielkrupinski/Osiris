#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Visuals/OutlineGlow/PlayerOutlineGlow/PlayerOutlineGlow.h>
#include <Mocks/MockBaseEntity.h>
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
    testing::StrictMock<MockBaseEntity> mockBaseEntity;

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
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<PlayerOutlineGlowEnabled>())).WillRepeatedly(testing::Return(GetParam().enabled));
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<PlayerOutlineGlowOnlyEnemies>())).WillRepeatedly(testing::Return(GetParam().onlyEnemies));
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<PlayerOutlineGlowColorMode>())).WillRepeatedly(testing::Return(PlayerOutlineGlowColorType::HealthBased));

    if (GetParam().expectPlayerPawnAccess) {
        EXPECT_CALL(mockPlayerPawn, baseEntity()).WillRepeatedly(testing::ReturnRef(mockBaseEntity));
        EXPECT_CALL(mockPlayerPawn, isAlive()).WillRepeatedly(testing::Return(GetParam().isAlive));
        EXPECT_CALL(mockPlayerPawn, health()).WillRepeatedly(testing::Return(GetParam().health));
        EXPECT_CALL(mockPlayerPawn, isControlledByLocalPlayer()).WillRepeatedly(testing::Return(GetParam().isControlledByLocalPlayer));
        EXPECT_CALL(mockPlayerPawn, isTTorCT()).WillRepeatedly(testing::Return(GetParam().isTTorCT));
        EXPECT_CALL(mockPlayerPawn, hasImmunity()).WillRepeatedly(testing::Return(false));
    
        if (GetParam().expectEnemyCheck)
            EXPECT_CALL(mockPlayerPawn, isEnemy()).WillRepeatedly(testing::Return(GetParam().isEnemy));
    }

    if (GetParam().expectGlowApplied)
        EXPECT_CALL(mockBaseEntity, applyGlowRecursively(testing::_));

    playerOutlineGlow.applyGlowToPlayer(mockPlayerPawn);
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
    cs2::Color expectedColor{0, 0, 0};
};

class PlayerOutlineGlowColorTest : public PlayerOutlineGlowTest, public testing::WithParamInterface<PlayerOutlineGlowColorTestParam> {
};

TEST_P(PlayerOutlineGlowColorTest, CorrectGlowColorIsApplied) {
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<PlayerOutlineGlowEnabled>())).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<PlayerOutlineGlowOnlyEnemies>())).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<PlayerOutlineGlowColorMode>())).WillRepeatedly(testing::Return(GetParam().colorMode));

    EXPECT_CALL(mockPlayerPawn, baseEntity()).WillRepeatedly(testing::ReturnRef(mockBaseEntity));
    EXPECT_CALL(mockPlayerPawn, isAlive()).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(mockPlayerPawn, health()).WillRepeatedly(testing::Return(GetParam().health));
    EXPECT_CALL(mockPlayerPawn, isControlledByLocalPlayer()).WillRepeatedly(testing::Return(false));
    EXPECT_CALL(mockPlayerPawn, isTTorCT()).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(mockPlayerPawn, hasImmunity()).WillRepeatedly(testing::Return(GetParam().hasImmunity));
    EXPECT_CALL(mockPlayerPawn, isEnemy()).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(mockPlayerPawn, playerController()).WillRepeatedly(testing::ReturnRef(mockPlayerController));
    EXPECT_CALL(mockPlayerPawn, teamNumber()).WillRepeatedly(testing::Return(GetParam().teamNumber));
    EXPECT_CALL(mockPlayerController, playerColorIndex()).WillRepeatedly(testing::Return(GetParam().playerColorIndex));

    EXPECT_CALL(mockBaseEntity, applyGlowRecursively(GetParam().expectedColor));

    playerOutlineGlow.applyGlowToPlayer(mockPlayerPawn);
}

INSTANTIATE_TEST_SUITE_P(HealthBased, PlayerOutlineGlowColorTest, testing::ValuesIn(
    std::to_array<PlayerOutlineGlowColorTestParam>({
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{100},
            .hasImmunity{std::nullopt},
            .expectedColor{color::HSBtoRGB(color::kGreenHue, outline_glow_params::kPlayerGlowSaturation, 1.0f).setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{120},
            .hasImmunity{std::nullopt},
            .expectedColor{color::HSBtoRGB(color::kGreenHue, outline_glow_params::kPlayerGlowSaturation, 1.0f).setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{75},
            .hasImmunity{std::nullopt},
            .expectedColor{color::HSBtoRGB(std::lerp(color::kRedHue, color::kGreenHue, 0.7474f), outline_glow_params::kPlayerGlowSaturation, 1.0f).setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{25},
            .hasImmunity{std::nullopt},
            .expectedColor{color::HSBtoRGB(std::lerp(color::kRedHue, color::kGreenHue, 0.2424f), outline_glow_params::kPlayerGlowSaturation, 1.0f).setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{1},
            .hasImmunity{std::nullopt},
            .expectedColor{color::HSBtoRGB(color::kRedHue, outline_glow_params::kPlayerGlowSaturation, 1.0f).setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{std::nullopt},
            .hasImmunity{std::nullopt},
            .expectedColor{outline_glow_params::kPlayerGlowFallbackColor.setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{100},
            .hasImmunity{false},
            .expectedColor{color::HSBtoRGB(color::kGreenHue, outline_glow_params::kPlayerGlowSaturation, 1.0f).setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{120},
            .hasImmunity{false},
            .expectedColor{color::HSBtoRGB(color::kGreenHue, outline_glow_params::kPlayerGlowSaturation, 1.0f).setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{75},
            .hasImmunity{false},
            .expectedColor{color::HSBtoRGB(std::lerp(color::kRedHue, color::kGreenHue, 0.7474f), outline_glow_params::kPlayerGlowSaturation, 1.0f).setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{25},
            .hasImmunity{false},
            .expectedColor{color::HSBtoRGB(std::lerp(color::kRedHue, color::kGreenHue, 0.2424f), outline_glow_params::kPlayerGlowSaturation, 1.0f).setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{1},
            .hasImmunity{false},
            .expectedColor{color::HSBtoRGB(color::kRedHue, outline_glow_params::kPlayerGlowSaturation, 1.0f).setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{std::nullopt},
            .hasImmunity{false},
            .expectedColor{outline_glow_params::kPlayerGlowFallbackColor.setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{100},
            .hasImmunity{true},
            .expectedColor{color::HSBtoRGB(color::kGreenHue, outline_glow_params::kPlayerGlowSaturation, 1.0f).setAlpha(outline_glow_params::kImmunePlayerGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{120},
            .hasImmunity{true},
            .expectedColor{color::HSBtoRGB(color::kGreenHue, outline_glow_params::kPlayerGlowSaturation, 1.0f).setAlpha(outline_glow_params::kImmunePlayerGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{75},
            .hasImmunity{true},
            .expectedColor{color::HSBtoRGB(std::lerp(color::kRedHue, color::kGreenHue, 0.7474f), outline_glow_params::kPlayerGlowSaturation, 1.0f).setAlpha(outline_glow_params::kImmunePlayerGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{25},
            .hasImmunity{true},
            .expectedColor{color::HSBtoRGB(std::lerp(color::kRedHue, color::kGreenHue, 0.2424f), outline_glow_params::kPlayerGlowSaturation, 1.0f).setAlpha(outline_glow_params::kImmunePlayerGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{1},
            .hasImmunity{true},
            .expectedColor{color::HSBtoRGB(color::kRedHue, outline_glow_params::kPlayerGlowSaturation, 1.0f).setAlpha(outline_glow_params::kImmunePlayerGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::HealthBased,
            .health{std::nullopt},
            .hasImmunity{true},
            .expectedColor{outline_glow_params::kPlayerGlowFallbackColor.setAlpha(outline_glow_params::kImmunePlayerGlowAlpha)}
        }
    })
));

INSTANTIATE_TEST_SUITE_P(PlayerOrTeamColorValidColorIndex, PlayerOutlineGlowColorTest, testing::ValuesIn(
    std::to_array<PlayerOutlineGlowColorTestParam>({
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{0},
            .hasImmunity{std::nullopt},
            .expectedColor{outline_glow_params::kPlayerColors.at(0).setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{1},
            .hasImmunity{std::nullopt},
            .expectedColor{outline_glow_params::kPlayerColors.at(1).setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{2},
            .hasImmunity{std::nullopt},
            .expectedColor{outline_glow_params::kPlayerColors.at(2).setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{3},
            .hasImmunity{std::nullopt},
            .expectedColor{outline_glow_params::kPlayerColors.at(3).setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{4},
            .hasImmunity{std::nullopt},
            .expectedColor{outline_glow_params::kPlayerColors.at(4).setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{0},
            .hasImmunity{false},
            .expectedColor{outline_glow_params::kPlayerColors.at(0).setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{1},
            .hasImmunity{false},
            .expectedColor{outline_glow_params::kPlayerColors.at(1).setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{2},
            .hasImmunity{false},
            .expectedColor{outline_glow_params::kPlayerColors.at(2).setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{3},
            .hasImmunity{false},
            .expectedColor{outline_glow_params::kPlayerColors.at(3).setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{4},
            .hasImmunity{false},
            .expectedColor{outline_glow_params::kPlayerColors.at(4).setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{0},
            .hasImmunity{true},
            .expectedColor{outline_glow_params::kPlayerColors.at(0).setAlpha(outline_glow_params::kImmunePlayerGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{1},
            .hasImmunity{true},
            .expectedColor{outline_glow_params::kPlayerColors.at(1).setAlpha(outline_glow_params::kImmunePlayerGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{2},
            .hasImmunity{true},
            .expectedColor{outline_glow_params::kPlayerColors.at(2).setAlpha(outline_glow_params::kImmunePlayerGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{3},
            .hasImmunity{true},
            .expectedColor{outline_glow_params::kPlayerColors.at(3).setAlpha(outline_glow_params::kImmunePlayerGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{4},
            .hasImmunity{true},
            .expectedColor{outline_glow_params::kPlayerColors.at(4).setAlpha(outline_glow_params::kImmunePlayerGlowAlpha)}
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
            .expectedColor{outline_glow_params::kPlayerGlowColorTeamT.setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{5},
            .teamNumber = TeamNumber::TT,
            .hasImmunity{std::nullopt},
            .expectedColor{outline_glow_params::kPlayerGlowColorTeamT.setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{-1},
            .teamNumber = TeamNumber::CT,
            .hasImmunity{std::nullopt},
            .expectedColor{outline_glow_params::kPlayerGlowColorTeamCT.setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{5},
            .teamNumber = TeamNumber::CT,
            .hasImmunity{std::nullopt},
            .expectedColor{outline_glow_params::kPlayerGlowColorTeamCT.setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{-1},
            .teamNumber = TeamNumber{},
            .hasImmunity{std::nullopt},
            .expectedColor{outline_glow_params::kPlayerGlowFallbackColor.setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{5},
            .teamNumber = TeamNumber{},
            .hasImmunity{std::nullopt},
            .expectedColor{outline_glow_params::kPlayerGlowFallbackColor.setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{-1},
            .teamNumber = TeamNumber::TT,
            .hasImmunity{false},
            .expectedColor{outline_glow_params::kPlayerGlowColorTeamT.setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{5},
            .teamNumber = TeamNumber::TT,
            .hasImmunity{false},
            .expectedColor{outline_glow_params::kPlayerGlowColorTeamT.setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{-1},
            .teamNumber = TeamNumber::CT,
            .hasImmunity{false},
            .expectedColor{outline_glow_params::kPlayerGlowColorTeamCT.setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{5},
            .teamNumber = TeamNumber::CT,
            .hasImmunity{false},
            .expectedColor{outline_glow_params::kPlayerGlowColorTeamCT.setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{-1},
            .teamNumber = TeamNumber{},
            .hasImmunity{false},
            .expectedColor{outline_glow_params::kPlayerGlowFallbackColor.setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{5},
            .teamNumber = TeamNumber{},
            .hasImmunity{false},
            .expectedColor{outline_glow_params::kPlayerGlowFallbackColor.setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{-1},
            .teamNumber = TeamNumber::TT,
            .hasImmunity{true},
            .expectedColor{outline_glow_params::kPlayerGlowColorTeamT.setAlpha(outline_glow_params::kImmunePlayerGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{5},
            .teamNumber = TeamNumber::TT,
            .hasImmunity{true},
            .expectedColor{outline_glow_params::kPlayerGlowColorTeamT.setAlpha(outline_glow_params::kImmunePlayerGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{-1},
            .teamNumber = TeamNumber::CT,
            .hasImmunity{true},
            .expectedColor{outline_glow_params::kPlayerGlowColorTeamCT.setAlpha(outline_glow_params::kImmunePlayerGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{5},
            .teamNumber = TeamNumber::CT,
            .hasImmunity{true},
            .expectedColor{outline_glow_params::kPlayerGlowColorTeamCT.setAlpha(outline_glow_params::kImmunePlayerGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{-1},
            .teamNumber = TeamNumber{},
            .hasImmunity{true},
            .expectedColor{outline_glow_params::kPlayerGlowFallbackColor.setAlpha(outline_glow_params::kImmunePlayerGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor,
            .playerColorIndex{5},
            .teamNumber = TeamNumber{},
            .hasImmunity{true},
            .expectedColor{outline_glow_params::kPlayerGlowFallbackColor.setAlpha(outline_glow_params::kImmunePlayerGlowAlpha)}
        }
    })
));

INSTANTIATE_TEST_SUITE_P(TeamColor, PlayerOutlineGlowColorTest, testing::ValuesIn(
    std::to_array<PlayerOutlineGlowColorTestParam>({
        {
            .colorMode = PlayerOutlineGlowColorType::TeamColor,
            .teamNumber = TeamNumber::TT,
            .hasImmunity{std::nullopt},
            .expectedColor{outline_glow_params::kPlayerGlowColorTeamT.setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::TeamColor,
            .teamNumber = TeamNumber::CT,
            .hasImmunity{std::nullopt},
            .expectedColor{outline_glow_params::kPlayerGlowColorTeamCT.setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::TeamColor,
            .teamNumber = TeamNumber{},
            .hasImmunity{std::nullopt},
            .expectedColor{outline_glow_params::kPlayerGlowFallbackColor.setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::TeamColor,
            .teamNumber = TeamNumber::TT,
            .hasImmunity{false},
            .expectedColor{outline_glow_params::kPlayerGlowColorTeamT.setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::TeamColor,
            .teamNumber = TeamNumber::CT,
            .hasImmunity{false},
            .expectedColor{outline_glow_params::kPlayerGlowColorTeamCT.setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::TeamColor,
            .teamNumber = TeamNumber{},
            .hasImmunity{false},
            .expectedColor{outline_glow_params::kPlayerGlowFallbackColor.setAlpha(outline_glow_params::kGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::TeamColor,
            .teamNumber = TeamNumber::TT,
            .hasImmunity{true},
            .expectedColor{outline_glow_params::kPlayerGlowColorTeamT.setAlpha(outline_glow_params::kImmunePlayerGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::TeamColor,
            .teamNumber = TeamNumber::CT,
            .hasImmunity{true},
            .expectedColor{outline_glow_params::kPlayerGlowColorTeamCT.setAlpha(outline_glow_params::kImmunePlayerGlowAlpha)}
        },
        {
            .colorMode = PlayerOutlineGlowColorType::TeamColor,
            .teamNumber = TeamNumber{},
            .hasImmunity{true},
            .expectedColor{outline_glow_params::kPlayerGlowFallbackColor.setAlpha(outline_glow_params::kImmunePlayerGlowAlpha)}
        }
    })
));
