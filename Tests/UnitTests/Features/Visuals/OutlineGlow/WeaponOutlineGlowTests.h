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
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<WeaponOutlineGlowEnabled>())).WillOnce(testing::Return(GetParam().enabled));

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

struct WeaponOutlineGlowColorTestParam {
    EntityTypeInfo entityTypeInfo{};
    Optional<color::Hue> expectedHue{std::nullopt};
};

class WeaponOutlineGlowColorTest : public WeaponOutlineGlowTest, public testing::WithParamInterface<WeaponOutlineGlowColorTestParam> {
};

TEST_P(WeaponOutlineGlowColorTest, CorrectGlowColorIsReturned) {
    EXPECT_EQ(weaponOutlineGlow.getGlowHue(GetParam().entityTypeInfo, mockBaseEntity), GetParam().expectedHue);
}

INSTANTIATE_TEST_SUITE_P(, WeaponOutlineGlowColorTest, testing::ValuesIn(
    std::to_array<WeaponOutlineGlowColorTestParam>({
        {
            .entityTypeInfo{},
            .expectedHue{},
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_Flashbang>()},
            .expectedHue{outline_glow_params::kFlashbangHue.toHueFloat()},
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_HEGrenade>()},
            .expectedHue{outline_glow_params::kHEGrenadeHue.toHueFloat()},
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_MolotovGrenade>()},
            .expectedHue{outline_glow_params::kMolotovHue.toHueFloat()},
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_SmokeGrenade>()},
            .expectedHue{outline_glow_params::kSmokeGrenadeHue.toHueFloat()},
        }
    })
));
