#pragma once

#include <gtest/gtest.h>

#include <Features/Visuals/ModelGlow/PlayerModelGlow/PlayerModelGlow.h>

extern "C" std::uint64_t PlayerPawn_sceneObjectUpdater_asm(cs2::C_CSPlayerPawn* playerPawn, void* unknown, bool unknownBool) noexcept
{
    return 0;
}

class PlayerModelGlowTest : public testing::Test {
protected:
    PlayerModelGlowTest()
    {
        EXPECT_CALL(mockHookContext, config()).WillRepeatedly(testing::ReturnRef(mockConfig));
    }

    testing::StrictMock<MockHookContext> mockHookContext{};
    testing::StrictMock<MockConfig> mockConfig{};
    testing::StrictMock<MockPlayerPawn> mockPlayerPawn{};
    testing::StrictMock<MockPlayerController> mockPlayerController{};
    FeaturesStates featuresStates{};
    PlayerModelGlow<MockHookContext> playerModelGlow{mockHookContext};
};

TEST_F(PlayerModelGlowTest, Disabled) {
    mockConfig.expectGetVariable<model_glow_vars::GlowPlayers>(false);
    EXPECT_FALSE(playerModelGlow.enabled());
}

TEST_F(PlayerModelGlowTest, Enabled) {
    mockConfig.expectGetVariable<model_glow_vars::GlowPlayers>(true);
    EXPECT_TRUE(playerModelGlow.enabled());
}

TEST_F(PlayerModelGlowTest, CorrectDisablingFlagIsReturned) {
    EXPECT_CALL(mockHookContext, featuresStates()).WillOnce(testing::ReturnRef(featuresStates));
    EXPECT_THAT(playerModelGlow.disablingFlag(), testing::Ref(featuresStates.visualFeaturesStates.modelGlowState.playerModelGlowDisabling));
}

TEST_F(PlayerModelGlowTest, CorrectOriginalSceneObjectUpdaterIsReturned) {
    EXPECT_CALL(mockHookContext, featuresStates()).WillOnce(testing::ReturnRef(featuresStates));
    EXPECT_THAT(playerModelGlow.originalSceneObjectUpdater(), testing::Ref(featuresStates.visualFeaturesStates.modelGlowState.originalPlayerPawnSceneObjectUpdater));
}

TEST_F(PlayerModelGlowTest, CorrectReplacementSceneObjectUpdaterIsReturned) {
    EXPECT_EQ(playerModelGlow.replacementSceneObjectUpdater(), &PlayerPawn_sceneObjectUpdater_asm);
}

struct PlayerModelGlowSaturationTestParam {
    Optional<bool> playerHasImmunity{};
    color::Saturation expectedSaturation;
};

class PlayerModelGlowSaturationTest
    : public PlayerModelGlowTest,
      public testing::WithParamInterface<PlayerModelGlowSaturationTestParam> {
};

TEST_P(PlayerModelGlowSaturationTest, CorrectSaturationIsReturned) {
    EXPECT_CALL(mockPlayerPawn, hasImmunity()).WillOnce(testing::Return(GetParam().playerHasImmunity));
    EXPECT_EQ(playerModelGlow.saturation(mockPlayerPawn), GetParam().expectedSaturation);
}

INSTANTIATE_TEST_SUITE_P(, PlayerModelGlowSaturationTest, testing::ValuesIn(
    std::to_array<PlayerModelGlowSaturationTestParam>({
        {.playerHasImmunity{std::nullopt}, .expectedSaturation{model_glow_params::kNormalPlayerColorSaturation}},
        {.playerHasImmunity{false}, .expectedSaturation{model_glow_params::kNormalPlayerColorSaturation}},
        {.playerHasImmunity{true}, .expectedSaturation{model_glow_params::kImmunePlayerColorSaturation}}
    })
));

