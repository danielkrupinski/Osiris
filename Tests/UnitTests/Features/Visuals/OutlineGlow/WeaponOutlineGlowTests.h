#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Mocks/MockBaseEntity.h>
#include <Mocks/MockConfig.h>
#include <Mocks/MockHookContext.h>

#include <Features/Visuals/OutlineGlow/WeaponOutlineGlow/WeaponOutlineGlow.h>
#include <Features/Visuals/OutlineGlow/OutlineGlowConfigVariables.h>

class WeaponOutlineGlowTest : public testing::Test {
protected:
    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockConfig> mockConfig;
    testing::StrictMock<MockBaseEntity> mockBaseEntity;

    WeaponOutlineGlow<MockHookContext> weaponOutlineGlow{mockHookContext};
};

TEST_F(WeaponOutlineGlowTest, CorrectGlowRangeIsReturned) {
    EXPECT_EQ(weaponOutlineGlow.getGlowRange(), outline_glow_params::kWeaponGlowRange);
}

TEST_F(WeaponOutlineGlowTest, NoHueIsReturnedWhenDefaultShouldBeUsed) {
    EXPECT_FALSE(weaponOutlineGlow.getGlowHue(EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_AK47>()}, mockBaseEntity).hasValue());
    EXPECT_FALSE(weaponOutlineGlow.getGlowHue(EntityTypeInfo{EntityTypeInfo::indexOf<cs2::C_WeaponAWP>()}, mockBaseEntity).hasValue());
    EXPECT_FALSE(weaponOutlineGlow.getGlowHue(EntityTypeInfo{}, mockBaseEntity).hasValue());
}

struct WeaponOutlineGlowConditionTestParam {
    bool enabled{};
    Optional<bool> hasOwner{};
    bool expectWeaponAccess{};
    bool expectGlowApplied{};
};

class WeaponOutlineGlowConditionTest : public WeaponOutlineGlowTest, public testing::WithParamInterface<WeaponOutlineGlowConditionTestParam> {
};

TEST_P(WeaponOutlineGlowConditionTest, GlowShouldBeAppliedWhenExpected) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<outline_glow_vars::GlowWeapons>(GetParam().enabled);

    if (GetParam().expectWeaponAccess)
        EXPECT_CALL(mockBaseEntity, hasOwner()).WillOnce(testing::Return(GetParam().hasOwner));

    EXPECT_EQ(weaponOutlineGlow.shouldApplyGlow(EntityTypeInfo{}, mockBaseEntity), GetParam().expectGlowApplied);
}

INSTANTIATE_TEST_SUITE_P(, WeaponOutlineGlowConditionTest, testing::ValuesIn(
    std::to_array<WeaponOutlineGlowConditionTestParam>({
        {
            .enabled = false,
            .expectWeaponAccess = false,
            .expectGlowApplied = false
        },
        {
            .enabled = true,
            .hasOwner{true},
            .expectWeaponAccess = true,
            .expectGlowApplied = false
        },
        {
            .enabled = true,
            .hasOwner{false},
            .expectWeaponAccess = true,
            .expectGlowApplied = true
        },
        {
            .enabled = true,
            .hasOwner{std::nullopt},
            .expectWeaponAccess = true,
            .expectGlowApplied = false
        }
    })
));

struct WeaponOutlineGlowHueTestParam {
    EntityTypeInfo entityTypeInfo{};
    std::size_t configVarIndex{};
    std::any configuredHue{};
    color::Hue::ValueType expectedHue{};
};

class WeaponOutlineGlowHueTest
    : public WeaponOutlineGlowTest,
      public testing::WithParamInterface<WeaponOutlineGlowHueTestParam> {
};

TEST_P(WeaponOutlineGlowHueTest, CorrectGlowHueIsReturned) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    EXPECT_CALL(mockConfig, getVariable(GetParam().configVarIndex))
        .WillRepeatedly(testing::Return(GetParam().configuredHue));

    const auto hue = weaponOutlineGlow.getGlowHue(GetParam().entityTypeInfo, mockBaseEntity);
    ASSERT_TRUE(hue.hasValue());
    EXPECT_FLOAT_EQ(hue.value(), GetParam().expectedHue);
}

static_assert(outline_glow_vars::FlashbangHue::ValueType::kMin == 191, "Update the test below");
static_assert(outline_glow_vars::HEGrenadeHue::ValueType::kMin == 300, "Update the test below");
static_assert(outline_glow_vars::MolotovHue::ValueType::kMin == 20, "Update the test below");
static_assert(outline_glow_vars::SmokeGrenadeHue::ValueType::kMin == 110, "Update the test below");

