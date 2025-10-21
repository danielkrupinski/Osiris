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

TEST_F(PlayerOutlineGlowTest, Disabled) {
    mockConfig.expectGetVariable<outline_glow_vars::GlowPlayers>(false);
    EXPECT_FALSE(playerOutlineGlow.enabled());
}

TEST_F(PlayerOutlineGlowTest, Enabled) {
    mockConfig.expectGetVariable<outline_glow_vars::GlowPlayers>(true);
    EXPECT_TRUE(playerOutlineGlow.enabled());
}

struct PlayerOutlineGlowConditionTestParam {
    bool onlyEnemies{true};
    std::optional<bool> isAlive{true};
    Optional<int> health{100};
    bool isControlledByLocalPlayer{false};
    bool isTTorCT{true};
    std::optional<bool> isEnemy{true};
    bool expectEnemyCheck{true};
    bool expectGlowApplied{true};
};

class PlayerOutlineGlowConditionTest
    : public PlayerOutlineGlowTest,
      public testing::WithParamInterface<PlayerOutlineGlowConditionTestParam> {
};

TEST_P(PlayerOutlineGlowConditionTest, GlowIsAppliedWhenExpected) {
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<outline_glow_vars::GlowOnlyEnemies>()))
        .WillRepeatedly(testing::Return(GetParam().onlyEnemies));

    EXPECT_CALL(mockPlayerPawn, isAlive()).WillRepeatedly(testing::Return(GetParam().isAlive));
    EXPECT_CALL(mockPlayerPawn, health()).WillRepeatedly(testing::Return(GetParam().health));
    EXPECT_CALL(mockPlayerPawn, isControlledByLocalPlayer()).WillRepeatedly(testing::Return(GetParam().isControlledByLocalPlayer));
    EXPECT_CALL(mockPlayerPawn, isTTorCT()).WillRepeatedly(testing::Return(GetParam().isTTorCT));

    if (GetParam().expectEnemyCheck)
        EXPECT_CALL(mockPlayerPawn, isEnemy()).WillRepeatedly(testing::Return(GetParam().isEnemy));

    EXPECT_EQ(playerOutlineGlow.shouldApplyGlow(EntityTypeInfo{}, mockPlayerPawn), GetParam().expectGlowApplied);
}

INSTANTIATE_TEST_SUITE_P(OnlyEnemies, PlayerOutlineGlowConditionTest, testing::ValuesIn(
    std::to_array<PlayerOutlineGlowConditionTestParam>({
        {
            .onlyEnemies = true,
            .isEnemy{true},
            .expectEnemyCheck = true,
            .expectGlowApplied = true
        },
        {
            .onlyEnemies = true,
            .isEnemy{false},
            .expectEnemyCheck = true,
            .expectGlowApplied = false
        },
        {
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
            .onlyEnemies = false,
            .isEnemy{true},
            .expectEnemyCheck = false,
            .expectGlowApplied = true
        },
        {
            .onlyEnemies = false,
            .isEnemy{false},
            .expectEnemyCheck = false,
            .expectGlowApplied = true
        },
        {
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
            .isAlive{true},
            .expectGlowApplied = true
        },
        {
            .isAlive{false},
            .expectGlowApplied = false
        },
        {
            .isAlive{std::nullopt},
            .expectGlowApplied = true
        }
    })
));

INSTANTIATE_TEST_SUITE_P(Health, PlayerOutlineGlowConditionTest, testing::ValuesIn(
    std::to_array<PlayerOutlineGlowConditionTestParam>({
        {
            .health{100},
            .expectGlowApplied = true
        },
        {
            .health{120},
            .expectGlowApplied = true
        },
        {
            .health{0},
            .expectGlowApplied = false
        },
        {
            .health{std::nullopt},
            .expectGlowApplied = true
        }
    })
));

INSTANTIATE_TEST_SUITE_P(IsControlledByLocalPlayer, PlayerOutlineGlowConditionTest, testing::ValuesIn(
    std::to_array<PlayerOutlineGlowConditionTestParam>({
        {
            .isControlledByLocalPlayer = false,
            .expectGlowApplied = true
        },
        {
            .isControlledByLocalPlayer = true,
            .expectGlowApplied = false
        }
    })
));