struct PlayerModelGlowShouldApplyTestParam {
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

class PlayerModelGlowShouldApplyTest
    : public PlayerModelGlowTest,
      public testing::WithParamInterface<PlayerModelGlowShouldApplyTestParam> {
};

TEST_P(PlayerModelGlowShouldApplyTest, GlowIsAppliedWhenExpected) {
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<model_glow_vars::GlowOnlyEnemies>()))
        .WillRepeatedly(testing::Return(GetParam().onlyEnemies));

    if (GetParam().expectPlayerPawnAccess) {
        EXPECT_CALL(mockPlayerPawn, isAlive()).WillRepeatedly(testing::Return(GetParam().isAlive));
        EXPECT_CALL(mockPlayerPawn, health()).WillRepeatedly(testing::Return(GetParam().health));
        EXPECT_CALL(mockPlayerPawn, isControlledByLocalPlayer()).WillRepeatedly(testing::Return(GetParam().isControlledByLocalPlayer));
        EXPECT_CALL(mockPlayerPawn, isTTorCT()).WillRepeatedly(testing::Return(GetParam().isTTorCT));

        if (GetParam().expectEnemyCheck)
            EXPECT_CALL(mockPlayerPawn, isEnemy()).WillRepeatedly(testing::Return(GetParam().isEnemy));
    }

    EXPECT_EQ(playerModelGlow.shouldApplyGlow(mockPlayerPawn), GetParam().expectGlowApplied);
}

