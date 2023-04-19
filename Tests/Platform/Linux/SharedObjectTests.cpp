#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Platform/Linux/LinuxDynamicLibrary.h>

namespace
{

using testing::_;
using testing::Return;
using testing::Values;

struct MockLinuxPlatformApi {
    MOCK_METHOD(void*, dlopen_, (const char* file, int mode), (const noexcept));
    MOCK_METHOD(void*, dlsym_, (void* handle, const char* name), (const noexcept));
    MOCK_METHOD(int, dlclose_, (void* handle), (const noexcept));
    MOCK_METHOD(int, dlinfo_, (void* handle, int request, void* info), (const noexcept));

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

    static int dlinfo(void* handle, int request, void* info) noexcept
    {
        return instance->dlinfo_(handle, request, info);
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
    EXPECT_CALL(platformApi, dlopen_(_, RTLD_LAZY | RTLD_NOLOAD)).WillOnce(Return(dummyHandleValue));
    EXPECT_CALL(platformApi, dlclose_(dummyHandleValue));

    createSharedObject(dummyLibraryName);
}

TEST_F(TestLinuxDynamicLibrary, LibraryIsNotBeingClosedWhenOpeningFailed) {
    EXPECT_CALL(platformApi, dlopen_(_, _)).WillOnce(Return(nullptr));
    EXPECT_CALL(platformApi, dlclose_(_)).Times(0);

    createSharedObject(dummyLibraryName);
}

TEST_F(TestLinuxDynamicLibrary, GetFunctionAddressReturnsZeroIfOpeningFailed) {
    EXPECT_CALL(platformApi, dlopen_(_, _)).WillOnce(Return(nullptr));
    EXPECT_CALL(platformApi, dlclose_(_)).Times(0);
    EXPECT_CALL(platformApi, dlsym_(_, _)).Times(0);

    const auto dll = createSharedObject(dummyLibraryName);
    EXPECT_EQ(dll.getFunctionAddress("functionA").get(), 0);
}

TEST_F(TestLinuxDynamicLibrary, GetLinkMapReturnsNullIfOpeningFailed) {
    EXPECT_CALL(platformApi, dlopen_(_, _)).WillOnce(Return(nullptr));
    EXPECT_CALL(platformApi, dlclose_(_)).Times(0);
    EXPECT_CALL(platformApi, dlinfo_(_, _, _)).Times(0);

    const auto dll = createSharedObject(dummyLibraryName);
    EXPECT_EQ(dll.getLinkMap(), nullptr);
}

class TestLinuxDynamicLibrary_Handle
    : public TestLinuxDynamicLibrary, public testing::WithParamInterface<std::uintptr_t> {};

TEST_P(TestLinuxDynamicLibrary_Handle, LibraryIsClosedWithTheHandleThatWasOpened) {
    EXPECT_CALL(platformApi, dlopen_(_, _)).WillOnce(Return(reinterpret_cast<void*>(GetParam())));
    EXPECT_CALL(platformApi, dlclose_(reinterpret_cast<void*>(GetParam())));

    createSharedObject(dummyLibraryName);
}

INSTANTIATE_TEST_SUITE_P(, TestLinuxDynamicLibrary_Handle, testing::Values(0x12345678, std::uintptr_t(-1)));

class TestLinuxDynamicLibrary_FunctionAddress
    : public TestLinuxDynamicLibrary, public testing::WithParamInterface<std::tuple<void*, const char*, void*>> {};

TEST_P(TestLinuxDynamicLibrary_FunctionAddress, GetFunctionAddressReturnsCorrectAddress) {
    const auto [handle, functionName, functionAddress] = GetParam();

    EXPECT_CALL(platformApi, dlopen_(_, _)).WillOnce(Return(handle));
    EXPECT_CALL(platformApi, dlclose_(handle));

    const auto dll = createSharedObject(dummyLibraryName);
    EXPECT_CALL(platformApi, dlsym_(handle, testing::StrEq(functionName))).WillOnce(Return(functionAddress));
    EXPECT_EQ(dll.getFunctionAddress(functionName).as<void*>(), functionAddress);
}

INSTANTIATE_TEST_SUITE_P(, TestLinuxDynamicLibrary_FunctionAddress, testing::Combine(
    Values((void*)0x12345678, (void*)-1),
    Values("function1", "function2"),
    Values((void*)0, (void*)0x123456789ABC, (void*)-1)
));

class TestLinuxDynamicLibrary_LinkMap
    : public TestLinuxDynamicLibrary, public testing::WithParamInterface<std::tuple<void*, link_map*>> {};

struct SetLinkMap {
    link_map* linkMap;

    int operator()(void* handle, int request, void* info) const
    {
        *reinterpret_cast<link_map**>(info) = linkMap;
        return 0;
    }
};

TEST_P(TestLinuxDynamicLibrary_LinkMap, GetLinkMapReturnsCorrectPointer) {
    const auto [handle, linkMap] = GetParam();

    EXPECT_CALL(platformApi, dlopen_(_, _)).WillOnce(Return(handle));
    EXPECT_CALL(platformApi, dlclose_(handle));

    const auto dll = createSharedObject(dummyLibraryName);
    EXPECT_CALL(platformApi, dlinfo_(handle, RTLD_DI_LINKMAP, _)).WillOnce(SetLinkMap{ linkMap });
    EXPECT_EQ(dll.getLinkMap(), linkMap);
}

INSTANTIATE_TEST_SUITE_P(, TestLinuxDynamicLibrary_LinkMap, testing::Combine(
    Values((void*)0x12345678, (void*)-1),
    Values((link_map*)0x123456789ABC, (link_map*)-1)
));

class TestLinuxDynamicLibrary_String
    : public TestLinuxDynamicLibrary, public testing::WithParamInterface<const char*> {};

TEST_P(TestLinuxDynamicLibrary_String, LibraryWithCorrectNameIsOpened) {
    EXPECT_CALL(platformApi, dlopen_(testing::StrEq(GetParam()), _)).WillOnce(Return(dummyHandleValue));
    EXPECT_CALL(platformApi, dlclose_(dummyHandleValue));

    createSharedObject(GetParam());
}

INSTANTIATE_TEST_SUITE_P(, TestLinuxDynamicLibrary_String, testing::Values("client.so", "engine.so"));

}