INSTANTIATE_TEST_SUITE_P(IsTTorCT, PlayerOutlineGlowConditionTest, testing::ValuesIn(
    std::to_array<PlayerOutlineGlowConditionTestParam>({
        {
            .isTTorCT = true,
            .expectGlowApplied = true
        },
        {
            .isTTorCT = false,
            .expectGlowApplied = false
        }
    })
));

struct PlayerOutlineGlowHealthBasedHueConfigVariables {
    color::HueInteger::UnderlyingType lowHealthHue;
    color::HueInteger::UnderlyingType highHealthHue;
};

struct PlayerOutlineGlowHealthBasedHueParam {
    Optional<int> health{100};
    Optional<color::Hue::ValueType> expectedHue{};
};

class PlayerOutlineGlowHealthBasedHueTest :
    public PlayerOutlineGlowTest,
    public testing::WithParamInterface<std::tuple<PlayerOutlineGlowHealthBasedHueConfigVariables, PlayerOutlineGlowHealthBasedHueParam>> {
};

TEST_P(PlayerOutlineGlowHealthBasedHueTest, CorrectHueIsReturned) {
    mockConfig.expectGetVariable<outline_glow_vars::PlayerGlowColorMode>(PlayerOutlineGlowColorType::HealthBased);

    const auto [configVariables, param] = GetParam();
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<outline_glow_vars::LowHealthHue>()))
        .WillRepeatedly(testing::Return(outline_glow_vars::LowHealthHue::ValueType{color::HueInteger{configVariables.lowHealthHue}}));
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<outline_glow_vars::HighHealthHue>()))
        .WillRepeatedly(testing::Return(outline_glow_vars::HighHealthHue::ValueType{color::HueInteger{configVariables.highHealthHue}}));
    EXPECT_CALL(mockPlayerPawn, health()).WillOnce(testing::Return(param.health));

    const auto hue = playerOutlineGlow.hue(EntityTypeInfo{}, mockPlayerPawn);
    EXPECT_EQ(hue.hasValue(), param.expectedHue.hasValue());
    if (hue.hasValue() && param.expectedHue.hasValue())
        EXPECT_FLOAT_EQ(hue.value(), param.expectedHue.value());
}

static_assert(outline_glow_vars::LowHealthHue::ValueType::kMin == 0, "Update the tests below");
static_assert(outline_glow_vars::LowHealthHue::ValueType::kMax == 359, "Update the tests below");
static_assert(outline_glow_vars::HighHealthHue::ValueType::kMin == 0, "Update the tests below");
static_assert(outline_glow_vars::HighHealthHue::ValueType::kMax == 359, "Update the tests below");

INSTANTIATE_TEST_SUITE_P(MinMax, PlayerOutlineGlowHealthBasedHueTest, testing::Combine(
    testing::Values(PlayerOutlineGlowHealthBasedHueConfigVariables{
        .lowHealthHue = 0,
        .highHealthHue = 359}),
    testing::ValuesIn(std::to_array<PlayerOutlineGlowHealthBasedHueParam>({
        {.health{std::nullopt}, .expectedHue{std::nullopt}},
        {.health{0}, .expectedHue{0.0f}},
        {.health{1}, .expectedHue{0.0f}},
        {.health{25}, .expectedHue{0.2417508f}},
        {.health{75}, .expectedHue{0.7453984f}},
        {.health{100}, .expectedHue{0.997222f}},
        {.health{120}, .expectedHue{0.997222f}}}))
));

INSTANTIATE_TEST_SUITE_P(MaxMin, PlayerOutlineGlowHealthBasedHueTest, testing::Combine(
    testing::Values(PlayerOutlineGlowHealthBasedHueConfigVariables{
        .lowHealthHue = 359,
        .highHealthHue = 0}),
    testing::ValuesIn(std::to_array<PlayerOutlineGlowHealthBasedHueParam>({
        {.health{std::nullopt}, .expectedHue{std::nullopt}},
        {.health{0}, .expectedHue{0.997222f}},
        {.health{1}, .expectedHue{0.997222f}},
        {.health{25}, .expectedHue{0.75547138f}},
        {.health{75}, .expectedHue{0.25182379f}},
        {.health{100}, .expectedHue{0.0f}},
        {.health{120}, .expectedHue{0.0f}}
    }))
));

