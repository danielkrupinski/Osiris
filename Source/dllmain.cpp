#include "Utils/ManuallyDestructible.h"
#include "Platform/Macros/IsPlatform.h"

#include "GlobalContext/GlobalContext.h"
#include "MemoryAllocation/MemoryAllocatorBaseImpl.h"

constinit ManuallyDestructible<GlobalContext> GlobalContext::globalContext;

#include "EntryPoints/EntryPoints.h"

#if IS_WIN64()
#include "BuildConfig.h"
#include "MemoryPatterns/Windows/WindowsPatterns.h"
#include "Platform/Windows/WindowsPlatformApiImpl.h"

#include <Windows.h>

void operator delete(void*, std::size_t) noexcept
{
    // for 'placement new' to work
}

extern "C" std::size_t DllMain(HMODULE, DWORD reason, LPVOID) noexcept
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
