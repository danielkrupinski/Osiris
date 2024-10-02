#include "Utils/ManuallyDestructible.h"
#include "Platform/Macros/IsPlatform.h"

#include "GlobalContext/GlobalContext.h"
#include "MemoryAllocation/MemoryAllocatorBaseImpl.h"

constinit ManuallyDestructible<GlobalContext> GlobalContext::globalContext;

#include "Endpoints.h"

#if IS_WIN64()
#include "BuildConfig.h"
#include "MemoryPatterns/Windows/WindowsPatterns.h"
#include "Platform/Windows/WindowsPlatformApiImpl.h"

#include <Windows.h>

void operator delete(void*, std::size_t) noexcept
{
    // for 'placement new' to work
}

extern "C" std::uint32_t textSectionStartMarker;
extern "C" std::uint32_t textSectionEndMarker;

[[nodiscard]] std::size_t getTextSectionLengthToHide() noexcept
{
#ifdef NDEBUG
    if constexpr (build::kHideExecutableMemory) {
        const auto textSectionStartAddress{reinterpret_cast<std::uintptr_t>(&textSectionStartMarker)};
        const auto textSectionEndAddress{reinterpret_cast<std::uintptr_t>(&textSectionEndMarker + 1)};

        if (textSectionEndAddress >= textSectionStartAddress)
            return textSectionEndAddress - textSectionStartAddress;
        assert(false && "Invalid .text section start/end markers. Check your linker.");
    }
#endif
    return 0;
}

extern "C" std::size_t DllMain_cpp(HMODULE, DWORD reason, LPVOID) noexcept
{
    if (reason == DLL_PROCESS_ATTACH)
        GlobalContext::initializeInstance();
    return getTextSectionLengthToHide();
}

#elif IS_LINUX()

#include "MemoryPatterns/Linux/LinuxPatterns.h"
#include "Platform/Linux/LinuxPlatformApiImpl.h"

void __attribute__((constructor)) DllEntryPoint()
{
    GlobalContext::initializeInstance();
}

#endif
