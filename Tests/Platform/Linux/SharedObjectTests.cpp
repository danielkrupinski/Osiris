#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Platform/Linux/LinuxDynamicLibrary.h>

namespace
{

using testing::_;
using testing::Return;
using testing::Values;

struct MockLinuxPlatformApi {
    MOCK_METHOD(void*, dlopen, (const char* file, int mode), (const noexcept));
    MOCK_METHOD(void*, dlsym, (void* handle, const char* name), (const noexcept));
    MOCK_METHOD(int, dlclose, (void* handle), (const noexcept));
    MOCK_METHOD(int, dlinfo, (void* handle, int request, void* info), (const noexcept));
    MOCK_METHOD(int, open, (const char* pathname, int flags), (const noexcept));
    MOCK_METHOD(int, close, (int fd), (const noexcept));
    MOCK_METHOD(int, fstat, (int fd, struct stat* buf), (const noexcept));
    MOCK_METHOD(void*, mmap, (void* addr, size_t length, int prot, int flags, int fd, off_t offset), (const noexcept));
    MOCK_METHOD(int, munmap, (void* addr, size_t length), (const noexcept));
    MOCK_METHOD(char*, getenv, (const char* name), (const noexcept));

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
        return LinuxDynamicLibrary{ name };
    }
};

TEST_F(TestLinuxDynamicLibrary, LibraryIsOpenedWithCorrectFlags) {
    EXPECT_CALL(platformApi, dlopen(_, RTLD_LAZY | RTLD_NOLOAD)).WillOnce(Return(dummyHandleValue));
    EXPECT_CALL(platformApi, dlclose(dummyHandleValue));

    createSharedObject(dummyLibraryName);
}

TEST_F(TestLinuxDynamicLibrary, LibraryIsNotBeingClosedWhenOpeningFailed) {
    EXPECT_CALL(platformApi, dlopen(_, _)).WillOnce(Return(nullptr));
    EXPECT_CALL(platformApi, dlclose(_)).Times(0);

    createSharedObject(dummyLibraryName);
}

TEST_F(TestLinuxDynamicLibrary, GetFunctionAddressReturnsZeroIfOpeningFailed) {
    EXPECT_CALL(platformApi, dlopen(_, _)).WillOnce(Return(nullptr));
    EXPECT_CALL(platformApi, dlclose(_)).Times(0);
    EXPECT_CALL(platformApi, dlsym(_, _)).Times(0);

    const auto dll = createSharedObject(dummyLibraryName);
    EXPECT_EQ(dll.getFunctionAddress("functionA").as<std::uintptr_t>(), 0);
}

TEST_F(TestLinuxDynamicLibrary, GetLinkMapReturnsNullIfOpeningFailed) {
    EXPECT_CALL(platformApi, dlopen(_, _)).WillOnce(Return(nullptr));
    EXPECT_CALL(platformApi, dlclose(_)).Times(0);
    EXPECT_CALL(platformApi, dlinfo(_, _, _)).Times(0);

    const auto dll = createSharedObject(dummyLibraryName);
    EXPECT_EQ(dll.getLinkMap(), nullptr);
}

class TestLinuxDynamicLibrary_Handle
    : public TestLinuxDynamicLibrary, public testing::WithParamInterface<std::uintptr_t> {};

TEST_P(TestLinuxDynamicLibrary_Handle, LibraryIsClosedWithTheHandleThatWasOpened) {
    EXPECT_CALL(platformApi, dlopen(_, _)).WillOnce(Return(reinterpret_cast<void*>(GetParam())));
    EXPECT_CALL(platformApi, dlclose(reinterpret_cast<void*>(GetParam())));

    createSharedObject(dummyLibraryName);
}

INSTANTIATE_TEST_SUITE_P(, TestLinuxDynamicLibrary_Handle, testing::Values(0x12345678, std::uintptr_t(-1)));

class TestLinuxDynamicLibrary_FunctionAddress
    : public TestLinuxDynamicLibrary, public testing::WithParamInterface<std::tuple<void*, const char*, void*>> {};

TEST_P(TestLinuxDynamicLibrary_FunctionAddress, GetFunctionAddressReturnsCorrectAddress) {
    const auto [handle, functionName, functionAddress] = GetParam();

    EXPECT_CALL(platformApi, dlopen(_, _)).WillOnce(Return(handle));
    EXPECT_CALL(platformApi, dlclose(handle));

    const auto dll = createSharedObject(dummyLibraryName);
    EXPECT_CALL(platformApi, dlsym(handle, testing::StrEq(functionName))).WillOnce(Return(functionAddress));
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

    EXPECT_CALL(platformApi, dlopen(_, _)).WillOnce(Return(handle));
    EXPECT_CALL(platformApi, dlclose(handle));

    const auto dll = createSharedObject(dummyLibraryName);
    EXPECT_CALL(platformApi, dlinfo(handle, RTLD_DI_LINKMAP, _)).WillOnce(SetLinkMap{ linkMap });
    EXPECT_EQ(dll.getLinkMap(), linkMap);
}

INSTANTIATE_TEST_SUITE_P(, TestLinuxDynamicLibrary_LinkMap, testing::Combine(
    Values((void*)0x12345678, (void*)-1),
    Values((link_map*)0x123456789ABC, (link_map*)-1)
));

class TestLinuxDynamicLibrary_String
    : public TestLinuxDynamicLibrary, public testing::WithParamInterface<const char*> {};

TEST_P(TestLinuxDynamicLibrary_String, LibraryWithCorrectNameIsOpened) {
    EXPECT_CALL(platformApi, dlopen(testing::StrEq(GetParam()), _)).WillOnce(Return(dummyHandleValue));
    EXPECT_CALL(platformApi, dlclose(dummyHandleValue));

    createSharedObject(GetParam());
}

INSTANTIATE_TEST_SUITE_P(, TestLinuxDynamicLibrary_String, testing::Values("client.so", "engine.so"));

}

void* LinuxPlatformApi::dlopen(const char* file, int mode) noexcept
{
    return MockLinuxPlatformApi::instance->dlopen(file, mode);
}

void* LinuxPlatformApi::dlsym(void* handle, const char* name) noexcept
{
    return MockLinuxPlatformApi::instance->dlsym(handle, name);
}

int LinuxPlatformApi::dlclose(void* handle) noexcept
{
    return MockLinuxPlatformApi::instance->dlclose(handle);
}

int LinuxPlatformApi::dlinfo(void* handle, int request, void* info) noexcept
{
    return MockLinuxPlatformApi::instance->dlinfo(handle, request, info);
}

int LinuxPlatformApi::open(const char* pathname, int flags) noexcept
{
    return MockLinuxPlatformApi::instance->open(pathname, flags);
}

int LinuxPlatformApi::close(int fd) noexcept
{
    return MockLinuxPlatformApi::instance->close(fd);
}

int LinuxPlatformApi::fstat(int fd, struct stat* buf) noexcept
{
    return MockLinuxPlatformApi::instance->fstat(fd, buf);
}

void* LinuxPlatformApi::mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset) noexcept
{
    return MockLinuxPlatformApi::instance->mmap(addr, length, prot, flags, fd, offset);
}

int LinuxPlatformApi::munmap(void* addr, size_t length) noexcept
{
    return MockLinuxPlatformApi::instance->munmap(addr, length);
}

char* LinuxPlatformApi::getenv(const char* name) noexcept
{
    return MockLinuxPlatformApi::instance->getenv(name);
}
