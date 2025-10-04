#pragma once

#include <gtest/gtest.h>

#include <Features/Visuals/ModelGlow/WeaponModelGlow/WeaponModelGlow.h>

class WeaponModelGlowTest : public testing::Test {
protected:
    testing::StrictMock<MockHookContext> mockHookContext{};
    testing::StrictMock<MockConfig> mockConfig{};
    FeaturesStates featuresStates{};
    WeaponModelGlow<MockHookContext> weaponModelGlow{mockHookContext};
};

TEST_F(WeaponModelGlowTest, Disabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<model_glow_vars::GlowWeapons>(false);
    EXPECT_FALSE(weaponModelGlow.enabled());
}

TEST_F(WeaponModelGlowTest, Enabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<model_glow_vars::GlowWeapons>(true);
    EXPECT_TRUE(weaponModelGlow.enabled());
}

TEST_F(WeaponModelGlowTest, CorrectDisablingFlagIsReturned) {
    EXPECT_CALL(mockHookContext, featuresStates()).WillOnce(testing::ReturnRef(featuresStates));
    EXPECT_THAT(weaponModelGlow.disablingFlag(), testing::Ref(featuresStates.visualFeaturesStates.modelGlowState.weaponModelGlowDisabling));
}

TEST_F(WeaponModelGlowTest, CorrectOriginalSceneObjectUpdaterIsReturned) {
    EXPECT_CALL(mockHookContext, featuresStates()).WillOnce(testing::ReturnRef(featuresStates));
    EXPECT_THAT(weaponModelGlow.originalSceneObjectUpdater(), testing::Ref(featuresStates.visualFeaturesStates.modelGlowState.originalWeaponSceneObjectUpdater));
}

TEST_F(WeaponModelGlowTest, CorrectReplacementSceneObjectUpdaterIsReturned) {
    EXPECT_EQ(weaponModelGlow.replacementSceneObjectUpdater(), &Weapon_sceneObjectUpdater_asm);
}

TEST_F(WeaponModelGlowTest, NoHueIsReturnedWhenDefaultShouldBeUsed) {
    EXPECT_FALSE(weaponModelGlow.hue(EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_AK47>()}).hasValue());
    EXPECT_FALSE(weaponModelGlow.hue(EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_WeaponAWP>()}).hasValue());
    EXPECT_FALSE(weaponModelGlow.hue(EntityTypeInfo{}).hasValue());
}

struct WeaponModelGlowShouldApplyTestParam {
    Optional<bool> hasOwner{};
    bool shouldApplyGlow{};
};

class WeaponModelGlowShouldApplyTest
    : public WeaponModelGlowTest,
      public testing::WithParamInterface<WeaponModelGlowShouldApplyTestParam> {
};

TEST_P(WeaponModelGlowShouldApplyTest, ShouldApplyGlowIfWeaponIsDropped) {
    testing::StrictMock<MockBaseEntity> mockBaseEntity;
    EXPECT_CALL(mockBaseEntity, hasOwner()).WillOnce(testing::Return(GetParam().hasOwner));
    EXPECT_EQ(weaponModelGlow.shouldApplyGlow(mockBaseEntity), GetParam().shouldApplyGlow);
}

INSTANTIATE_TEST_SUITE_P(, WeaponModelGlowShouldApplyTest, testing::ValuesIn(
    std::to_array<WeaponModelGlowShouldApplyTestParam>({
        {.hasOwner{std::nullopt}, .shouldApplyGlow = false},
        {.hasOwner{true}, .shouldApplyGlow = false},
        {.hasOwner{false}, .shouldApplyGlow = true}
    })
));

struct WeaponModelGlowHueTestParam {
    EntityTypeInfo entityTypeInfo{};
    std::size_t configVarIndex{};
    std::any configuredHue{};
    color::HueInteger::UnderlyingType expectedHue{};
};

class WeaponModelGlowHueTest
    : public WeaponModelGlowTest,
      public testing::WithParamInterface<WeaponModelGlowHueTestParam> {
};

TEST_P(WeaponModelGlowHueTest, CorrectGlowHueIsReturned) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    EXPECT_CALL(mockConfig, getVariable(GetParam().configVarIndex))
        .WillRepeatedly(testing::Return(GetParam().configuredHue));

    const auto hue = weaponModelGlow.hue(GetParam().entityTypeInfo);
    ASSERT_TRUE(hue.hasValue());
    EXPECT_FLOAT_EQ(hue.value(), GetParam().expectedHue);
}

static_assert(model_glow_vars::FlashbangHue::ValueType::kMin == 191, "Update the test below");
static_assert(model_glow_vars::HEGrenadeHue::ValueType::kMin == 300, "Update the test below");
static_assert(model_glow_vars::MolotovHue::ValueType::kMin == 20, "Update the test below");
static_assert(model_glow_vars::SmokeGrenadeHue::ValueType::kMin == 110, "Update the test below");

