#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Platform/Linux/LinuxDynamicLibrary.h>

namespace
{

using testing::_;

struct MockLinuxPlatformApi {
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

    static inline testing::StrictMock<MockLinuxPlatformApi>* instance = nullptr;
};

class TestLinuxDynamicLibrary : public testing::Test {
protected:
    TestLinuxDynamicLibrary()
    {
        MockLinuxPlatformApi::instance = &platformApi;
    }

    testing::StrictMock<MockLinuxPlatformApi> platformApi;
    void* dummyHandleValue = reinterpret_cast<void*>(0x1234567890ABCDEF);
    static constexpr auto dummyLibraryName = "lib.so";

    auto createSharedObject(const char* name)
    {
        return LinuxDynamicLibrary<MockLinuxPlatformApi>{ name };
    }
};

TEST_F(TestLinuxDynamicLibrary, LibraryIsOpenedWithCorrectFlags) {
    EXPECT_CALL(platformApi, dlopen_(_, RTLD_LAZY | RTLD_NOLOAD)).WillOnce(testing::Return(dummyHandleValue));
    EXPECT_CALL(platformApi, dlclose_(dummyHandleValue));

    createSharedObject(dummyLibraryName);
}

TEST_F(TestLinuxDynamicLibrary, LibraryIsNotBeingClosedWhenOpeningFailed) {
    EXPECT_CALL(platformApi, dlopen_(_, _)).WillOnce(testing::Return(nullptr));
    EXPECT_CALL(platformApi, dlclose_(_)).Times(0);

    createSharedObject(dummyLibraryName);
}

class TestLinuxDynamicLibrary_Handle : public TestLinuxDynamicLibrary, public testing::WithParamInterface<std::uintptr_t> {};

TEST_P(TestLinuxDynamicLibrary_Handle, LibraryIsClosedWithTheHandleThatWasOpened) {
    EXPECT_CALL(platformApi, dlopen_(_, _)).WillOnce(testing::Return(reinterpret_cast<void*>(GetParam())));
    EXPECT_CALL(platformApi, dlclose_(reinterpret_cast<void*>(GetParam())));

    createSharedObject(dummyLibraryName);
}

INSTANTIATE_TEST_SUITE_P(, TestLinuxDynamicLibrary_Handle, testing::Values(0x12345678, std::uintptr_t(-1)));

class TestLinuxDynamicLibrary_String : public TestLinuxDynamicLibrary, public testing::WithParamInterface<const char*> {};

TEST_P(TestLinuxDynamicLibrary_String, LibraryWithCorrectNameIsOpened) {
    EXPECT_CALL(platformApi, dlopen_(testing::StrEq(GetParam()), testing::_)).WillOnce(testing::Return(dummyHandleValue));
    EXPECT_CALL(platformApi, dlclose_(dummyHandleValue));

    createSharedObject(GetParam());
}

INSTANTIATE_TEST_SUITE_P(, TestLinuxDynamicLibrary_String, testing::Values("client.so", "engine.so"));

}
