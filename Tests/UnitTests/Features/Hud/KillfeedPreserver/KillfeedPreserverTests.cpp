#include <array>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Hud/KillfeedPreserver/KillfeedPreserver.h>
#include <Mocks/HudMocks/MockDeathNotices.h>
#include <Mocks/HudMocks/MockDeathNotice.h>
#include <Mocks/HudMocks/MockHud.h>
#include <Mocks/MockConfig.h>
#include <Mocks/MockHookContext.h>

class KillfeedPreserverTest : public testing::Test {
protected:
    KillfeedPreserverTest()
    {
        EXPECT_CALL(mockHookContext, config()).WillRepeatedly(testing::ReturnRef(mockConfig));
    }

    testing::StrictMock<MockHookContext> mockHookContext;
    testing::StrictMock<MockConfig> mockConfig;

    KillfeedPreserver<MockHookContext> killfeedPreserver{mockHookContext};
};

TEST_F(KillfeedPreserverTest, DoesNotRunWhenNotEnabled) {
    mockConfig.expectGetVariable<KillfeedPreserverEnabled>(false);
    killfeedPreserver.run();
}

struct KillfeedPreserverEnabledTestParam {
    bool isLocalPlayerKiller{false};
    bool wasSpawnedThisRound{false};
    bool expectMarkAsJustSpawned{false};
};

class KillfeedPreserverEnabledTest : public KillfeedPreserverTest, public testing::WithParamInterface<KillfeedPreserverEnabledTestParam> {
protected:
    testing::StrictMock<MockHud> mockHud;
    testing::StrictMock<MockDeathNotices> mockDeathNotices;
    testing::StrictMock<MockDeathNotice> mockDeathNotice;
};

TEST_P(KillfeedPreserverEnabledTest, PreservesDeathNoticesWhenEnabled) {
    mockConfig.expectGetVariable<KillfeedPreserverEnabled>(true);

    EXPECT_CALL(mockHookContext, hud()).WillOnce(testing::ReturnRef(mockHud));
    EXPECT_CALL(mockHud, deathNotices()).WillOnce(testing::ReturnRef(mockDeathNotices));

    EXPECT_CALL(mockDeathNotices, forEach(testing::_)).WillOnce(testing::WithArg<0>(
        [this](auto&& f) {
            EXPECT_CALL(mockDeathNotice, isLocalPlayerKiller()).Times(testing::AtMost(1)).WillOnce(testing::Return(GetParam().isLocalPlayerKiller));
            EXPECT_CALL(mockDeathNotice, wasSpawnedThisRound()).Times(testing::AtMost(1)).WillOnce(testing::Return(GetParam().wasSpawnedThisRound));
            EXPECT_CALL(mockDeathNotice, markAsJustSpawned()).Times(GetParam().expectMarkAsJustSpawned ? 1 : 0);

            f(mockDeathNotice);
        }
    ));

    killfeedPreserver.run();
}

INSTANTIATE_TEST_SUITE_P(, KillfeedPreserverEnabledTest, testing::ValuesIn(
    std::to_array<KillfeedPreserverEnabledTestParam>({
        {.isLocalPlayerKiller = true, .wasSpawnedThisRound = true, .expectMarkAsJustSpawned = true},
        {.isLocalPlayerKiller = true, .wasSpawnedThisRound = false, .expectMarkAsJustSpawned = false},
        {.isLocalPlayerKiller = false, .wasSpawnedThisRound = true, .expectMarkAsJustSpawned = false},
        {.isLocalPlayerKiller = false, .wasSpawnedThisRound = false, .expectMarkAsJustSpawned = false}
    })
));
