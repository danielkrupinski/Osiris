#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Mocks/MockConfig.h>
#include <Mocks/MockSmokeGrenadeProjectile.h>
#include <Mocks/MockHookContext.h>

#include <Features/Visuals/OutlineGlow/GrenadeProjectileOutlineGlow/GrenadeProjectileOutlineGlow.h>
#include <Features/Visuals/OutlineGlow/OutlineGlowConfigVariables.h>

struct GrenadeProjectileOutlineGlowTestParam {
    bool enabled{};
    EntityTypeInfo entityTypeInfo{};
    bool expectSmokeGrenadeCheck{};
    Optional<bool> didSmokeEffect{};
    cs2::Color expectedColor{0, 0, 0};
    bool expectGlowApplied{};
};

class GrenadeProjectileOutlineGlowTest : public testing::TestWithParam<GrenadeProjectileOutlineGlowTestParam> {
protected:
    GrenadeProjectileOutlineGlowTest()
    {
        EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    }

    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockConfig> mockConfig;
    testing::StrictMock<MockBaseEntity> mockGrenadeProjectile;
    testing::StrictMock<MockSmokeGrenadeProjectile> mockSmokeGrenadeProjectile;

    GrenadeProjectileOutlineGlow<MockHookContext> grenadeProjectileOutlineGlow{mockHookContext};
};


TEST_P(GrenadeProjectileOutlineGlowTest, GlowIsAppliedAsExpected) {
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<GrenadeProjectileOutlineGlowEnabled>())).WillOnce(testing::Return(GetParam().enabled));

    if (GetParam().expectSmokeGrenadeCheck) {
        EXPECT_CALL(mockGrenadeProjectile, asSmokeGrenadeProjectile()).WillOnce(testing::ReturnRef(mockSmokeGrenadeProjectile));
        EXPECT_CALL(mockSmokeGrenadeProjectile, didSmokeEffect()).WillOnce(testing::Return(GetParam().didSmokeEffect));
    }

    if (GetParam().expectGlowApplied)
        EXPECT_CALL(mockGrenadeProjectile, applyGlowRecursively(GetParam().expectedColor));

    grenadeProjectileOutlineGlow.applyGlowToGrenadeProjectile(GetParam().entityTypeInfo, mockGrenadeProjectile);
}

INSTANTIATE_TEST_SUITE_P(, GrenadeProjectileOutlineGlowTest, testing::ValuesIn(
    std::to_array<GrenadeProjectileOutlineGlowTestParam>({
        {
            .enabled = false,
            .expectGlowApplied = false
        },
        {
            .enabled = true,
            .expectedColor{outline_glow_params::kDefaultWeaponColor},
            .expectGlowApplied = true
        },
        {
            .enabled = true,
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_FlashbangProjectile>()},
            .expectedColor{outline_glow_params::kFlashbangColor},
            .expectGlowApplied = true
        },
        {
            .enabled = true,
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_HEGrenadeProjectile>()},
            .expectedColor{outline_glow_params::kHEGrenadeColor},
            .expectGlowApplied = true
        },
        {
            .enabled = true,
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_MolotovProjectile>()},
            .expectedColor{outline_glow_params::kMolotovColor},
            .expectGlowApplied = true
        },
        {
            .enabled = true,
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_SmokeGrenadeProjectile>()},
            .expectSmokeGrenadeCheck = true,
            .didSmokeEffect{std::nullopt},
            .expectedColor{outline_glow_params::kSmokeGrenadeColor},
            .expectGlowApplied = true
        },
        {
            .enabled = true,
            .entityTypeInfo{EntityTypeInfo::indexOf<cs2::C_SmokeGrenadeProjectile>()},
            .expectSmokeGrenadeCheck = true,
            .didSmokeEffect{false},
            .expectedColor{outline_glow_params::kSmokeGrenadeColor},
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
