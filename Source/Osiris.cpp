#include <memory>

#include "Platform/Macros/IsPlatform.h"

#if IS_WIN32() || IS_WIN64()
#include <bit>
#include <clocale>
#include <Windows.h>

#include <intrin.h>
#include "Platform/Windows/Win.h"
#include "Platform/Windows/WindowsPlatformApi.h"

#endif

#if IS_LINUX()
#include <signal.h>
#endif

#include "GlobalContext.h"
#include "Hooks.h"

namespace
{
    std::optional<GlobalContext> globalContext;
}

#include "Endpoints.h"
#include "MemoryAllocation/MemoryAllocatorBase.h"

template <typename... Args>
void initializeGlobalContext(Args&&... args)
{
    globalContext.emplace(std::forward<Args>(args)...);
    globalContext->enable();
}

std::byte* MemoryAllocatorBase::allocate(std::size_t size) noexcept
{
    return globalContext->fixedAllocator.allocate(size);
}

void MemoryAllocatorBase::deallocate(std::byte* memory, std::size_t size) noexcept
{
    globalContext->fixedAllocator.deallocate(memory, size);
}

#if IS_WIN32() || IS_WIN64()

extern "C" BOOL WINAPI _CRT_INIT(HMODULE moduleHandle, DWORD reason, LPVOID reserved);

BOOL APIENTRY DllEntryPoint(HMODULE moduleHandle, DWORD reason, LPVOID reserved)
{
    if (!_CRT_INIT(moduleHandle, reason, reserved))
        return FALSE;

    if (reason == DLL_PROCESS_ATTACH) {
        std::setlocale(LC_CTYPE, ".utf8");
        initializeGlobalContext(moduleHandle);
    }
    return TRUE;
}

win::Peb* WindowsPlatformApi::getPeb() noexcept
{
    static_assert(IS_WIN32() || IS_WIN64());
#if IS_WIN32()
    return std::bit_cast<win::Peb*>(__readfsdword(0x30));
#elif IS_WIN64()
    return std::bit_cast<win::Peb*>(__readgsqword(0x60));
#endif
}

void WindowsPlatformApi::debugBreak() noexcept
{
    __debugbreak();
}

#elif IS_LINUX()

void __attribute__((constructor)) DllEntryPoint()
{
    initializeGlobalContext();
}

void* LinuxPlatformApi::dlopen(const char* file, int mode) noexcept
{
    return ::dlopen(file, mode);
}

void* LinuxPlatformApi::dlsym(void* handle, const char* name) noexcept
{
    return ::dlsym(handle, name);
}

int LinuxPlatformApi::dlclose(void* handle) noexcept
{
    return ::dlclose(handle);
}

int LinuxPlatformApi::dlinfo(void* handle, int request, void* info) noexcept
{
    return ::dlinfo(handle, request, info);
}

int LinuxPlatformApi::open(const char* pathname, int flags) noexcept
{
    return ::open(pathname, flags);
}

int LinuxPlatformApi::close(int fd) noexcept
{
    return ::close(fd);
}

int LinuxPlatformApi::fstat(int fd, struct stat* buf) noexcept
{
    return ::fstat(fd, buf);
}

void* LinuxPlatformApi::mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset) noexcept
{
    return ::mmap(addr, length, prot, flags, fd, offset);
}

int LinuxPlatformApi::munmap(void* addr, size_t length) noexcept
{
    return ::munmap(addr, length);
}

void LinuxPlatformApi::debugBreak() noexcept
{
    raise(SIGTRAP);
}

char* LinuxPlatformApi::getenv(const char* name) noexcept
{
    return ::getenv(name);
}

#endif
