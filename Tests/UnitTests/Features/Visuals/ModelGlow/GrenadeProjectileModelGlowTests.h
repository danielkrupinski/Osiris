#pragma once

#include <Features/Visuals/ModelGlow/GrenadeProjectileModelGlow/GrenadeProjectileModelGlow.h>

class GrenadeProjectileModelGlowTest : public testing::Test {
protected:
    testing::StrictMock<MockHookContext> mockHookContext{};
    testing::StrictMock<MockConfig> mockConfig{};
    FeaturesStates featuresStates{};
    GrenadeProjectileModelGlow<MockHookContext> grenadeProjectileModelGlow{mockHookContext};
};

TEST_F(GrenadeProjectileModelGlowTest, Disabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<model_glow_vars::GlowGrenadeProjectiles>(false);
    EXPECT_FALSE(grenadeProjectileModelGlow.enabled());
}

TEST_F(GrenadeProjectileModelGlowTest, Enabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<model_glow_vars::GlowGrenadeProjectiles>(true);
    EXPECT_TRUE(grenadeProjectileModelGlow.enabled());
}

TEST_F(GrenadeProjectileModelGlowTest, CorrectDisablingFlagIsReturned) {
    EXPECT_CALL(mockHookContext, featuresStates()).WillOnce(testing::ReturnRef(featuresStates));
    EXPECT_THAT(grenadeProjectileModelGlow.disablingFlag(), testing::Ref(featuresStates.visualFeaturesStates.modelGlowState.grenadeProjectileModelGlowDisabling));
}

struct GrenadeProjectileModelGlowHueTestParam {
    EntityTypeInfo entityTypeInfo{};
    std::size_t configVarIndex{};
    std::any configuredHue{};
    color::HueInteger expectedHue;
};

class GrenadeProjectileModelGlowHueTest
    : public GrenadeProjectileModelGlowTest,
      public testing::WithParamInterface<GrenadeProjectileModelGlowHueTestParam> {
};

TEST_P(GrenadeProjectileModelGlowHueTest, CorrectHueIsReturned) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    EXPECT_CALL(mockConfig, getVariable(GetParam().configVarIndex))
        .WillOnce(testing::Return(GetParam().configuredHue));
    
    const auto hue = grenadeProjectileModelGlow.hue(GetParam().entityTypeInfo);
    ASSERT_TRUE(hue.hasValue());
    EXPECT_EQ(hue.value(), GetParam().expectedHue);
}

static_assert(model_glow_vars::FlashbangHue::ValueType::kMin == 191, "Update the test below");
static_assert(model_glow_vars::HEGrenadeHue::ValueType::kMin == 300, "Update the test below");
static_assert(model_glow_vars::MolotovHue::ValueType::kMin == 20, "Update the test below");
static_assert(model_glow_vars::SmokeGrenadeHue::ValueType::kMin == 110, "Update the test below");

INSTANTIATE_TEST_SUITE_P(MinConfigVars, GrenadeProjectileModelGlowHueTest, testing::ValuesIn(
    std::to_array<GrenadeProjectileModelGlowHueTestParam>({
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_FlashbangProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::FlashbangHue>(),
            .configuredHue{model_glow_vars::FlashbangHue::ValueType{color::HueInteger{191}}},
            .expectedHue = color::HueInteger{191}
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_HEGrenadeProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::HEGrenadeHue>(),
            .configuredHue{model_glow_vars::HEGrenadeHue::ValueType{color::HueInteger{300}}},
            .expectedHue = color::HueInteger{300}
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_MolotovProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::MolotovHue>(),
            .configuredHue{model_glow_vars::MolotovHue::ValueType{color::HueInteger{20}}},
            .expectedHue = color::HueInteger{20}
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_SmokeGrenadeProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::SmokeGrenadeHue>(),
            .configuredHue{model_glow_vars::SmokeGrenadeHue::ValueType{color::HueInteger{110}}},
            .expectedHue = color::HueInteger{110}
        }
    })
));

