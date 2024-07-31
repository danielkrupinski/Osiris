#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Hud/KillfeedPreserver/KillfeedPreserver.h>
#include <Features/Hud/KillfeedPreserver/KillfeedPreserverState.h>
#include <Mocks/KillfeedPreserverMocks/MockKillfeedPreserverContext.h>
#include <Mocks/HudMocks/MockDeathNotices.h>

class KillfeedPreserverTest : public testing::Test {
protected:
    KillfeedPreserverTest()
    {
        EXPECT_CALL(mockContext, state()).WillOnce(testing::ReturnRef(state));
    }

    testing::StrictMock<MockKillfeedPreserverContext> mockContext;
    testing::StrictMock<MockDeathNotices> mockDeathNotices;

    KillfeedPreserver<MockKillfeedPreserverContext&> killfeedPreserver{mockContext};
    KillfeedPreserverState state;
    std::function<void(MockDeathNotice&)> preserveDeathNoticeFunctor;
};

TEST_F(KillfeedPreserverTest, DoesNotRunWhenNotEnabled) {
    state.enabled = false;
    EXPECT_CALL(mockContext, deathNotices()).Times(0);
    killfeedPreserver.run();
}

TEST_F(KillfeedPreserverTest, PreservesDeathNoticesWhenEnabled) {
    state.enabled = true;

    EXPECT_CALL(mockContext, deathNotices()).WillOnce(testing::ReturnRef(mockDeathNotices));

    EXPECT_CALL(mockContext, preserveDeathNotice()).WillOnce(testing::ReturnRef(preserveDeathNoticeFunctor));
    EXPECT_CALL(mockDeathNotices, forEach(testing::Ref(preserveDeathNoticeFunctor)));

    killfeedPreserver.run();
}
