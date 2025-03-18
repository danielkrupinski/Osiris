#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Hud/KillfeedPreserver/KillfeedPreserver.h>
#include <Mocks/KillfeedPreserverMocks/MockKillfeedPreserverContext.h>
#include <Mocks/HudMocks/MockDeathNotices.h>

class KillfeedPreserverTest : public testing::Test {
protected:
    testing::StrictMock<MockKillfeedPreserverContext> mockContext;
    testing::StrictMock<MockDeathNotices> mockDeathNotices;
    testing::StrictMock<MockConfig> mockConfig;

    KillfeedPreserver<MockHookContext, MockKillfeedPreserverContext&> killfeedPreserver{mockContext};
    std::function<void(MockDeathNotice&)> preserveDeathNoticeFunctor;
};

TEST_F(KillfeedPreserverTest, DoesNotRunWhenNotEnabled) {
    EXPECT_CALL(mockContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    EXPECT_CALL(mockConfig, getVariableBool(ConfigVariableTypes::indexOf<KillfeedPreserverEnabled>())).WillOnce(testing::Return(false));
    EXPECT_CALL(mockContext, deathNotices()).Times(0);
    killfeedPreserver.run();
}

TEST_F(KillfeedPreserverTest, PreservesDeathNoticesWhenEnabled) {
    EXPECT_CALL(mockContext, config()).WillOnce(testing::ReturnRef(mockConfig));
    EXPECT_CALL(mockConfig, getVariableBool(ConfigVariableTypes::indexOf<KillfeedPreserverEnabled>())).WillOnce(testing::Return(true));
    EXPECT_CALL(mockContext, deathNotices()).WillOnce(testing::ReturnRef(mockDeathNotices));

    EXPECT_CALL(mockContext, preserveDeathNotice()).WillOnce(testing::ReturnRef(preserveDeathNoticeFunctor));
    EXPECT_CALL(mockDeathNotices, forEach(testing::Ref(preserveDeathNoticeFunctor)));

    killfeedPreserver.run();
}