INSTANTIATE_TEST_SUITE_P(MinMin, PlayerOutlineGlowHealthBasedHueTest, testing::Combine(
    testing::Values(PlayerOutlineGlowHealthBasedHueConfigVariables{
        .lowHealthHue = 0,
        .highHealthHue = 0}),
    testing::ValuesIn(std::to_array<PlayerOutlineGlowHealthBasedHueParam>({
        {.health{std::nullopt}, .expectedHue{std::nullopt}},
        {.health{0}, .expectedHue{0.0f}},
        {.health{1}, .expectedHue{0.0f}},
        {.health{25}, .expectedHue{0.0f}},
        {.health{75}, .expectedHue{0.0f}},
        {.health{100}, .expectedHue{0.0f}},
        {.health{120}, .expectedHue{0.0f}}
    }))
));

INSTANTIATE_TEST_SUITE_P(MaxMax, PlayerOutlineGlowHealthBasedHueTest, testing::Combine(
    testing::Values(PlayerOutlineGlowHealthBasedHueConfigVariables{
        .lowHealthHue = 359,
        .highHealthHue = 359}),
    testing::ValuesIn(std::to_array<PlayerOutlineGlowHealthBasedHueParam>({
        {.health{std::nullopt}, .expectedHue{std::nullopt}},
        {.health{0}, .expectedHue{0.997222f}},
        {.health{1}, .expectedHue{0.997222f}},
        {.health{25}, .expectedHue{0.997222f}},
        {.health{75}, .expectedHue{0.997222f}},
        {.health{100}, .expectedHue{0.997222f}},
        {.health{120}, .expectedHue{0.997222f}}
    }))
));

static_assert(outline_glow_vars::LowHealthHue::kDefaultValue == color::HueInteger{0}, "Update the tests below");
static_assert(outline_glow_vars::HighHealthHue::kDefaultValue == color::HueInteger{120}, "Update the tests below");

INSTANTIATE_TEST_SUITE_P(Default, PlayerOutlineGlowHealthBasedHueTest, testing::Combine(
    testing::Values(PlayerOutlineGlowHealthBasedHueConfigVariables{
        .lowHealthHue = 0,
        .highHealthHue = 120}),
    testing::ValuesIn(std::to_array<PlayerOutlineGlowHealthBasedHueParam>({
        {.health{std::nullopt}, .expectedHue{std::nullopt}},
        {.health{0}, .expectedHue{0.0f}},
        {.health{1}, .expectedHue{0.0f}},
        {.health{25}, .expectedHue{0.08080808f}},
        {.health{75}, .expectedHue{0.24915824f}},
        {.health{100}, .expectedHue{0.33333333f}},
        {.health{120}, .expectedHue{0.33333333f}}
    }))
));

INSTANTIATE_TEST_SUITE_P(NonDefault, PlayerOutlineGlowHealthBasedHueTest, testing::Combine(
    testing::Values(PlayerOutlineGlowHealthBasedHueConfigVariables{
        .lowHealthHue = 220,
        .highHealthHue = 280}),
    testing::ValuesIn(std::to_array<PlayerOutlineGlowHealthBasedHueParam>({
        {.health{std::nullopt}, .expectedHue{std::nullopt}},
        {.health{0}, .expectedHue{0.61111111f}},
        {.health{1}, .expectedHue{0.61111111f}},
        {.health{25}, .expectedHue{0.65151515f}},
        {.health{75}, .expectedHue{0.73569023f}},
        {.health{100}, .expectedHue{0.77777777f}},
        {.health{120}, .expectedHue{0.77777777f}}
    }))
));

struct PlayerOutlineGlowPlayerColorIndexHueTestParam {
    cs2::CCSPlayerController::m_iCompTeammateColor playerColorIndex{};
    std::size_t configVariableIndex{};
    std::any configuredHue{};
    color::Hue::ValueType expectedHue{};
};

class PlayerOutlineGlowPlayerColorIndexHueTest
    : public PlayerOutlineGlowTest,
      public testing::WithParamInterface<PlayerOutlineGlowPlayerColorIndexHueTestParam> {
};

