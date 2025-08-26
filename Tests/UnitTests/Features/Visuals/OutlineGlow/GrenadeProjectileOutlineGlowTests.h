#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Mocks/MockConfig.h>
#include <Mocks/MockSmokeGrenadeProjectile.h>
#include <Mocks/MockHookContext.h>

#include <Features/Visuals/OutlineGlow/GrenadeProjectileOutlineGlow/GrenadeProjectileOutlineGlow.h>
#include <Features/Visuals/OutlineGlow/OutlineGlowConfigVariables.h>

class GrenadeProjectileOutlineGlowTest : public testing::Test {
protected:
    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockConfig> mockConfig;
    testing::StrictMock<MockBaseEntity> mockGrenadeProjectile;
    testing::StrictMock<MockSmokeGrenadeProjectile> mockSmokeGrenadeProjectile;

    GrenadeProjectileOutlineGlow<MockHookContext> grenadeProjectileOutlineGlow{mockHookContext};
};

struct GrenadeProjectileOutlineGlowHueTestParam {
    EntityTypeInfo entityTypeInfo{};
    std::size_t configVarIndex{};
    std::any configuredHue{};
    color::Hue::ValueType expectedHue{};
};

class GrenadeProjectileOutlineGlowHueTest
    : public GrenadeProjectileOutlineGlowTest,
      public testing::WithParamInterface<GrenadeProjectileOutlineGlowHueTestParam> {
};

TEST_P(GrenadeProjectileOutlineGlowHueTest, CorrectGlowHueIsReturned) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    EXPECT_CALL(mockConfig, getVariable(GetParam().configVarIndex))
        .WillOnce(testing::Return(GetParam().configuredHue));

    const auto hue = grenadeProjectileOutlineGlow.getGlowHue(GetParam().entityTypeInfo, mockGrenadeProjectile);
    ASSERT_TRUE(hue.hasValue());
    EXPECT_FLOAT_EQ(hue.value(), GetParam().expectedHue);
}

static_assert(outline_glow_vars::FlashbangHue::ValueType::kMin == 191, "Update the test below");
static_assert(outline_glow_vars::HEGrenadeHue::ValueType::kMin == 300, "Update the test below");
static_assert(outline_glow_vars::MolotovHue::ValueType::kMin == 20, "Update the test below");
static_assert(outline_glow_vars::SmokeGrenadeHue::ValueType::kMin == 110, "Update the test below");

INSTANTIATE_TEST_SUITE_P(MinConfigVars, GrenadeProjectileOutlineGlowHueTest, testing::ValuesIn(
    std::to_array<GrenadeProjectileOutlineGlowHueTestParam>({
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_FlashbangProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::FlashbangHue>(),
            .configuredHue{outline_glow_vars::FlashbangHue::ValueType{color::HueInteger{191}}},
            .expectedHue = 0.53055555f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_HEGrenadeProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::HEGrenadeHue>(),
            .configuredHue{outline_glow_vars::HEGrenadeHue::ValueType{color::HueInteger{300}}},
            .expectedHue = 0.83333333f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_MolotovProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::MolotovHue>(),
            .configuredHue{outline_glow_vars::MolotovHue::ValueType{color::HueInteger{20}}},
            .expectedHue = 0.05555555f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_SmokeGrenadeProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::SmokeGrenadeHue>(),
            .configuredHue{outline_glow_vars::SmokeGrenadeHue::ValueType{color::HueInteger{110}}},
            .expectedHue = 0.30555555f
        }
    })
));

static_assert(outline_glow_vars::FlashbangHue::ValueType::kMax == 250, "Update the test below");
static_assert(outline_glow_vars::HEGrenadeHue::ValueType::kMax == 359, "Update the test below");
static_assert(outline_glow_vars::MolotovHue::ValueType::kMax == 60, "Update the test below");
static_assert(outline_glow_vars::SmokeGrenadeHue::ValueType::kMax == 140, "Update the test below");

INSTANTIATE_TEST_SUITE_P(MaxConfigVars, GrenadeProjectileOutlineGlowHueTest, testing::ValuesIn(
    std::to_array<GrenadeProjectileOutlineGlowHueTestParam>({
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_FlashbangProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::FlashbangHue>(),
            .configuredHue{outline_glow_vars::FlashbangHue::ValueType{color::HueInteger{250}}},
            .expectedHue = 0.69444444f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_HEGrenadeProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::HEGrenadeHue>(),
            .configuredHue{outline_glow_vars::HEGrenadeHue::ValueType{color::HueInteger{359}}},
            .expectedHue = 0.99722222f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_MolotovProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::MolotovHue>(),
            .configuredHue{outline_glow_vars::MolotovHue::ValueType{color::HueInteger{60}}},
            .expectedHue = 0.16666666f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_SmokeGrenadeProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::SmokeGrenadeHue>(),
            .configuredHue{outline_glow_vars::SmokeGrenadeHue::ValueType{color::HueInteger{140}}},
            .expectedHue = 0.38888888f
        }
    })
));

