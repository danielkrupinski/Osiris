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

struct GrenadeProjectileOutlineGlowColorTestParam {
    EntityTypeInfo entityTypeInfo{};
    Optional<color::Hue> expectedHue{};
};

class GrenadeProjectileOutlineGlowColorTest : public GrenadeProjectileOutlineGlowTest, public testing::WithParamInterface<GrenadeProjectileOutlineGlowColorTestParam> {
};

TEST_P(GrenadeProjectileOutlineGlowColorTest, CorrectGlowColorIsReturned) {
    EXPECT_EQ(grenadeProjectileOutlineGlow.getGlowHue(GetParam().entityTypeInfo, mockGrenadeProjectile), GetParam().expectedHue);
}

INSTANTIATE_TEST_SUITE_P(, GrenadeProjectileOutlineGlowColorTest, testing::ValuesIn(
    std::to_array<GrenadeProjectileOutlineGlowColorTestParam>({
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_FlashbangProjectile>()},
            .expectedHue{outline_glow_params::kFlashbangHue.toHueFloat()},
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_HEGrenadeProjectile>()},
            .expectedHue{outline_glow_params::kHEGrenadeHue.toHueFloat()}
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_MolotovProjectile>()},
            .expectedHue{outline_glow_params::kMolotovHue.toHueFloat()}
        },
        {
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_SmokeGrenadeProjectile>()},
            .expectedHue{outline_glow_params::kSmokeGrenadeHue.toHueFloat()}
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
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<GrenadeProjectileOutlineGlowEnabled>())).WillOnce(testing::Return(GetParam().enabled));

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
