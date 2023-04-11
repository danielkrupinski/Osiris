#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Platform/Linux/SharedObject.h>

namespace linux_platform
{
namespace
{

struct MockPlatformApi {
    MOCK_METHOD(void*, dlopen_, (const char* file, int mode), (const noexcept));
    MOCK_METHOD(void*, dlsym_, (void* handle, const char* name), (const noexcept));
    MOCK_METHOD(int, dlclose_, (void* handle), (const noexcept));

    static void* dlopen(const char* file, int mode) noexcept
    {
        return instance->dlopen_(file, mode);
    }

    static void* dlsym(void* handle, const char* name) noexcept
    {
        return instance->dlsym_(handle, name);
    }

    static int dlclose(void* handle) noexcept
    {
        return instance->dlclose_(handle);
    }

    static inline testing::StrictMock<MockPlatformApi>* instance = nullptr;
};

class LinuxPlatform_SharedObjectTest : public testing::Test {
protected:
    LinuxPlatform_SharedObjectTest()
    {
        MockPlatformApi::instance = &platformApi;
    }

    testing::StrictMock<MockPlatformApi> platformApi;
    void* dummyHandleValue = reinterpret_cast<void*>(0x12345678);
    static constexpr auto dummyLibraryName = "lib.so";

    auto createSharedObject(const char* name)
    {
        return SharedObject<MockPlatformApi>{ name };
    }
};

TEST_F(LinuxPlatform_SharedObjectTest, LibraryIsOpenedWithCorrectFlags) {
    EXPECT_CALL(platformApi, dlopen_(testing::_, RTLD_LAZY | RTLD_NOLOAD)).WillOnce(testing::Return(dummyHandleValue));
    EXPECT_CALL(platformApi, dlclose_(dummyHandleValue));

    createSharedObject(dummyLibraryName);
}

TEST_F(LinuxPlatform_SharedObjectTest, LibraryIsNotBeingClosedWhenOpeningFailed) {
    EXPECT_CALL(platformApi, dlopen_(testing::_, testing::_)).WillOnce(testing::Return(nullptr));
    EXPECT_CALL(platformApi, dlclose_(testing::_)).Times(0);

    createSharedObject(dummyLibraryName);
}

class LinuxPlatform_SharedObject_HandleTest : public LinuxPlatform_SharedObjectTest, public testing::WithParamInterface<std::uintptr_t> {};

TEST_P(LinuxPlatform_SharedObject_HandleTest, LibraryIsClosedWithTheHandleThatWasOpened) {
    EXPECT_CALL(platformApi, dlopen_(testing::_, testing::_)).WillOnce(testing::Return(reinterpret_cast<void*>(GetParam())));
    EXPECT_CALL(platformApi, dlclose_(reinterpret_cast<void*>(GetParam())));

    createSharedObject(dummyLibraryName);
}

INSTANTIATE_TEST_SUITE_P(, LinuxPlatform_SharedObject_HandleTest, testing::Values(0x12345678, std::uintptr_t(-1)));

class LinuxPlatform_SharedObject_NameTest : public LinuxPlatform_SharedObjectTest, public testing::WithParamInterface<const char*> {};

TEST_P(LinuxPlatform_SharedObject_NameTest, LibraryWithCorrectNameIsOpened) {
    EXPECT_CALL(platformApi, dlopen_(testing::StrEq(GetParam()), testing::_)).WillOnce(testing::Return(dummyHandleValue));
    EXPECT_CALL(platformApi, dlclose_(dummyHandleValue));

    createSharedObject(GetParam());
}

INSTANTIATE_TEST_SUITE_P(, LinuxPlatform_SharedObject_NameTest, testing::Values("client.so", "engine.so"));

}
}
