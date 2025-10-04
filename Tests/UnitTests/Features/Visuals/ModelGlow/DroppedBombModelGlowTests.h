#pragma once

#include <gtest/gtest.h>

#include <Features/Visuals/ModelGlow/DroppedBombModelGlow/DroppedBombModelGlow.h>

extern "C" std::uint64_t Weapon_sceneObjectUpdater_asm(cs2::C_CSWeaponBase* weapon, void* unknown, bool unknownBool) noexcept
{
    return {};
}

class DroppedBombModelGlowTest : public testing::Test {
protected:
    testing::StrictMock<MockHookContext> mockHookContext{};
    testing::StrictMock<MockConfig> mockConfig{};
    FeaturesStates featuresStates{};
    DroppedBombModelGlow<MockHookContext> droppedBombModelGlow{mockHookContext};
};

TEST_F(DroppedBombModelGlowTest, Disabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<model_glow_vars::GlowDroppedBomb>(false);
    EXPECT_FALSE(droppedBombModelGlow.enabled());
}

TEST_F(DroppedBombModelGlowTest, Enabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<model_glow_vars::GlowDroppedBomb>(true);
    EXPECT_TRUE(droppedBombModelGlow.enabled());
}

TEST_F(DroppedBombModelGlowTest, CorrectDisablingFlagIsReturned) {
    EXPECT_CALL(mockHookContext, featuresStates()).WillOnce(testing::ReturnRef(featuresStates));
    EXPECT_THAT(droppedBombModelGlow.disablingFlag(), testing::Ref(featuresStates.visualFeaturesStates.modelGlowState.droppedBombModelGlowDisabling));
}

TEST_F(DroppedBombModelGlowTest, CorrectOriginalSceneObjectUpdaterIsReturned) {
    EXPECT_CALL(mockHookContext, featuresStates()).WillOnce(testing::ReturnRef(featuresStates));
    EXPECT_THAT(droppedBombModelGlow.originalSceneObjectUpdater(), testing::Ref(featuresStates.visualFeaturesStates.modelGlowState.originalWeaponSceneObjectUpdater));
}

TEST_F(DroppedBombModelGlowTest, CorrectReplacementSceneObjectUpdaterIsReturned) {
    EXPECT_EQ(droppedBombModelGlow.replacementSceneObjectUpdater(), &Weapon_sceneObjectUpdater_asm);
}

class DroppedBombModelGlowHueTest
    : public DroppedBombModelGlowTest,
      public testing::WithParamInterface<color::HueInteger> {
};

TEST_P(DroppedBombModelGlowHueTest, CorrectHueIsReturned) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<model_glow_vars::DroppedBombHue>(model_glow_vars::DroppedBombHue::ValueType{GetParam()});
    EXPECT_EQ(droppedBombModelGlow.hue(), GetParam());
}

INSTANTIATE_TEST_SUITE_P(, DroppedBombModelGlowHueTest, testing::Values(
    model_glow_vars::DroppedBombHue::ValueType::kMin,
    model_glow_vars::DroppedBombHue::ValueType::kMax,
    model_glow_vars::DroppedBombHue::kDefaultValue
));

struct DroppedBombModelGlowShouldApplyTestParam {
    Optional<bool> hasOwner{};
    bool shouldApplyGlow{};
};

class DroppedBombModelGlowShouldApplyTest
    : public DroppedBombModelGlowTest,
      public testing::WithParamInterface<DroppedBombModelGlowShouldApplyTestParam> {
};

TEST_P(DroppedBombModelGlowShouldApplyTest, ShouldApplyGlowIfBombIsDropped) {
    testing::StrictMock<MockBaseEntity> mockBaseEntity;
    EXPECT_CALL(mockBaseEntity, hasOwner()).WillOnce(testing::Return(GetParam().hasOwner));
    EXPECT_EQ(droppedBombModelGlow.shouldApplyGlow(mockBaseEntity), GetParam().shouldApplyGlow);
}

INSTANTIATE_TEST_SUITE_P(, DroppedBombModelGlowShouldApplyTest, testing::ValuesIn(
    std::to_array<DroppedBombModelGlowShouldApplyTestParam>({
        {.hasOwner{std::nullopt}, .shouldApplyGlow = false},
        {.hasOwner{true}, .shouldApplyGlow = false},
        {.hasOwner{false}, .shouldApplyGlow = true}
    })
));
