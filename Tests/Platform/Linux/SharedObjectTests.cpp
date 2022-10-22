#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Platform/Linux/SharedObject.h>

namespace linux_platform
{
namespace
{

struct MockDynamicLibraryWrapper {
    MOCK_METHOD(void*, dlopen, (const char* file, int mode), (const noexcept));
    MOCK_METHOD(void*, dlsym, (void* handle, const char* name), (const noexcept));
    MOCK_METHOD(int, dlclose, (void* handle), (const noexcept));
};

class LinuxPlatform_SharedObjectTest : public testing::Test {
protected:
    testing::StrictMock<MockDynamicLibraryWrapper> dlWrapper;
    void* dummyHandleValue = reinterpret_cast<void*>(0x12345678);
    static constexpr auto dummyLibraryName = "lib.so";

    auto createSharedObject(const char* name)
    {
        return SharedObject<const MockDynamicLibraryWrapper&>{ dlWrapper, name };
    }
};

TEST_F(LinuxPlatform_SharedObjectTest, LibraryIsOpenedWithCorrectFlags) {
    EXPECT_CALL(dlWrapper, dlopen(testing::_, RTLD_LAZY | RTLD_NOLOAD)).WillOnce(testing::Return(dummyHandleValue));
    EXPECT_CALL(dlWrapper, dlclose(dummyHandleValue));

    createSharedObject(dummyLibraryName);
}

TEST_F(LinuxPlatform_SharedObjectTest, LibraryIsNotBeginClosedWhenOpeningFailed) {
    EXPECT_CALL(dlWrapper, dlopen(testing::_, testing::_)).WillOnce(testing::Return(nullptr));
    EXPECT_CALL(dlWrapper, dlclose(testing::_)).Times(0);

    createSharedObject(dummyLibraryName);
}

class LinuxPlatform_SharedObject_HandleTest : public LinuxPlatform_SharedObjectTest, public testing::WithParamInterface<std::uintptr_t> {};

TEST_P(LinuxPlatform_SharedObject_HandleTest, LibraryIsClosedWithTheHandleThatWasOpened) {
    EXPECT_CALL(dlWrapper, dlopen(testing::_, testing::_)).WillOnce(testing::Return(reinterpret_cast<void*>(GetParam())));
    EXPECT_CALL(dlWrapper, dlclose(reinterpret_cast<void*>(GetParam())));

    createSharedObject(dummyLibraryName);
}

INSTANTIATE_TEST_SUITE_P(, LinuxPlatform_SharedObject_HandleTest, testing::Values(0x12345678, std::uintptr_t(-1)));

class LinuxPlatform_SharedObject_NameTest : public LinuxPlatform_SharedObjectTest, public testing::WithParamInterface<const char*> {};

TEST_P(LinuxPlatform_SharedObject_NameTest, LibraryWithCorrectNameIsOpened) {
    EXPECT_CALL(dlWrapper, dlopen(testing::StrEq(GetParam()), testing::_)).WillOnce(testing::Return(dummyHandleValue));
    EXPECT_CALL(dlWrapper, dlclose(dummyHandleValue));

    createSharedObject(GetParam());
}

INSTANTIATE_TEST_SUITE_P(, LinuxPlatform_SharedObject_NameTest, testing::Values("client.so", "engine.so"));

}
}