TEST_P(PlayerOutlineGlowPlayerColorIndexHueTest, CorrectHueIsReturned) {
    mockConfig.expectGetVariable<outline_glow_vars::PlayerGlowColorMode>(PlayerOutlineGlowColorType::PlayerOrTeamColor);

    EXPECT_CALL(mockConfig, getVariable(GetParam().configVariableIndex))
        .WillRepeatedly(testing::Return(GetParam().configuredHue));

    EXPECT_CALL(mockPlayerPawn, playerController()).WillOnce(testing::ReturnRef(mockPlayerController));
    EXPECT_CALL(mockPlayerController, playerColorIndex()).WillOnce(testing::Return(GetParam().playerColorIndex));

    const auto hue = playerOutlineGlow.hue(EntityTypeInfo{}, mockPlayerPawn);
    ASSERT_TRUE(hue.hasValue());
    EXPECT_FLOAT_EQ(hue.value(), GetParam().expectedHue);
}

static_assert(outline_glow_vars::PlayerBlueHue::ValueType::kMin == 191, "Update the test below");
static_assert(outline_glow_vars::PlayerGreenHue::ValueType::kMin == 110, "Update the test below");
static_assert(outline_glow_vars::PlayerYellowHue::ValueType::kMin == 47, "Update the test below");
static_assert(outline_glow_vars::PlayerOrangeHue::ValueType::kMin == 11, "Update the test below");
static_assert(outline_glow_vars::PlayerPurpleHue::ValueType::kMin == 250, "Update the test below");

INSTANTIATE_TEST_SUITE_P(MinConfigVars, PlayerOutlineGlowPlayerColorIndexHueTest,
    testing::ValuesIn(std::to_array<PlayerOutlineGlowPlayerColorIndexHueTestParam>({
        {
            .playerColorIndex = cs2::PlayerColorIndex::Blue,
            .configVariableIndex = ConfigVariableTypes::indexOf<outline_glow_vars::PlayerBlueHue>(),
            .configuredHue{outline_glow_vars::PlayerBlueHue::ValueType{color::HueInteger{191}}},
            .expectedHue = 0.53055555f
        },
        {
            .playerColorIndex = cs2::PlayerColorIndex::Green,
            .configVariableIndex = ConfigVariableTypes::indexOf<outline_glow_vars::PlayerGreenHue>(),
            .configuredHue{outline_glow_vars::PlayerGreenHue::ValueType{color::HueInteger{110}}},
            .expectedHue = 0.30555555f
        },
        {
            .playerColorIndex = cs2::PlayerColorIndex::Yellow,
            .configVariableIndex = ConfigVariableTypes::indexOf<outline_glow_vars::PlayerYellowHue>(),
            .configuredHue{outline_glow_vars::PlayerYellowHue::ValueType{color::HueInteger{47}}},
            .expectedHue = 0.13055555f
        },
        {
            .playerColorIndex = cs2::PlayerColorIndex::Orange,
            .configVariableIndex = ConfigVariableTypes::indexOf<outline_glow_vars::PlayerOrangeHue>(),
            .configuredHue{outline_glow_vars::PlayerOrangeHue::ValueType{color::HueInteger{11}}},
            .expectedHue = 0.03055555f
        },
        {
            .playerColorIndex = cs2::PlayerColorIndex::Purple,
            .configVariableIndex = ConfigVariableTypes::indexOf<outline_glow_vars::PlayerPurpleHue>(),
            .configuredHue{outline_glow_vars::PlayerPurpleHue::ValueType{color::HueInteger{250}}},
            .expectedHue = 0.69444444f
        }
    })
));

static_assert(outline_glow_vars::PlayerBlueHue::ValueType::kMax == 240, "Update the test below");
static_assert(outline_glow_vars::PlayerGreenHue::ValueType::kMax == 140, "Update the test below");
static_assert(outline_glow_vars::PlayerYellowHue::ValueType::kMax == 60, "Update the test below");
static_assert(outline_glow_vars::PlayerOrangeHue::ValueType::kMax == 20, "Update the test below");
static_assert(outline_glow_vars::PlayerPurpleHue::ValueType::kMax == 280, "Update the test below");