INSTANTIATE_TEST_SUITE_P(OnlyEnemies, PlayerModelGlowShouldApplyTest, testing::ValuesIn(
    std::to_array<PlayerModelGlowShouldApplyTestParam>({
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

INSTANTIATE_TEST_SUITE_P(NotOnlyEnemies, PlayerModelGlowShouldApplyTest, testing::ValuesIn(
    std::to_array<PlayerModelGlowShouldApplyTestParam>({
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

INSTANTIATE_TEST_SUITE_P(IsAlive, PlayerModelGlowShouldApplyTest, testing::ValuesIn(
    std::to_array<PlayerModelGlowShouldApplyTestParam>({
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

INSTANTIATE_TEST_SUITE_P(Health, PlayerModelGlowShouldApplyTest, testing::ValuesIn(
    std::to_array<PlayerModelGlowShouldApplyTestParam>({
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

INSTANTIATE_TEST_SUITE_P(IsControlledByLocalPlayer, PlayerModelGlowShouldApplyTest, testing::ValuesIn(
    std::to_array<PlayerModelGlowShouldApplyTestParam>({
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

INSTANTIATE_TEST_SUITE_P(IsTTorCT, PlayerModelGlowShouldApplyTest, testing::ValuesIn(
    std::to_array<PlayerModelGlowShouldApplyTestParam>({
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

struct PlayerModelGlowHealthBasedHueConfigVariables {
    color::HueInteger::UnderlyingType lowHealthHue;
    color::HueInteger::UnderlyingType highHealthHue;
};

struct PlayerModelGlowHealthBasedHueParam {
    Optional<int> health{100};
    Optional<color::Hue::ValueType> expectedHue{};
};

class PlayerModelGlowHealthBasedHueTest
    : public PlayerModelGlowTest,
      public testing::WithParamInterface<std::tuple<PlayerModelGlowHealthBasedHueConfigVariables, PlayerModelGlowHealthBasedHueParam>> {
};

TEST_P(PlayerModelGlowHealthBasedHueTest, CorrectHueIsReturned) {
    mockConfig.expectGetVariable<model_glow_vars::PlayerGlowColorMode>(PlayerModelGlowColorType::HealthBased);

    const auto [configVariables, param] = GetParam();
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<model_glow_vars::LowHealthHue>()))
        .WillRepeatedly(testing::Return(model_glow_vars::LowHealthHue::ValueType{color::HueInteger{configVariables.lowHealthHue}}));
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<model_glow_vars::HighHealthHue>()))
        .WillRepeatedly(testing::Return(model_glow_vars::HighHealthHue::ValueType{color::HueInteger{configVariables.highHealthHue}}));
    EXPECT_CALL(mockPlayerPawn, health()).WillOnce(testing::Return(param.health));

    const auto hue = playerModelGlow.hue(mockPlayerPawn);
    EXPECT_EQ(hue.hasValue(), param.expectedHue.hasValue());
    if (hue.hasValue() && param.expectedHue.hasValue())
        EXPECT_FLOAT_EQ(hue.value(), param.expectedHue.value());
}

static_assert(model_glow_vars::LowHealthHue::ValueType::kMin == 0, "Update the tests below");
static_assert(model_glow_vars::LowHealthHue::ValueType::kMax == 359, "Update the tests below");
static_assert(model_glow_vars::HighHealthHue::ValueType::kMin == 0, "Update the tests below");
static_assert(model_glow_vars::HighHealthHue::ValueType::kMax == 359, "Update the tests below");

INSTANTIATE_TEST_SUITE_P(MinMax, PlayerModelGlowHealthBasedHueTest, testing::Combine(
    testing::Values(PlayerModelGlowHealthBasedHueConfigVariables{
        .lowHealthHue = 0,
        .highHealthHue = 359}),
    testing::ValuesIn(std::to_array<PlayerModelGlowHealthBasedHueParam>({
        {.health{std::nullopt}, .expectedHue{std::nullopt}},
        {.health{0}, .expectedHue{0.0f}},
        {.health{1}, .expectedHue{0.0f}},
        {.health{25}, .expectedHue{0.2417508f}},
        {.health{75}, .expectedHue{0.7453984f}},
        {.health{100}, .expectedHue{0.997222f}},
        {.health{120}, .expectedHue{0.997222f}}}))
));

INSTANTIATE_TEST_SUITE_P(MaxMin, PlayerModelGlowHealthBasedHueTest, testing::Combine(
    testing::Values(PlayerModelGlowHealthBasedHueConfigVariables{
        .lowHealthHue = 359,
        .highHealthHue = 0}),
    testing::ValuesIn(std::to_array<PlayerModelGlowHealthBasedHueParam>({
        {.health{std::nullopt}, .expectedHue{std::nullopt}},
        {.health{0}, .expectedHue{0.997222f}},
        {.health{1}, .expectedHue{0.997222f}},
        {.health{25}, .expectedHue{0.75547138f}},
        {.health{75}, .expectedHue{0.25182379f}},
        {.health{100}, .expectedHue{0.0f}},
        {.health{120}, .expectedHue{0.0f}}
    }))
));

INSTANTIATE_TEST_SUITE_P(MinMin, PlayerModelGlowHealthBasedHueTest, testing::Combine(
    testing::Values(PlayerModelGlowHealthBasedHueConfigVariables{
        .lowHealthHue = 0,
        .highHealthHue = 0}),
    testing::ValuesIn(std::to_array<PlayerModelGlowHealthBasedHueParam>({
        {.health{std::nullopt}, .expectedHue{std::nullopt}},
        {.health{0}, .expectedHue{0.0f}},
        {.health{1}, .expectedHue{0.0f}},
        {.health{25}, .expectedHue{0.0f}},
        {.health{75}, .expectedHue{0.0f}},
        {.health{100}, .expectedHue{0.0f}},
        {.health{120}, .expectedHue{0.0f}}
    }))
));

INSTANTIATE_TEST_SUITE_P(MaxMax, PlayerModelGlowHealthBasedHueTest, testing::Combine(
    testing::Values(PlayerModelGlowHealthBasedHueConfigVariables{
        .lowHealthHue = 359,
        .highHealthHue = 359}),
    testing::ValuesIn(std::to_array<PlayerModelGlowHealthBasedHueParam>({
        {.health{std::nullopt}, .expectedHue{std::nullopt}},
        {.health{0}, .expectedHue{0.997222f}},
        {.health{1}, .expectedHue{0.997222f}},
        {.health{25}, .expectedHue{0.997222f}},
        {.health{75}, .expectedHue{0.997222f}},
        {.health{100}, .expectedHue{0.997222f}},
        {.health{120}, .expectedHue{0.997222f}}
    }))
));

static_assert(model_glow_vars::LowHealthHue::kDefaultValue == color::HueInteger{0}, "Update the tests below");
static_assert(model_glow_vars::HighHealthHue::kDefaultValue == color::HueInteger{120}, "Update the tests below");

INSTANTIATE_TEST_SUITE_P(Default, PlayerModelGlowHealthBasedHueTest, testing::Combine(
    testing::Values(PlayerModelGlowHealthBasedHueConfigVariables{
        .lowHealthHue = 0,
        .highHealthHue = 120}),
    testing::ValuesIn(std::to_array<PlayerModelGlowHealthBasedHueParam>({
        {.health{std::nullopt}, .expectedHue{std::nullopt}},
        {.health{0}, .expectedHue{0.0f}},
        {.health{1}, .expectedHue{0.0f}},
        {.health{25}, .expectedHue{0.08080808f}},
        {.health{75}, .expectedHue{0.24915824f}},
        {.health{100}, .expectedHue{0.33333333f}},
        {.health{120}, .expectedHue{0.33333333f}}
    }))
));

INSTANTIATE_TEST_SUITE_P(NonDefault, PlayerModelGlowHealthBasedHueTest, testing::Combine(
    testing::Values(PlayerModelGlowHealthBasedHueConfigVariables{
        .lowHealthHue = 220,
        .highHealthHue = 280}),
    testing::ValuesIn(std::to_array<PlayerModelGlowHealthBasedHueParam>({
        {.health{std::nullopt}, .expectedHue{std::nullopt}},
        {.health{0}, .expectedHue{0.61111111f}},
        {.health{1}, .expectedHue{0.61111111f}},
        {.health{25}, .expectedHue{0.65151515f}},
        {.health{75}, .expectedHue{0.73569023f}},
        {.health{100}, .expectedHue{0.77777777f}},
        {.health{120}, .expectedHue{0.77777777f}}
    }))
));

struct PlayerModelGlowPlayerColorIndexHueTestParam {
    cs2::CCSPlayerController::m_iCompTeammateColor playerColorIndex{};
    std::size_t configVariableIndex{};
    std::any configuredHue{};
    color::Hue::ValueType expectedHue{};
};

class PlayerModelGlowPlayerColorIndexHueTest
    : public PlayerModelGlowTest,
      public testing::WithParamInterface<PlayerModelGlowPlayerColorIndexHueTestParam> {
};

TEST_P(PlayerModelGlowPlayerColorIndexHueTest, CorrectHueIsReturned) {
    mockConfig.expectGetVariable<model_glow_vars::PlayerGlowColorMode>(PlayerModelGlowColorType::PlayerOrTeamColor);

    EXPECT_CALL(mockConfig, getVariable(GetParam().configVariableIndex))
        .WillRepeatedly(testing::Return(GetParam().configuredHue));

    EXPECT_CALL(mockPlayerPawn, playerController()).WillOnce(testing::ReturnRef(mockPlayerController));
    EXPECT_CALL(mockPlayerController, playerColorIndex()).WillOnce(testing::Return(GetParam().playerColorIndex));

    const auto hue = playerModelGlow.hue(mockPlayerPawn);
    ASSERT_TRUE(hue.hasValue());
    EXPECT_FLOAT_EQ(hue.value(), GetParam().expectedHue);
}

static_assert(model_glow_vars::PlayerBlueHue::ValueType::kMin == 191, "Update the test below");
static_assert(model_glow_vars::PlayerGreenHue::ValueType::kMin == 110, "Update the test below");
static_assert(model_glow_vars::PlayerYellowHue::ValueType::kMin == 47, "Update the test below");
static_assert(model_glow_vars::PlayerOrangeHue::ValueType::kMin == 11, "Update the test below");
static_assert(model_glow_vars::PlayerPurpleHue::ValueType::kMin == 250, "Update the test below");

INSTANTIATE_TEST_SUITE_P(MinConfigVars, PlayerModelGlowPlayerColorIndexHueTest,
    testing::ValuesIn(std::to_array<PlayerModelGlowPlayerColorIndexHueTestParam>({
        {
            .playerColorIndex = cs2::PlayerColorIndex::Blue,
            .configVariableIndex = ConfigVariableTypes::indexOf<model_glow_vars::PlayerBlueHue>(),
            .configuredHue{model_glow_vars::PlayerBlueHue::ValueType{color::HueInteger{191}}},
            .expectedHue = 0.53055555f
        },
        {
            .playerColorIndex = cs2::PlayerColorIndex::Green,
            .configVariableIndex = ConfigVariableTypes::indexOf<model_glow_vars::PlayerGreenHue>(),
            .configuredHue{model_glow_vars::PlayerGreenHue::ValueType{color::HueInteger{110}}},
            .expectedHue = 0.30555555f
        },
        {
            .playerColorIndex = cs2::PlayerColorIndex::Yellow,
            .configVariableIndex = ConfigVariableTypes::indexOf<model_glow_vars::PlayerYellowHue>(),
            .configuredHue{model_glow_vars::PlayerYellowHue::ValueType{color::HueInteger{47}}},
            .expectedHue = 0.13055555f
        },
        {
            .playerColorIndex = cs2::PlayerColorIndex::Orange,
            .configVariableIndex = ConfigVariableTypes::indexOf<model_glow_vars::PlayerOrangeHue>(),
            .configuredHue{model_glow_vars::PlayerOrangeHue::ValueType{color::HueInteger{11}}},
            .expectedHue = 0.03055555f
        },
        {
            .playerColorIndex = cs2::PlayerColorIndex::Purple,
            .configVariableIndex = ConfigVariableTypes::indexOf<model_glow_vars::PlayerPurpleHue>(),
            .configuredHue{model_glow_vars::PlayerPurpleHue::ValueType{color::HueInteger{250}}},
            .expectedHue = 0.69444444f
        }
    })
));

static_assert(model_glow_vars::PlayerBlueHue::ValueType::kMax == 240, "Update the test below");
static_assert(model_glow_vars::PlayerGreenHue::ValueType::kMax == 140, "Update the test below");
static_assert(model_glow_vars::PlayerYellowHue::ValueType::kMax == 60, "Update the test below");
static_assert(model_glow_vars::PlayerOrangeHue::ValueType::kMax == 20, "Update the test below");
static_assert(model_glow_vars::PlayerPurpleHue::ValueType::kMax == 280, "Update the test below");

INSTANTIATE_TEST_SUITE_P(MaxConfigVars, PlayerModelGlowPlayerColorIndexHueTest,
    testing::ValuesIn(std::to_array<PlayerModelGlowPlayerColorIndexHueTestParam>({
        {
            .playerColorIndex = cs2::PlayerColorIndex::Blue,
            .configVariableIndex = ConfigVariableTypes::indexOf<model_glow_vars::PlayerBlueHue>(),
            .configuredHue{model_glow_vars::PlayerBlueHue::ValueType{color::HueInteger{240}}},
            .expectedHue = 0.66666666f
        },
        {
            .playerColorIndex = cs2::PlayerColorIndex::Green,
            .configVariableIndex = ConfigVariableTypes::indexOf<model_glow_vars::PlayerGreenHue>(),
            .configuredHue{model_glow_vars::PlayerGreenHue::ValueType{color::HueInteger{140}}},
            .expectedHue = 0.38888888f
        },
        {
            .playerColorIndex = cs2::PlayerColorIndex::Yellow,
            .configVariableIndex = ConfigVariableTypes::indexOf<model_glow_vars::PlayerYellowHue>(),
            .configuredHue{model_glow_vars::PlayerYellowHue::ValueType{color::HueInteger{60}}},
            .expectedHue = 0.16666666f
        },
        {
            .playerColorIndex = cs2::PlayerColorIndex::Orange,
            .configVariableIndex = ConfigVariableTypes::indexOf<model_glow_vars::PlayerOrangeHue>(),
            .configuredHue{model_glow_vars::PlayerOrangeHue::ValueType{color::HueInteger{20}}},
            .expectedHue = 0.05555555f
        },
        {
            .playerColorIndex = cs2::PlayerColorIndex::Purple,
            .configVariableIndex = ConfigVariableTypes::indexOf<model_glow_vars::PlayerPurpleHue>(),
            .configuredHue{model_glow_vars::PlayerPurpleHue::ValueType{color::HueInteger{280}}},
            .expectedHue = 0.77777777f
        }
    })
));

static_assert(model_glow_vars::PlayerBlueHue::kDefaultValue == color::HueInteger{215}, "Update the test below");
static_assert(model_glow_vars::PlayerGreenHue::kDefaultValue == color::HueInteger{125}, "Update the test below");
static_assert(model_glow_vars::PlayerYellowHue::kDefaultValue == color::HueInteger{53}, "Update the test below");
static_assert(model_glow_vars::PlayerOrangeHue::kDefaultValue == color::HueInteger{15}, "Update the test below");
static_assert(model_glow_vars::PlayerPurpleHue::kDefaultValue == color::HueInteger{265}, "Update the test below");

INSTANTIATE_TEST_SUITE_P(DefaultConfigVars, PlayerModelGlowPlayerColorIndexHueTest,
    testing::ValuesIn(std::to_array<PlayerModelGlowPlayerColorIndexHueTestParam>({
        {
            .playerColorIndex = cs2::PlayerColorIndex::Blue,
            .configVariableIndex = ConfigVariableTypes::indexOf<model_glow_vars::PlayerBlueHue>(),
            .configuredHue{model_glow_vars::PlayerBlueHue::ValueType{color::HueInteger{215}}},
            .expectedHue = 0.59722222f
        },
        {
            .playerColorIndex = cs2::PlayerColorIndex::Green,
            .configVariableIndex = ConfigVariableTypes::indexOf<model_glow_vars::PlayerGreenHue>(),
            .configuredHue{model_glow_vars::PlayerGreenHue::ValueType{color::HueInteger{125}}},
            .expectedHue = 0.34722222f
        },
        {
            .playerColorIndex = cs2::PlayerColorIndex::Yellow,
            .configVariableIndex = ConfigVariableTypes::indexOf<model_glow_vars::PlayerYellowHue>(),
            .configuredHue{model_glow_vars::PlayerYellowHue::ValueType{color::HueInteger{53}}},
            .expectedHue = 0.14722222f
        },
        {
            .playerColorIndex = cs2::PlayerColorIndex::Orange,
            .configVariableIndex = ConfigVariableTypes::indexOf<model_glow_vars::PlayerOrangeHue>(),
            .configuredHue{model_glow_vars::PlayerOrangeHue::ValueType{color::HueInteger{15}}},
            .expectedHue = 0.04166666f
        },
        {
            .playerColorIndex = cs2::PlayerColorIndex::Purple,
            .configVariableIndex = ConfigVariableTypes::indexOf<model_glow_vars::PlayerPurpleHue>(),
            .configuredHue{model_glow_vars::PlayerPurpleHue::ValueType{color::HueInteger{265}}},
            .expectedHue = 0.73611111f
        }
    })
));

struct PlayerModelGlowTeamHueTestParam {
    TeamNumber teamNumber{};
    Optional<color::HueInteger::UnderlyingType> teamTHue{};
    Optional<color::HueInteger::UnderlyingType> teamCTHue{};
    Optional<color::Hue::ValueType> expectedHue{};
};

class PlayerModelGlowTeamHueTest
    : public PlayerModelGlowTest,
      public testing::WithParamInterface<PlayerModelGlowTeamHueTestParam> {
};

TEST_P(PlayerModelGlowTeamHueTest, CorrectHueIsReturned) {
    mockConfig.expectGetVariable<model_glow_vars::PlayerGlowColorMode>(PlayerModelGlowColorType::TeamColor);

    if (GetParam().teamTHue.hasValue())
        mockConfig.expectGetVariable<model_glow_vars::TeamTHue>(model_glow_vars::TeamTHue::ValueType{color::HueInteger{GetParam().teamTHue.value()}});

    if (GetParam().teamCTHue.hasValue())
        mockConfig.expectGetVariable<model_glow_vars::TeamCTHue>(model_glow_vars::TeamCTHue::ValueType{color::HueInteger{GetParam().teamCTHue.value()}});

    EXPECT_CALL(mockPlayerPawn, teamNumber()).WillRepeatedly(testing::Return(GetParam().teamNumber));

    const auto hue = playerModelGlow.hue(mockPlayerPawn);
    EXPECT_EQ(hue.hasValue(), GetParam().expectedHue.hasValue());
    if (hue.hasValue() && GetParam().expectedHue.hasValue())
        EXPECT_FLOAT_EQ(hue.value(), GetParam().expectedHue.value());
}

TEST_P(PlayerModelGlowTeamHueTest, CorrectHueIsReturnedAsFallbackWhenInvalidPlayerColorIndex) {
    mockConfig.expectGetVariable<model_glow_vars::PlayerGlowColorMode>(PlayerModelGlowColorType::PlayerOrTeamColor);

    if (GetParam().teamTHue.hasValue())
        mockConfig.expectGetVariable<model_glow_vars::TeamTHue>(model_glow_vars::TeamTHue::ValueType{color::HueInteger{GetParam().teamTHue.value()}});

    if (GetParam().teamCTHue.hasValue())
        mockConfig.expectGetVariable<model_glow_vars::TeamCTHue>(model_glow_vars::TeamCTHue::ValueType{color::HueInteger{GetParam().teamCTHue.value()}});

    EXPECT_CALL(mockPlayerPawn, teamNumber()).WillRepeatedly(testing::Return(GetParam().teamNumber));
    EXPECT_CALL(mockPlayerPawn, playerController()).WillRepeatedly(testing::ReturnRef(mockPlayerController));
    EXPECT_CALL(mockPlayerController, playerColorIndex()).WillRepeatedly(testing::Return(cs2::PlayerColorIndex{-1}));

    const auto hue = playerModelGlow.hue(mockPlayerPawn);
    EXPECT_EQ(hue.hasValue(), GetParam().expectedHue.hasValue());
    if (hue.hasValue() && GetParam().expectedHue.hasValue())
        EXPECT_FLOAT_EQ(hue.value(), GetParam().expectedHue.value());
}

INSTANTIATE_TEST_SUITE_P(InvalidTeam, PlayerModelGlowTeamHueTest,
    testing::Values(PlayerModelGlowTeamHueTestParam{.teamNumber{}, .expectedHue{std::nullopt}}));

static_assert(model_glow_vars::TeamTHue::ValueType::kMin == 30, "Update the test below");
static_assert(model_glow_vars::TeamCTHue::ValueType::kMin == 210, "Update the test below");

INSTANTIATE_TEST_SUITE_P(MinConfigVars, PlayerModelGlowTeamHueTest,
    testing::ValuesIn(std::to_array<PlayerModelGlowTeamHueTestParam>({
        {.teamNumber = TeamNumber::TT, .teamTHue = 30, .expectedHue{0.08333333f}},
        {.teamNumber = TeamNumber::CT, .teamCTHue = 210, .expectedHue{0.58333333f}}
    }))
);

static_assert(model_glow_vars::TeamTHue::ValueType::kMax == 40, "Update the test below");
static_assert(model_glow_vars::TeamCTHue::ValueType::kMax == 230, "Update the test below");

INSTANTIATE_TEST_SUITE_P(MaxConfigVars, PlayerModelGlowTeamHueTest,
    testing::ValuesIn(std::to_array<PlayerModelGlowTeamHueTestParam>({
        {.teamNumber = TeamNumber::TT, .teamTHue = 40, .expectedHue{0.11111111f}},
        {.teamNumber = TeamNumber::CT, .teamCTHue = 230, .expectedHue{0.63888888f}}
    }))
);

static_assert(model_glow_vars::TeamTHue::kDefaultValue == color::HueInteger{35}, "Update the tests below");
static_assert(model_glow_vars::TeamCTHue::kDefaultValue == color::HueInteger{220}, "Update the tests below");

INSTANTIATE_TEST_SUITE_P(DefaultConfigVars, PlayerModelGlowTeamHueTest,
    testing::ValuesIn(std::to_array<PlayerModelGlowTeamHueTestParam>({
        {.teamNumber = TeamNumber::TT, .teamTHue = 35, .expectedHue{0.09722222f}},
        {.teamNumber = TeamNumber::CT, .teamCTHue = 220, .expectedHue{0.61111111f}}
    }))
);

INSTANTIATE_TEST_SUITE_P(NonDefaultConfigVars, PlayerModelGlowTeamHueTest,
    testing::ValuesIn(std::to_array<PlayerModelGlowTeamHueTestParam>({
        {.teamNumber = TeamNumber::TT, .teamTHue = 37, .expectedHue{0.10277777f}},
        {.teamNumber = TeamNumber::CT, .teamCTHue = 229, .expectedHue{0.63611111f}}
    }))
);

struct PlayerModelGlowEnemyHueTestParam {
    std::optional<bool> isEnemy;
    Optional<color::HueInteger::UnderlyingType> enemyHue{};
    Optional<color::HueInteger::UnderlyingType> allyHue{};
    Optional<color::Hue::ValueType> expectedHue{};
};

class PlayerModelGlowEnemyHueTest
    : public PlayerModelGlowTest,
      public testing::WithParamInterface<PlayerModelGlowEnemyHueTestParam> {
};

TEST_P(PlayerModelGlowEnemyHueTest, CorrectHueIsReturned) {
    mockConfig.expectGetVariable<model_glow_vars::PlayerGlowColorMode>(PlayerModelGlowColorType::EnemyAlly);

    if (GetParam().enemyHue.hasValue())
        mockConfig.expectGetVariable<model_glow_vars::EnemyHue>(model_glow_vars::EnemyHue::ValueType{color::HueInteger{GetParam().enemyHue.value()}});
    if (GetParam().allyHue.hasValue())
        mockConfig.expectGetVariable<model_glow_vars::AllyHue>(model_glow_vars::AllyHue::ValueType{color::HueInteger{GetParam().allyHue.value()}});

    EXPECT_CALL(mockPlayerPawn, isEnemy()).WillOnce(testing::Return(GetParam().isEnemy));

    const auto hue = playerModelGlow.hue(mockPlayerPawn);
    EXPECT_EQ(hue.hasValue(), GetParam().expectedHue.hasValue());
    if (hue.hasValue() && GetParam().expectedHue.hasValue())
        EXPECT_FLOAT_EQ(hue.value(), GetParam().expectedHue.value());
}

INSTANTIATE_TEST_SUITE_P(UnknownIfIsEnemy, PlayerModelGlowEnemyHueTest,
    testing::Values(PlayerModelGlowEnemyHueTestParam{.isEnemy{std::nullopt}, .expectedHue{std::nullopt}}));

static_assert(model_glow_vars::EnemyHue::ValueType::kMin == 0, "Update the test below");
static_assert(model_glow_vars::AllyHue::ValueType::kMin == 0, "Update the test below");

INSTANTIATE_TEST_SUITE_P(MinConfigVars, PlayerModelGlowEnemyHueTest,
    testing::ValuesIn(std::to_array<PlayerModelGlowEnemyHueTestParam>({
        {.isEnemy{true}, .enemyHue{0}, .expectedHue{0.0f}},
        {.isEnemy{false}, .allyHue{0}, .expectedHue{0.0f}}
    }))
);

static_assert(model_glow_vars::EnemyHue::ValueType::kMax == 359, "Update the test below");
static_assert(model_glow_vars::AllyHue::ValueType::kMax == 359, "Update the test below");

INSTANTIATE_TEST_SUITE_P(MaxConfigVars, PlayerModelGlowEnemyHueTest,
    testing::ValuesIn(std::to_array<PlayerModelGlowEnemyHueTestParam>({
        {.isEnemy{true}, .enemyHue{359}, .expectedHue{0.99722222f}},
        {.isEnemy{false}, .allyHue{359}, .expectedHue{0.99722222f}}
    }))
);

static_assert(model_glow_vars::EnemyHue::kDefaultValue == color::HueInteger{0}, "Update the tests below");
static_assert(model_glow_vars::AllyHue::kDefaultValue == color::HueInteger{120}, "Update the tests below");

INSTANTIATE_TEST_SUITE_P(DefaultConfigVars, PlayerModelGlowEnemyHueTest,
    testing::ValuesIn(std::to_array<PlayerModelGlowEnemyHueTestParam>({
        {.isEnemy{true}, .enemyHue{0}, .expectedHue{0.0f}},
        {.isEnemy{false}, .allyHue{120}, .expectedHue{0.33333333f}}
    }))
);

INSTANTIATE_TEST_SUITE_P(NonDefaultConfigVars, PlayerModelGlowEnemyHueTest,
    testing::ValuesIn(std::to_array<PlayerModelGlowEnemyHueTestParam>({
        {.isEnemy{true}, .enemyHue{123}, .expectedHue{0.34166666f}},
        {.isEnemy{false}, .allyHue{234}, .expectedHue{0.65f}}
    }))
);
