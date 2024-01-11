#include "Utils/ManuallyDestructible.h"
#include "Platform/Macros/IsPlatform.h"

#include "GlobalContext/GlobalContext.h"

constinit ManuallyDestructible<GlobalContext> GlobalContext::globalContext;

#include "MemoryAllocation/MemoryAllocatorBase.h"

std::byte* MemoryAllocatorBase::allocate(std::size_t size) noexcept
{
    return GlobalContext::instance().freeRegionList().allocate(size);
}

void MemoryAllocatorBase::deallocate(std::byte* memory, std::size_t size) noexcept
{
    return GlobalContext::instance().freeRegionList().deallocate(memory, size);
}

#include "Endpoints.h"
#include "GameClasses/Implementation/GameClassImplementationInstances.h"

#if IS_WIN64()
#include "MemoryPatterns/Windows/WindowsPatterns.h"
#include "Platform/Windows/WindowsPlatformApiImpl.h"

#include <Windows.h>

void operator delete(void*, std::size_t) noexcept
{
    // for 'placement new' to work
}

BOOL APIENTRY DllMain(HMODULE, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
        GlobalContext::initializeInstance();
    return TRUE;
}

#elif IS_LINUX()

#include "MemoryPatterns/Linux/LinuxPatterns.h"
#include "Platform/Linux/LinuxPlatformApiImpl.h"

void __attribute__((constructor)) DllEntryPoint()
{
    GlobalContext::initializeInstance();
}

#endif