static_assert(model_glow_vars::FlashbangHue::ValueType::kMax == 250, "Update the test below");
static_assert(model_glow_vars::HEGrenadeHue::ValueType::kMax == 359, "Update the test below");
static_assert(model_glow_vars::MolotovHue::ValueType::kMax == 60, "Update the test below");
static_assert(model_glow_vars::SmokeGrenadeHue::ValueType::kMax == 140, "Update the test below");

INSTANTIATE_TEST_SUITE_P(MaxConfigVars, GrenadeProjectileModelGlowHueTest, testing::ValuesIn(
    std::to_array<GrenadeProjectileModelGlowHueTestParam>({
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_FlashbangProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::FlashbangHue>(),
            .configuredHue{model_glow_vars::FlashbangHue::ValueType{color::HueInteger{250}}},
            .expectedHue = color::HueInteger{250}
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_HEGrenadeProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::HEGrenadeHue>(),
            .configuredHue{model_glow_vars::HEGrenadeHue::ValueType{color::HueInteger{359}}},
            .expectedHue = color::HueInteger{359}
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_MolotovProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::MolotovHue>(),
            .configuredHue{model_glow_vars::MolotovHue::ValueType{color::HueInteger{60}}},
            .expectedHue = color::HueInteger{60}
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_SmokeGrenadeProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::SmokeGrenadeHue>(),
            .configuredHue{model_glow_vars::SmokeGrenadeHue::ValueType{color::HueInteger{140}}},
            .expectedHue = color::HueInteger{140}
        }
    })
));

static_assert(model_glow_vars::FlashbangHue::kDefaultValue == color::HueInteger{219}, "Update the tests below");
static_assert(model_glow_vars::HEGrenadeHue::kDefaultValue == color::HueInteger{359}, "Update the tests below");
static_assert(model_glow_vars::MolotovHue::kDefaultValue == color::HueInteger{40}, "Update the tests below");
static_assert(model_glow_vars::SmokeGrenadeHue::kDefaultValue == color::HueInteger{120}, "Update the tests below");

INSTANTIATE_TEST_SUITE_P(DefaultConfigVars, GrenadeProjectileModelGlowHueTest, testing::ValuesIn(
    std::to_array<GrenadeProjectileModelGlowHueTestParam>({
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_FlashbangProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::FlashbangHue>(),
            .configuredHue{model_glow_vars::FlashbangHue::ValueType{color::HueInteger{219}}},
            .expectedHue = color::HueInteger{219}
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_HEGrenadeProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::HEGrenadeHue>(),
            .configuredHue{model_glow_vars::HEGrenadeHue::ValueType{color::HueInteger{359}}},
            .expectedHue = color::HueInteger{359}
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_MolotovProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::MolotovHue>(),
            .configuredHue{model_glow_vars::MolotovHue::ValueType{color::HueInteger{40}}},
            .expectedHue = color::HueInteger{40}
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_SmokeGrenadeProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::SmokeGrenadeHue>(),
            .configuredHue{model_glow_vars::SmokeGrenadeHue::ValueType{color::HueInteger{120}}},
            .expectedHue = color::HueInteger{120}
        }
    })
));

INSTANTIATE_TEST_SUITE_P(NonDefaultConfigVars, GrenadeProjectileModelGlowHueTest, testing::ValuesIn(
    std::to_array<GrenadeProjectileModelGlowHueTestParam>({
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_FlashbangProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::FlashbangHue>(),
            .configuredHue{model_glow_vars::FlashbangHue::ValueType{color::HueInteger{222}}},
            .expectedHue = color::HueInteger{222}
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_HEGrenadeProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::HEGrenadeHue>(),
            .configuredHue{model_glow_vars::HEGrenadeHue::ValueType{color::HueInteger{333}}},
            .expectedHue = color::HueInteger{333}
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_MolotovProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::MolotovHue>(),
            .configuredHue{model_glow_vars::MolotovHue::ValueType{color::HueInteger{55}}},
            .expectedHue = color::HueInteger{55}
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_SmokeGrenadeProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<model_glow_vars::SmokeGrenadeHue>(),
            .configuredHue{model_glow_vars::SmokeGrenadeHue::ValueType{color::HueInteger{111}}},
            .expectedHue = color::HueInteger{111}
        }
    })
));