INSTANTIATE_TEST_SUITE_P(MaxConfigVars, PlayerOutlineGlowPlayerColorIndexHueTest,
    testing::ValuesIn(std::to_array<PlayerOutlineGlowPlayerColorIndexHueTestParam>({
        {
            .playerColorIndex = cs2::PlayerColorIndex::Blue,
            .configVariableIndex = ConfigVariableTypes::indexOf<outline_glow_vars::PlayerBlueHue>(),
            .configuredHue{outline_glow_vars::PlayerBlueHue::ValueType{color::HueInteger{240}}},
            .expectedHue = 0.66666666f
        },
        {
            .playerColorIndex = cs2::PlayerColorIndex::Green,
            .configVariableIndex = ConfigVariableTypes::indexOf<outline_glow_vars::PlayerGreenHue>(),
            .configuredHue{outline_glow_vars::PlayerGreenHue::ValueType{color::HueInteger{140}}},
            .expectedHue = 0.38888888f
        },
        {
            .playerColorIndex = cs2::PlayerColorIndex::Yellow,
            .configVariableIndex = ConfigVariableTypes::indexOf<outline_glow_vars::PlayerYellowHue>(),
            .configuredHue{outline_glow_vars::PlayerYellowHue::ValueType{color::HueInteger{60}}},
            .expectedHue = 0.16666666f
        },
        {
            .playerColorIndex = cs2::PlayerColorIndex::Orange,
            .configVariableIndex = ConfigVariableTypes::indexOf<outline_glow_vars::PlayerOrangeHue>(),
            .configuredHue{outline_glow_vars::PlayerOrangeHue::ValueType{color::HueInteger{20}}},
            .expectedHue = 0.05555555f
        },
        {
            .playerColorIndex = cs2::PlayerColorIndex::Purple,
            .configVariableIndex = ConfigVariableTypes::indexOf<outline_glow_vars::PlayerPurpleHue>(),
            .configuredHue{outline_glow_vars::PlayerPurpleHue::ValueType{color::HueInteger{280}}},
            .expectedHue = 0.77777777f
        }
    })
));

static_assert(outline_glow_vars::PlayerBlueHue::kDefaultValue == color::HueInteger{215}, "Update the test below");
static_assert(outline_glow_vars::PlayerGreenHue::kDefaultValue == color::HueInteger{125}, "Update the test below");
static_assert(outline_glow_vars::PlayerYellowHue::kDefaultValue == color::HueInteger{53}, "Update the test below");
static_assert(outline_glow_vars::PlayerOrangeHue::kDefaultValue == color::HueInteger{15}, "Update the test below");
static_assert(outline_glow_vars::PlayerPurpleHue::kDefaultValue == color::HueInteger{265}, "Update the test below");

INSTANTIATE_TEST_SUITE_P(DefaultConfigVars, PlayerOutlineGlowPlayerColorIndexHueTest,
    testing::ValuesIn(std::to_array<PlayerOutlineGlowPlayerColorIndexHueTestParam>({
        {
            .playerColorIndex = cs2::PlayerColorIndex::Blue,
            .configVariableIndex = ConfigVariableTypes::indexOf<outline_glow_vars::PlayerBlueHue>(),
            .configuredHue{outline_glow_vars::PlayerBlueHue::ValueType{color::HueInteger{215}}},
            .expectedHue = 0.59722222f
        },
        {
            .playerColorIndex = cs2::PlayerColorIndex::Green,
            .configVariableIndex = ConfigVariableTypes::indexOf<outline_glow_vars::PlayerGreenHue>(),
            .configuredHue{outline_glow_vars::PlayerGreenHue::ValueType{color::HueInteger{125}}},
            .expectedHue = 0.34722222f
        },
        {
            .playerColorIndex = cs2::PlayerColorIndex::Yellow,
            .configVariableIndex = ConfigVariableTypes::indexOf<outline_glow_vars::PlayerYellowHue>(),
            .configuredHue{outline_glow_vars::PlayerYellowHue::ValueType{color::HueInteger{53}}},
            .expectedHue = 0.14722222f
        },
        {
            .playerColorIndex = cs2::PlayerColorIndex::Orange,
            .configVariableIndex = ConfigVariableTypes::indexOf<outline_glow_vars::PlayerOrangeHue>(),
            .configuredHue{outline_glow_vars::PlayerOrangeHue::ValueType{color::HueInteger{15}}},
            .expectedHue = 0.04166666f
        },
        {
            .playerColorIndex = cs2::PlayerColorIndex::Purple,
            .configVariableIndex = ConfigVariableTypes::indexOf<outline_glow_vars::PlayerPurpleHue>(),
            .configuredHue{outline_glow_vars::PlayerPurpleHue::ValueType{color::HueInteger{265}}},
            .expectedHue = 0.73611111f
        }
    })
));