INSTANTIATE_TEST_SUITE_P(MinConfigVars, WeaponModelGlowHueTest, testing::ValuesIn(
    std::to_array<WeaponModelGlowHueTestParam>({
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_Flashbang>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::FlashbangHue>(),
            .configuredHue{model_glow_vars::FlashbangHue::ValueType{color::HueInteger{191}}},
            .expectedHue = 191
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_HEGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::HEGrenadeHue>(),
            .configuredHue{model_glow_vars::HEGrenadeHue::ValueType{color::HueInteger{300}}},
            .expectedHue = 300
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_MolotovGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::MolotovHue>(),
            .configuredHue{model_glow_vars::MolotovHue::ValueType{color::HueInteger{20}}},
            .expectedHue = 20
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_IncendiaryGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::MolotovHue>(),
            .configuredHue{model_glow_vars::MolotovHue::ValueType{color::HueInteger{20}}},
            .expectedHue = 20
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_SmokeGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::SmokeGrenadeHue>(),
            .configuredHue{model_glow_vars::SmokeGrenadeHue::ValueType{color::HueInteger{110}}},
            .expectedHue = 110
        }
    })
));

static_assert(model_glow_vars::FlashbangHue::ValueType::kMax == 250, "Update the test below");
static_assert(model_glow_vars::HEGrenadeHue::ValueType::kMax == 359, "Update the test below");
static_assert(model_glow_vars::MolotovHue::ValueType::kMax == 60, "Update the test below");
static_assert(model_glow_vars::SmokeGrenadeHue::ValueType::kMax == 140, "Update the test below");

INSTANTIATE_TEST_SUITE_P(MaxConfigVars, WeaponModelGlowHueTest, testing::ValuesIn(
    std::to_array<WeaponModelGlowHueTestParam>({
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_Flashbang>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::FlashbangHue>(),
            .configuredHue{model_glow_vars::FlashbangHue::ValueType{color::HueInteger{250}}},
            .expectedHue = 250
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_HEGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::HEGrenadeHue>(),
            .configuredHue{model_glow_vars::HEGrenadeHue::ValueType{color::HueInteger{359}}},
            .expectedHue = 359
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_MolotovGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::MolotovHue>(),
            .configuredHue{model_glow_vars::MolotovHue::ValueType{color::HueInteger{60}}},
            .expectedHue = 60
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_IncendiaryGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::MolotovHue>(),
            .configuredHue{model_glow_vars::MolotovHue::ValueType{color::HueInteger{60}}},
            .expectedHue = 60
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_SmokeGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::SmokeGrenadeHue>(),
            .configuredHue{model_glow_vars::SmokeGrenadeHue::ValueType{color::HueInteger{140}}},
            .expectedHue = 140
        }
    })
));

static_assert(model_glow_vars::FlashbangHue::kDefaultValue == color::HueInteger{219}, "Update the tests below");
static_assert(model_glow_vars::HEGrenadeHue::kDefaultValue == color::HueInteger{359}, "Update the tests below");
static_assert(model_glow_vars::MolotovHue::kDefaultValue == color::HueInteger{40}, "Update the tests below");
static_assert(model_glow_vars::SmokeGrenadeHue::kDefaultValue == color::HueInteger{120}, "Update the tests below");

INSTANTIATE_TEST_SUITE_P(DefaultConfigVars, WeaponModelGlowHueTest, testing::ValuesIn(
    std::to_array<WeaponModelGlowHueTestParam>({
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_Flashbang>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::FlashbangHue>(),
            .configuredHue{model_glow_vars::FlashbangHue::ValueType{color::HueInteger{219}}},
            .expectedHue = 219
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_HEGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::HEGrenadeHue>(),
            .configuredHue{model_glow_vars::HEGrenadeHue::ValueType{color::HueInteger{359}}},
            .expectedHue = 359
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_MolotovGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::MolotovHue>(),
            .configuredHue{model_glow_vars::MolotovHue::ValueType{color::HueInteger{40}}},
            .expectedHue = 40
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_IncendiaryGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::MolotovHue>(),
            .configuredHue{model_glow_vars::MolotovHue::ValueType{color::HueInteger{40}}},
            .expectedHue = 40
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_SmokeGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::SmokeGrenadeHue>(),
            .configuredHue{model_glow_vars::SmokeGrenadeHue::ValueType{color::HueInteger{120}}},
            .expectedHue = 120
        }
    })
));

INSTANTIATE_TEST_SUITE_P(NonDefaultConfigVars, WeaponModelGlowHueTest, testing::ValuesIn(
    std::to_array<WeaponModelGlowHueTestParam>({
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_Flashbang>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::FlashbangHue>(),
            .configuredHue{model_glow_vars::FlashbangHue::ValueType{color::HueInteger{222}}},
            .expectedHue = 222
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_HEGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::HEGrenadeHue>(),
            .configuredHue{model_glow_vars::HEGrenadeHue::ValueType{color::HueInteger{333}}},
            .expectedHue = 333
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_MolotovGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::MolotovHue>(),
            .configuredHue{model_glow_vars::MolotovHue::ValueType{color::HueInteger{55}}},
            .expectedHue = 55
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_IncendiaryGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::MolotovHue>(),
            .configuredHue{model_glow_vars::MolotovHue::ValueType{color::HueInteger{55}}},
            .expectedHue = 55
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_SmokeGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::SmokeGrenadeHue>(),
            .configuredHue{model_glow_vars::SmokeGrenadeHue::ValueType{color::HueInteger{111}}},
            .expectedHue = 111
        }
    })
));
