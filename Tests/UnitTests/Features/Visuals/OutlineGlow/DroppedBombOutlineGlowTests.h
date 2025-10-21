#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Mocks/MockConfig.h>
#include <Mocks/MockHookContext.h>

#include <Features/Visuals/OutlineGlow/DroppedBombOutlineGlow/DroppedBombOutlineGlow.h>
#include <Features/Visuals/OutlineGlow/OutlineGlowConfigVariables.h>
#include <Utils/Optional.h>

class DroppedBombOutlineGlowTest : public testing::Test {
protected:
    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockConfig> mockConfig;
    testing::StrictMock<MockBaseEntity> mockBaseEntity;

    DroppedBombOutlineGlow<MockHookContext> droppedBombOutlineGlow{mockHookContext};
};

TEST_F(DroppedBombOutlineGlowTest, Disabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<outline_glow_vars::GlowDroppedBomb>(false);
    EXPECT_FALSE(droppedBombOutlineGlow.enabled());
}

TEST_F(DroppedBombOutlineGlowTest, Enabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<outline_glow_vars::GlowDroppedBomb>(true);
    EXPECT_TRUE(droppedBombOutlineGlow.enabled());
}

struct DroppedBombOutlineGlowConditionTestParam {
    Optional<bool> bombHasOwner{};
    bool expectGlowApplied{};
};

class DroppedBombOutlineGlowConditionTest
    : public DroppedBombOutlineGlowTest,
      public testing::WithParamInterface<DroppedBombOutlineGlowConditionTestParam> {
};

TEST_P(DroppedBombOutlineGlowConditionTest, ShouldApplyGlowWhenExpected) {
    EXPECT_CALL(mockBaseEntity, hasOwner()).WillOnce(testing::Return(GetParam().bombHasOwner));
    EXPECT_EQ(droppedBombOutlineGlow.shouldApplyGlow(EntityTypeInfo{}, mockBaseEntity), GetParam().expectGlowApplied);
}

INSTANTIATE_TEST_SUITE_P(, DroppedBombOutlineGlowConditionTest, testing::ValuesIn(
    std::to_array<DroppedBombOutlineGlowConditionTestParam>({
        {.bombHasOwner{std::nullopt}, .expectGlowApplied = false},
        {.bombHasOwner{true}, .expectGlowApplied = false},
        {.bombHasOwner{false}, .expectGlowApplied = true}
    })
));

class DroppedBombOutlineGlowHueTest
    : public DroppedBombOutlineGlowTest,
      public testing::WithParamInterface<color::HueInteger> {
};

TEST_P(DroppedBombOutlineGlowHueTest, CorrectGlowHueIsReturned) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<outline_glow_vars::DroppedBombHue>(outline_glow_vars::DroppedBombHue::ValueType{GetParam()});
    EXPECT_EQ(droppedBombOutlineGlow.hue(), GetParam());
}

static_assert(outline_glow_vars::DroppedBombHue::ValueType::kMin == 0, "Update the test below");
INSTANTIATE_TEST_SUITE_P(MinConfigVar, DroppedBombOutlineGlowHueTest, testing::Values(color::HueInteger{0}));

static_assert(outline_glow_vars::DroppedBombHue::ValueType::kMax == 359, "Update the test below");
INSTANTIATE_TEST_SUITE_P(MaxConfigVar, DroppedBombOutlineGlowHueTest, testing::Values(color::HueInteger{359}));

static_assert(outline_glow_vars::DroppedBombHue::kDefaultValue == color::HueInteger{60}, "Update the tests below");

INSTANTIATE_TEST_SUITE_P(DefaultConfigVar, DroppedBombOutlineGlowHueTest, testing::Values(color::HueInteger{60}));
INSTANTIATE_TEST_SUITE_P(NonDefaultConfigVar, DroppedBombOutlineGlowHueTest, testing::Values(color::HueInteger{140}));