struct PlayerOutlineGlowTeamHueTestParam {
    TeamNumber teamNumber{};
    Optional<color::HueInteger::UnderlyingType> teamTHue{};
    Optional<color::HueInteger::UnderlyingType> teamCTHue{};
    Optional<color::Hue::ValueType> expectedHue{};
};

class PlayerOutlineGlowTeamHueTest
    : public PlayerOutlineGlowTest,
      public testing::WithParamInterface<PlayerOutlineGlowTeamHueTestParam> {
};

TEST_P(PlayerOutlineGlowTeamHueTest, CorrectHueIsReturned) {
    mockConfig.expectGetVariable<outline_glow_vars::PlayerGlowColorMode>(PlayerOutlineGlowColorType::TeamColor);

    if (GetParam().teamTHue.hasValue())
        mockConfig.expectGetVariable<outline_glow_vars::TeamTHue>(outline_glow_vars::TeamTHue::ValueType{color::HueInteger{GetParam().teamTHue.value()}});

    if (GetParam().teamCTHue.hasValue())
        mockConfig.expectGetVariable<outline_glow_vars::TeamCTHue>(outline_glow_vars::TeamCTHue::ValueType{color::HueInteger{GetParam().teamCTHue.value()}});

    EXPECT_CALL(mockPlayerPawn, teamNumber()).WillRepeatedly(testing::Return(GetParam().teamNumber));

    const auto hue = playerOutlineGlow.hue(EntityTypeInfo{}, mockPlayerPawn);
    EXPECT_EQ(hue.hasValue(), GetParam().expectedHue.hasValue());
    if (hue.hasValue() && GetParam().expectedHue.hasValue())
        EXPECT_FLOAT_EQ(hue.value(), GetParam().expectedHue.value());
}

TEST_P(PlayerOutlineGlowTeamHueTest, CorrectHueIsReturnedAsFallbackWhenInvalidPlayerColorIndex) {
    mockConfig.expectGetVariable<outline_glow_vars::PlayerGlowColorMode>(PlayerOutlineGlowColorType::PlayerOrTeamColor);

    if (GetParam().teamTHue.hasValue())
        mockConfig.expectGetVariable<outline_glow_vars::TeamTHue>(outline_glow_vars::TeamTHue::ValueType{color::HueInteger{GetParam().teamTHue.value()}});

    if (GetParam().teamCTHue.hasValue())
        mockConfig.expectGetVariable<outline_glow_vars::TeamCTHue>(outline_glow_vars::TeamCTHue::ValueType{color::HueInteger{GetParam().teamCTHue.value()}});

    EXPECT_CALL(mockPlayerPawn, teamNumber()).WillRepeatedly(testing::Return(GetParam().teamNumber));
    EXPECT_CALL(mockPlayerPawn, playerController()).WillRepeatedly(testing::ReturnRef(mockPlayerController));
    EXPECT_CALL(mockPlayerController, playerColorIndex()).WillRepeatedly(testing::Return(cs2::PlayerColorIndex{-1}));

    const auto hue = playerOutlineGlow.hue(EntityTypeInfo{}, mockPlayerPawn);
    EXPECT_EQ(hue.hasValue(), GetParam().expectedHue.hasValue());
    if (hue.hasValue() && GetParam().expectedHue.hasValue())
        EXPECT_FLOAT_EQ(hue.value(), GetParam().expectedHue.value());
}

