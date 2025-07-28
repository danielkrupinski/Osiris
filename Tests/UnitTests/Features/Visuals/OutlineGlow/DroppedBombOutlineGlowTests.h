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
    DroppedBombOutlineGlowTest()
    {
        EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    }

    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockConfig> mockConfig;
    testing::StrictMock<MockBaseEntity> mockBaseEntity;

    DroppedBombOutlineGlow<MockHookContext> droppedBombOutlineGlow{mockHookContext};
};

struct DroppedBombOutlineGlowConditionTestParam {
    bool enabled{};
    bool expectBombHasOwnerCalled{};
    Optional<bool> bombHasOwner{};
    bool expectGlowApplied{};
};

class DroppedBombOutlineGlowConditionTest
    : public DroppedBombOutlineGlowTest,
      public testing::WithParamInterface<DroppedBombOutlineGlowConditionTestParam> {
};

TEST_P(DroppedBombOutlineGlowConditionTest, ShouldApplyGlowWhenExpected) {
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<DroppedBombOutlineGlowEnabled>()))
        .WillOnce(testing::Return(GetParam().enabled));
    if (GetParam().expectBombHasOwnerCalled)
        EXPECT_CALL(mockBaseEntity, hasOwner()).WillOnce(testing::Return(GetParam().bombHasOwner));
    EXPECT_EQ(droppedBombOutlineGlow.shouldApplyGlow(EntityTypeInfo{}, mockBaseEntity), GetParam().expectGlowApplied);
}

INSTANTIATE_TEST_SUITE_P(, DroppedBombOutlineGlowConditionTest, testing::ValuesIn(
    std::to_array<DroppedBombOutlineGlowConditionTestParam>({
        {.enabled = false, .expectBombHasOwnerCalled = false, .expectGlowApplied = false},
        {.enabled = true, .expectBombHasOwnerCalled = true, .bombHasOwner{std::nullopt}, .expectGlowApplied = false},
        {.enabled = true, .expectBombHasOwnerCalled = true, .bombHasOwner{true}, .expectGlowApplied = false},
        {.enabled = true, .expectBombHasOwnerCalled = true, .bombHasOwner{false}, .expectGlowApplied = true}
    })
));

struct DroppedBombOutlineGlowHueTestParam {
    color::HueInteger::UnderlyingType configuredHue{};
    color::Hue::ValueType expectedHue{};
};

class DroppedBombOutlineGlowHueTest
    : public DroppedBombOutlineGlowTest,
      public testing::WithParamInterface<DroppedBombOutlineGlowHueTestParam> {
};

TEST_P(DroppedBombOutlineGlowHueTest, CorrectGlowHueIsReturned) {
    EXPECT_CALL(mockConfig, getVariable(ConfigVariableTypes::indexOf<OutlineGlowDroppedBombHue>()))
        .WillOnce(testing::Return(OutlineGlowDroppedBombHue::ValueType{color::HueInteger{GetParam().configuredHue}}));

    const auto hue = droppedBombOutlineGlow.getGlowHue(EntityTypeInfo{}, mockBaseEntity);
    ASSERT_TRUE(hue.hasValue());
    EXPECT_FLOAT_EQ(hue.value(), GetParam().expectedHue);
}

static_assert(OutlineGlowDroppedBombHue::ValueType::kMin == 0, "Update the test below");
INSTANTIATE_TEST_SUITE_P(MinConfigVar, DroppedBombOutlineGlowHueTest,
                         testing::Values(DroppedBombOutlineGlowHueTestParam{.configuredHue = 0, .expectedHue = 0.0f}));

static_assert(OutlineGlowDroppedBombHue::ValueType::kMax == 359, "Update the test below");
INSTANTIATE_TEST_SUITE_P(MaxConfigVar, DroppedBombOutlineGlowHueTest,
                         testing::Values(DroppedBombOutlineGlowHueTestParam{.configuredHue = 359, .expectedHue = 0.99722222f}));

static_assert(OutlineGlowDroppedBombHue::kDefaultValue == color::HueInteger{60}, "Update the tests below");

INSTANTIATE_TEST_SUITE_P(DefaultConfigVar, DroppedBombOutlineGlowHueTest,
                         testing::Values(DroppedBombOutlineGlowHueTestParam{.configuredHue = 60, .expectedHue = 0.16666666f}));

INSTANTIATE_TEST_SUITE_P(NonDefaultConfigVar, DroppedBombOutlineGlowHueTest,
                         testing::Values(DroppedBombOutlineGlowHueTestParam{.configuredHue = 140, .expectedHue = 0.38888888f}));
