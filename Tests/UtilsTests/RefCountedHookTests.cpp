#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Utils/RefCountedHook.h>

namespace
{

struct MockHook : RefCountedHook<MockHook> {
    MOCK_METHOD(bool, isInstalled, (), (const noexcept));
    MOCK_METHOD(void, install, (), (noexcept));
    MOCK_METHOD(void, uninstall, (), (noexcept));
};

class RefCountedHookTest : public testing::Test {
protected:
    testing::StrictMock<MockHook> hook;
};

using testing::Return;

TEST_F(RefCountedHookTest, WillBeInstalledIfReferenced) {
    hook.incrementReferenceCount();
    hook.incrementReferenceCount();
    hook.decrementReferenceCount();

    EXPECT_CALL(hook, isInstalled()).WillOnce(Return(false));
    EXPECT_CALL(hook, install());
    hook.update();
}

TEST_F(RefCountedHookTest, WillNotBeInstalledIfNotReferenced) {
    hook.incrementReferenceCount();
    hook.decrementReferenceCount();

    EXPECT_CALL(hook, isInstalled()).WillOnce(Return(false));
    EXPECT_CALL(hook, install()).Times(0);
    hook.update();
}

TEST_F(RefCountedHookTest, WillBeUninstalledIfNotReferenced) {
    hook.incrementReferenceCount();
    hook.decrementReferenceCount();

    EXPECT_CALL(hook, isInstalled()).WillOnce(Return(true));
    EXPECT_CALL(hook, uninstall());
    hook.update();
}

TEST_F(RefCountedHookTest, WillNotBeUninstalledIfReferenced) {
    hook.incrementReferenceCount();
    hook.incrementReferenceCount();
    hook.decrementReferenceCount();

    EXPECT_CALL(hook, isInstalled()).WillOnce(Return(true));
    EXPECT_CALL(hook, uninstall()).Times(0);
    hook.update();
}

TEST_F(RefCountedHookTest, WillBeForceUninstalledWhenInstalled) {
    EXPECT_CALL(hook, isInstalled()).WillOnce(Return(true));
    EXPECT_CALL(hook, uninstall());
    hook.forceUninstall();
}

TEST_F(RefCountedHookTest, WillNotBeForceUninstalledWhenNotInstalled) {
    EXPECT_CALL(hook, isInstalled()).WillOnce(Return(false));
    EXPECT_CALL(hook, uninstall()).Times(0);
    hook.forceUninstall();
}

}