INSTANTIATE_TEST_SUITE_P(InvalidTeam, PlayerOutlineGlowTeamHueTest,
    testing::Values(PlayerOutlineGlowTeamHueTestParam{.teamNumber{}, .expectedHue{std::nullopt}}));

static_assert(outline_glow_vars::TeamTHue::ValueType::kMin == 30, "Update the test below");
static_assert(outline_glow_vars::TeamCTHue::ValueType::kMin == 210, "Update the test below");

INSTANTIATE_TEST_SUITE_P(MinConfigVars, PlayerOutlineGlowTeamHueTest,
    testing::ValuesIn(std::to_array<PlayerOutlineGlowTeamHueTestParam>({
        {.teamNumber = TeamNumber::TT, .teamTHue = 30, .expectedHue{0.08333333f}},
        {.teamNumber = TeamNumber::CT, .teamCTHue = 210, .expectedHue{0.58333333f}}
    }))
);

static_assert(outline_glow_vars::TeamTHue::ValueType::kMax == 40, "Update the test below");
static_assert(outline_glow_vars::TeamCTHue::ValueType::kMax == 230, "Update the test below");

INSTANTIATE_TEST_SUITE_P(MaxConfigVars, PlayerOutlineGlowTeamHueTest,
    testing::ValuesIn(std::to_array<PlayerOutlineGlowTeamHueTestParam>({
        {.teamNumber = TeamNumber::TT, .teamTHue = 40, .expectedHue{0.11111111f}},
        {.teamNumber = TeamNumber::CT, .teamCTHue = 230, .expectedHue{0.63888888f}}
    }))
);

static_assert(outline_glow_vars::TeamTHue::kDefaultValue == color::HueInteger{35}, "Update the tests below");
static_assert(outline_glow_vars::TeamCTHue::kDefaultValue == color::HueInteger{220}, "Update the tests below");

INSTANTIATE_TEST_SUITE_P(DefaultConfigVars, PlayerOutlineGlowTeamHueTest,
    testing::ValuesIn(std::to_array<PlayerOutlineGlowTeamHueTestParam>({
        {.teamNumber = TeamNumber::TT, .teamTHue = 35, .expectedHue{0.09722222f}},
        {.teamNumber = TeamNumber::CT, .teamCTHue = 220, .expectedHue{0.61111111f}}
    }))
);

INSTANTIATE_TEST_SUITE_P(NonDefaultConfigVars, PlayerOutlineGlowTeamHueTest,
    testing::ValuesIn(std::to_array<PlayerOutlineGlowTeamHueTestParam>({
        {.teamNumber = TeamNumber::TT, .teamTHue = 37, .expectedHue{0.10277777f}},
        {.teamNumber = TeamNumber::CT, .teamCTHue = 229, .expectedHue{0.63611111f}}
    }))
);

struct PlayerOutlineGlowEnemyHueTestParam {
    std::optional<bool> isEnemy;
    Optional<color::HueInteger::UnderlyingType> enemyHue{};
    Optional<color::HueInteger::UnderlyingType> allyHue{};
    Optional<color::Hue::ValueType> expectedHue{};
};

class PlayerOutlineGlowEnemyHueTest
    : public PlayerOutlineGlowTest,
      public testing::WithParamInterface<PlayerOutlineGlowEnemyHueTestParam> {
};

TEST_P(PlayerOutlineGlowEnemyHueTest, CorrectHueIsReturned) {
    mockConfig.expectGetVariable<outline_glow_vars::PlayerGlowColorMode>(PlayerOutlineGlowColorType::EnemyAlly);

    if (GetParam().enemyHue.hasValue())
        mockConfig.expectGetVariable<outline_glow_vars::EnemyHue>(outline_glow_vars::EnemyHue::ValueType{color::HueInteger{GetParam().enemyHue.value()}});
    if (GetParam().allyHue.hasValue())
        mockConfig.expectGetVariable<outline_glow_vars::AllyHue>(outline_glow_vars::AllyHue::ValueType{color::HueInteger{GetParam().allyHue.value()}});

    EXPECT_CALL(mockPlayerPawn, isEnemy()).WillOnce(testing::Return(GetParam().isEnemy));

    const auto hue = playerOutlineGlow.hue(EntityTypeInfo{}, mockPlayerPawn);
    EXPECT_EQ(hue.hasValue(), GetParam().expectedHue.hasValue());
    if (hue.hasValue() && GetParam().expectedHue.hasValue())
        EXPECT_FLOAT_EQ(hue.value(), GetParam().expectedHue.value());
}