static_assert(outline_glow_vars::FlashbangHue::kDefaultValue == color::HueInteger{219}, "Update the tests below");
static_assert(outline_glow_vars::HEGrenadeHue::kDefaultValue == color::HueInteger{359}, "Update the tests below");
static_assert(outline_glow_vars::MolotovHue::kDefaultValue == color::HueInteger{40}, "Update the tests below");
static_assert(outline_glow_vars::SmokeGrenadeHue::kDefaultValue == color::HueInteger{120}, "Update the tests below");

INSTANTIATE_TEST_SUITE_P(DefaultConfigVars, GrenadeProjectileOutlineGlowHueTest, testing::ValuesIn(
    std::to_array<GrenadeProjectileOutlineGlowHueTestParam>({
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_FlashbangProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::FlashbangHue>(),
            .configuredHue{outline_glow_vars::FlashbangHue::ValueType{color::HueInteger{219}}},
            .expectedHue = 0.60833333f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_HEGrenadeProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::HEGrenadeHue>(),
            .configuredHue{outline_glow_vars::HEGrenadeHue::ValueType{color::HueInteger{359}}},
            .expectedHue = 0.99722222f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_MolotovProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::MolotovHue>(),
            .configuredHue{outline_glow_vars::MolotovHue::ValueType{color::HueInteger{40}}},
            .expectedHue = 0.11111111f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_SmokeGrenadeProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::SmokeGrenadeHue>(),
            .configuredHue{outline_glow_vars::SmokeGrenadeHue::ValueType{color::HueInteger{120}}},
            .expectedHue = 0.33333333f
        }
    })
));

INSTANTIATE_TEST_SUITE_P(NonDefaultConfigVars, GrenadeProjectileOutlineGlowHueTest, testing::ValuesIn(
    std::to_array<GrenadeProjectileOutlineGlowHueTestParam>({
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_FlashbangProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::FlashbangHue>(),
            .configuredHue{outline_glow_vars::FlashbangHue::ValueType{color::HueInteger{222}}},
            .expectedHue = 0.61666666f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_HEGrenadeProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::HEGrenadeHue>(),
            .configuredHue{outline_glow_vars::HEGrenadeHue::ValueType{color::HueInteger{333}}},
            .expectedHue = 0.925f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_MolotovProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::MolotovHue>(),
            .configuredHue{outline_glow_vars::MolotovHue::ValueType{color::HueInteger{55}}},
            .expectedHue = 0.15277777f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_SmokeGrenadeProjectile>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::SmokeGrenadeHue>(),
            .configuredHue{outline_glow_vars::SmokeGrenadeHue::ValueType{color::HueInteger{111}}},
            .expectedHue = 0.30833333f
        }
    })
));

struct GrenadeProjectileOutlineGlowConditionTestParam {
    bool enabled{};
    EntityTypeInfo entityTypeInfo{};
    bool expectSmokeGrenadeCheck{};
    Optional<bool> didSmokeEffect{};
    bool expectGlowApplied{};
};

class GrenadeProjectileOutlineGlowConditionTest : public GrenadeProjectileOutlineGlowTest, public testing::WithParamInterface<GrenadeProjectileOutlineGlowConditionTestParam> {
};

TEST_P(GrenadeProjectileOutlineGlowConditionTest, GlowIsAppliedAsExpected) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<outline_glow_vars::GlowGrenadeProjectiles>())).WillOnce(testing::Return(GetParam().enabled));

    if (GetParam().expectSmokeGrenadeCheck) {
        EXPECT_CALL(mockGrenadeProjectile, asSmokeGrenadeProjectile()).WillOnce(testing::ReturnRef(mockSmokeGrenadeProjectile));
        EXPECT_CALL(mockSmokeGrenadeProjectile, didSmokeEffect()).WillOnce(testing::Return(GetParam().didSmokeEffect));
    }

    EXPECT_EQ(grenadeProjectileOutlineGlow.shouldApplyGlow(GetParam().entityTypeInfo, mockGrenadeProjectile), GetParam().expectGlowApplied);
}

INSTANTIATE_TEST_SUITE_P(, GrenadeProjectileOutlineGlowConditionTest, testing::ValuesIn(
    std::to_array<GrenadeProjectileOutlineGlowConditionTestParam>({
        {
            .enabled = false,
            .expectGlowApplied = false
        },
        {
            .enabled = true,
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_FlashbangProjectile>()},
            .expectGlowApplied = true
        },
        {
            .enabled = true,
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_HEGrenadeProjectile>()},
            .expectGlowApplied = true
        },
        {
            .enabled = true,
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_MolotovProjectile>()},
            .expectGlowApplied = true
        },
        {
            .enabled = true,
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_SmokeGrenadeProjectile>()},
            .expectSmokeGrenadeCheck = true,
            .didSmokeEffect{std::nullopt},
            .expectGlowApplied = true
        },
        {
            .enabled = true,
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_SmokeGrenadeProjectile>()},
            .expectSmokeGrenadeCheck = true,
            .didSmokeEffect{false},
            .expectGlowApplied = true
        },
        {
            .enabled = true,
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_SmokeGrenadeProjectile>()},
            .expectSmokeGrenadeCheck = true,
            .didSmokeEffect{true},
            .expectGlowApplied = false
        }
    })
));
