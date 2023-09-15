#pragma once

#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>
#include <Utils/RefCountedHook.h>
#include <Vmt/VmtLengthCalculator.h>

namespace csgo { struct KeyValuesSystemPOD; }

class KeyValuesSystemHooks : public RefCountedHook<KeyValuesSystemHooks> {
public:
    explicit KeyValuesSystemHooks(const VmtLengthCalculator& vmtLengthCalculator, csgo::KeyValuesSystemPOD* keyValuesSystem)
        : hookImpl{ vmtLengthCalculator }, keyValuesSystem{ keyValuesSystem }
    {
    }

    [[nodiscard]] auto getOriginalAllocKeyValuesMemory() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalAllocKeyValuesMemory };
    }

    static void* FASTCALL_CONV allocKeyValuesMemory(FASTCALL_THIS(csgo::KeyValuesSystemPOD* thisptr), int size) noexcept;

private:
    void install()
    {
        hookImpl.install(*reinterpret_cast<std::uintptr_t**>(keyValuesSystem));
        originalAllocKeyValuesMemory = hookImpl.hook(2, &allocKeyValuesMemory);
    }

    void uninstall()
    {
        hookImpl.uninstall(*reinterpret_cast<std::uintptr_t**>(keyValuesSystem));
    }

    [[nodiscard]] bool isInstalled() const noexcept
    {
        return hookImpl.isInstalled(*reinterpret_cast<std::uintptr_t**>(keyValuesSystem));
    }

    friend RefCountedHook;

    HookType hookImpl;
    csgo::KeyValuesSystemPOD* keyValuesSystem;

    void* (THISCALL_CONV* originalAllocKeyValuesMemory)(csgo::KeyValuesSystemPOD* thisptr, int size);
};
