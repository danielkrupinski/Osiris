#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Hud/KillfeedPreserver/KillfeedPreserverContext.h>
#include <Features/Hud/KillfeedPreserver/KillfeedPreserverState.h>
#include <Mocks/HudMocks/MockDeathNotice.h>
#include <Mocks/HudMocks/MockDeathNotices.h>
#include <Mocks/HudMocks/MockHud.h>
#include <Mocks/MockHookContext.h>

class KillfeedPreserverContextTest : public testing::Test {
protected:
    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockHud> mockHud;
    testing::StrictMock<MockDeathNotices> mockDeathNotices;

    KillfeedPreserverState state;
    KillfeedPreserverContext<MockHookContext&> killfeedPreserverContext{state, mockHookContext};
};

TEST_F(KillfeedPreserverContextTest, StateCanBeAccessed) {
    EXPECT_THAT(killfeedPreserverContext.state(), testing::Ref(state));
}

TEST_F(KillfeedPreserverContextTest, DeathNoticesCanBeAccessed) {
    EXPECT_CALL(mockHookContext, hud()).WillOnce(testing::ReturnRef(mockHud));
    EXPECT_CALL(mockHud, deathNotices()).WillOnce(testing::ReturnRef(mockDeathNotices));

    EXPECT_THAT(killfeedPreserverContext.deathNotices(), testing::Ref(mockDeathNotices));
}

struct KillfeedPreserverContextDeathNoticeTestParam {
    bool isLocalPlayerKiller{false};
    bool wasSpawnedThisRound{false};
    bool expectMarkAsJustSpawned{false};
};

class KillfeedPreserverContextDeathNoticeTest : public KillfeedPreserverContextTest, public testing::WithParamInterface<KillfeedPreserverContextDeathNoticeTestParam> {
protected:
    testing::StrictMock<MockDeathNotice> mockDeathNotice;
};

TEST_P(KillfeedPreserverContextDeathNoticeTest, CorrectlyMarksDeathNoticesAsJustSpawned) {
    EXPECT_CALL(mockDeathNotice, isLocalPlayerKiller()).Times(testing::AtMost(1)).WillOnce(testing::Return(GetParam().isLocalPlayerKiller));
    EXPECT_CALL(mockDeathNotice, wasSpawnedThisRound()).Times(testing::AtMost(1)).WillOnce(testing::Return(GetParam().wasSpawnedThisRound));
    EXPECT_CALL(mockDeathNotice, markAsJustSpawned()).Times(GetParam().expectMarkAsJustSpawned ? 1 : 0);

    killfeedPreserverContext.preserveDeathNotice()(mockDeathNotice);
}

INSTANTIATE_TEST_SUITE_P(, KillfeedPreserverContextDeathNoticeTest, testing::Values(
    KillfeedPreserverContextDeathNoticeTestParam{.isLocalPlayerKiller = true, .wasSpawnedThisRound = true, .expectMarkAsJustSpawned = true},
    KillfeedPreserverContextDeathNoticeTestParam{.isLocalPlayerKiller = true, .wasSpawnedThisRound = false, .expectMarkAsJustSpawned = false},
    KillfeedPreserverContextDeathNoticeTestParam{.isLocalPlayerKiller = false, .wasSpawnedThisRound = true, .expectMarkAsJustSpawned = false},
    KillfeedPreserverContextDeathNoticeTestParam{.isLocalPlayerKiller = false, .wasSpawnedThisRound = false, .expectMarkAsJustSpawned = false}
));
