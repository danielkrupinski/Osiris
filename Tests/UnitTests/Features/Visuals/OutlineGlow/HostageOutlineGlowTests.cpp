#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Mocks/MockBaseEntity.h>
#include <Mocks/MockConfig.h>
#include <Mocks/MockHookContext.h>

#include <Features/Visuals/OutlineGlow/HostageOutlineGlow/HostageOutlineGlow.h>

class HostageOutlineGlowTest : public testing::Test {
protected:
    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockConfig> mockConfig;
    testing::StrictMock<MockBaseEntity> mockBaseEntity;

    HostageOutlineGlow<MockHookContext> hostageOutlineGlow{mockHookContext};
};

TEST_F(HostageOutlineGlowTest, Disabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<outline_glow_vars::GlowHostages>(false);
    EXPECT_FALSE(hostageOutlineGlow.enabled());
}

TEST_F(HostageOutlineGlowTest, Enabled) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<outline_glow_vars::GlowHostages>(true);
    EXPECT_TRUE(hostageOutlineGlow.enabled());
}

class HostageOutlineGlowHueTest
    : public HostageOutlineGlowTest,
      public testing::WithParamInterface<color::HueInteger> {
};

TEST_P(HostageOutlineGlowHueTest, CorrectGlowHueIsReturned) {
    EXPECT_CALL(mockHookContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    mockConfig.expectGetVariable<outline_glow_vars::HostageHue>(outline_glow_vars::HostageHue::ValueType{GetParam()});
    EXPECT_EQ(hostageOutlineGlow.hue(), GetParam());
}

static_assert(outline_glow_vars::HostageHue::kDefaultValue == color::HueInteger{50}, "Update the test below");
static_assert(outline_glow_vars::HostageHue::ValueType::kMin == 0, "Update the test below");
static_assert(outline_glow_vars::HostageHue::ValueType::kMax == 359, "Update the test below");

INSTANTIATE_TEST_SUITE_P(, HostageOutlineGlowHueTest, testing::Values(
    color::HueInteger{0},
    color::HueInteger{50},
    color::HueInteger{70},
    color::HueInteger{359}
));