INSTANTIATE_TEST_SUITE_P(MinConfigVars, WeaponOutlineGlowHueTest, testing::ValuesIn(
    std::to_array<WeaponOutlineGlowHueTestParam>({
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_Flashbang>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::FlashbangHue>(),
            .configuredHue{outline_glow_vars::FlashbangHue::ValueType{color::HueInteger{191}}},
            .expectedHue = 0.53055555f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_HEGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::HEGrenadeHue>(),
            .configuredHue{outline_glow_vars::HEGrenadeHue::ValueType{color::HueInteger{300}}},
            .expectedHue = 0.83333333f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_MolotovGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::MolotovHue>(),
            .configuredHue{outline_glow_vars::MolotovHue::ValueType{color::HueInteger{20}}},
            .expectedHue = 0.05555555f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_IncendiaryGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::MolotovHue>(),
            .configuredHue{outline_glow_vars::MolotovHue::ValueType{color::HueInteger{20}}},
            .expectedHue = 0.05555555f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_SmokeGrenade>()},
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

INSTANTIATE_TEST_SUITE_P(MaxConfigVars, WeaponOutlineGlowHueTest, testing::ValuesIn(
    std::to_array<WeaponOutlineGlowHueTestParam>({
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_Flashbang>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::FlashbangHue>(),
            .configuredHue{outline_glow_vars::FlashbangHue::ValueType{color::HueInteger{250}}},
            .expectedHue = 0.69444444f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_HEGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::HEGrenadeHue>(),
            .configuredHue{outline_glow_vars::HEGrenadeHue::ValueType{color::HueInteger{359}}},
            .expectedHue = 0.99722222f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_MolotovGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::MolotovHue>(),
            .configuredHue{outline_glow_vars::MolotovHue::ValueType{color::HueInteger{60}}},
            .expectedHue = 0.16666666f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_IncendiaryGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::MolotovHue>(),
            .configuredHue{outline_glow_vars::MolotovHue::ValueType{color::HueInteger{60}}},
            .expectedHue = 0.16666666f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_SmokeGrenade>()},
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

INSTANTIATE_TEST_SUITE_P(DefaultConfigVars, WeaponOutlineGlowHueTest, testing::ValuesIn(
    std::to_array<WeaponOutlineGlowHueTestParam>({
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_Flashbang>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::FlashbangHue>(),
            .configuredHue{outline_glow_vars::FlashbangHue::ValueType{color::HueInteger{219}}},
            .expectedHue = 0.60833333f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_HEGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::HEGrenadeHue>(),
            .configuredHue{outline_glow_vars::HEGrenadeHue::ValueType{color::HueInteger{359}}},
            .expectedHue = 0.99722222f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_MolotovGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::MolotovHue>(),
            .configuredHue{outline_glow_vars::MolotovHue::ValueType{color::HueInteger{40}}},
            .expectedHue = 0.11111111f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_IncendiaryGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::MolotovHue>(),
            .configuredHue{outline_glow_vars::MolotovHue::ValueType{color::HueInteger{40}}},
            .expectedHue = 0.11111111f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_SmokeGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::SmokeGrenadeHue>(),
            .configuredHue{outline_glow_vars::SmokeGrenadeHue::ValueType{color::HueInteger{120}}},
            .expectedHue = 0.33333333f
        }
    })
));

INSTANTIATE_TEST_SUITE_P(NonDefaultConfigVars, WeaponOutlineGlowHueTest, testing::ValuesIn(
    std::to_array<WeaponOutlineGlowHueTestParam>({
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_Flashbang>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::FlashbangHue>(),
            .configuredHue{outline_glow_vars::FlashbangHue::ValueType{color::HueInteger{222}}},
            .expectedHue = 0.61666666f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_HEGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::HEGrenadeHue>(),
            .configuredHue{outline_glow_vars::HEGrenadeHue::ValueType{color::HueInteger{333}}},
            .expectedHue = 0.925f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_MolotovGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::MolotovHue>(),
            .configuredHue{outline_glow_vars::MolotovHue::ValueType{color::HueInteger{55}}},
            .expectedHue = 0.15277777f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_IncendiaryGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::MolotovHue>(),
            .configuredHue{outline_glow_vars::MolotovHue::ValueType{color::HueInteger{55}}},
            .expectedHue = 0.15277777f
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_SmokeGrenade>()},
            .configVarIndex = ConfigVariableTypes::indexOf<outline_glow_vars::SmokeGrenadeHue>(),
            .configuredHue{outline_glow_vars::SmokeGrenadeHue::ValueType{color::HueInteger{111}}},
            .expectedHue = 0.30833333f
        }
    })
));