INSTANTIATE_TEST_SUITE_P(UnknownIfIsEnemy, PlayerOutlineGlowEnemyHueTest,
    testing::Values(PlayerOutlineGlowEnemyHueTestParam{.isEnemy{std::nullopt}, .expectedHue{std::nullopt}}));

static_assert(outline_glow_vars::EnemyHue::ValueType::kMin == 0, "Update the test below");
static_assert(outline_glow_vars::AllyHue::ValueType::kMin == 0, "Update the test below");

INSTANTIATE_TEST_SUITE_P(MinConfigVars, PlayerOutlineGlowEnemyHueTest,
    testing::ValuesIn(std::to_array<PlayerOutlineGlowEnemyHueTestParam>({
        {.isEnemy{true}, .enemyHue{0}, .expectedHue{0.0f}},
        {.isEnemy{false}, .allyHue{0}, .expectedHue{0.0f}}
    }))
);

static_assert(outline_glow_vars::EnemyHue::ValueType::kMax == 359, "Update the test below");
static_assert(outline_glow_vars::AllyHue::ValueType::kMax == 359, "Update the test below");

INSTANTIATE_TEST_SUITE_P(MaxConfigVars, PlayerOutlineGlowEnemyHueTest,
    testing::ValuesIn(std::to_array<PlayerOutlineGlowEnemyHueTestParam>({
        {.isEnemy{true}, .enemyHue{359}, .expectedHue{0.99722222f}},
        {.isEnemy{false}, .allyHue{359}, .expectedHue{0.99722222f}}
    }))
);

static_assert(outline_glow_vars::EnemyHue::kDefaultValue == color::HueInteger{0}, "Update the tests below");
static_assert(outline_glow_vars::AllyHue::kDefaultValue == color::HueInteger{120}, "Update the tests below");

INSTANTIATE_TEST_SUITE_P(DefaultConfigVars, PlayerOutlineGlowEnemyHueTest,
    testing::ValuesIn(std::to_array<PlayerOutlineGlowEnemyHueTestParam>({
        {.isEnemy{true}, .enemyHue{0}, .expectedHue{0.0f}},
        {.isEnemy{false}, .allyHue{120}, .expectedHue{0.33333333f}}
    }))
);

INSTANTIATE_TEST_SUITE_P(NonDefaultConfigVars, PlayerOutlineGlowEnemyHueTest,
    testing::ValuesIn(std::to_array<PlayerOutlineGlowEnemyHueTestParam>({
        {.isEnemy{true}, .enemyHue{123}, .expectedHue{0.34166666f}},
        {.isEnemy{false}, .allyHue{234}, .expectedHue{0.65f}}
    }))
);

struct PlayerOutlineGlowAlphaTestParam {
    Optional<bool> hasImmunity{};
    std::uint8_t expectedAlpha{};
};

class PlayerOutlineGlowAlphaTest
    : public PlayerOutlineGlowTest,
      public testing::WithParamInterface<PlayerOutlineGlowAlphaTestParam> {
};

TEST_P(PlayerOutlineGlowAlphaTest, CorrectGlowColorAlphaIsReturned) {
    EXPECT_CALL(mockPlayerPawn, hasImmunity()).WillOnce(testing::Return(GetParam().hasImmunity));
    EXPECT_EQ(playerOutlineGlow.getGlowColorAlpha(mockPlayerPawn), GetParam().expectedAlpha);
}

INSTANTIATE_TEST_SUITE_P(, PlayerOutlineGlowAlphaTest,
    testing::ValuesIn(std::to_array<PlayerOutlineGlowAlphaTestParam>({
        {.hasImmunity{std::nullopt}, .expectedAlpha = outline_glow_params::kGlowAlpha},
        {.hasImmunity{false}, .expectedAlpha = outline_glow_params::kGlowAlpha},
        {.hasImmunity{true}, .expectedAlpha = outline_glow_params::kImmunePlayerGlowAlpha}
    }))
);
