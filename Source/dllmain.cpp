#include "Utils/ManuallyDestructible.h"
#include "Platform/Macros/IsPlatform.h"

#include "BuildConfig.h"
#include "GlobalContext.h"

#if IS_LINUX()
#include <signal.h>
#endif

constinit ManuallyDestructible<GlobalContext> GlobalContext::globalContext;
alignas(FreeMemoryRegionList::minimumAlignment()) constinit std::byte GlobalContext::storage[build::MEMORY_CAPACITY];

#include "MemoryAllocation/MemoryAllocatorBase.h"

std::byte* MemoryAllocatorBase::allocate(std::size_t size) noexcept
{
    return GlobalContext::instance().freeRegionList.allocate(size);
}

void MemoryAllocatorBase::deallocate(std::byte* memory, std::size_t size) noexcept
{
    return GlobalContext::instance().freeRegionList.deallocate(memory, size);
}

void operator delete(void*, std::size_t) noexcept
{
    // for placement new to work
}

#include "Endpoints.h"
#include "GameClasses/Implementation/GameClassImplementationInstances.h"

#if IS_WIN64()
#include "MemoryPatterns/Windows/WindowsPatterns.h"

#include <bit>
#include <intrin.h>
#include <Windows.h>

BOOL APIENTRY DllMain(HMODULE moduleHandle, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
        GlobalContext::initializeInstance();
    return TRUE;
}

win::Peb* WindowsPlatformApi::getPeb() noexcept
{
    static_assert(IS_WIN64());
    return std::bit_cast<win::Peb*>(__readgsqword(0x60));
}

void WindowsPlatformApi::debugBreak() noexcept
{
    __debugbreak();
}

#elif IS_LINUX()

#include "MemoryPatterns/Linux/LinuxPatterns.h"

void __attribute__((constructor)) DllEntryPoint()
{
    GlobalContext::initializeInstance();
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
    (void)raise(SIGTRAP);
}

char* LinuxPlatformApi::getenv(const char* name) noexcept
{
    return ::getenv(name);
}

#endif
