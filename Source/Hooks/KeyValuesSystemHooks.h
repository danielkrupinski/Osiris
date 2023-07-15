#pragma once

#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>
#include <Vmt/VmtLengthCalculator.h>

namespace csgo { struct KeyValuesSystemPOD; }

class KeyValuesSystemHooks {
public:
    explicit KeyValuesSystemHooks(const VmtLengthCalculator& vmtLengthCalculator)
        : hookImpl{ vmtLengthCalculator }
    {
    }

    void install(csgo::KeyValuesSystemPOD* keyValuesSystem)
    {
        hookImpl.install(*reinterpret_cast<std::uintptr_t**>(keyValuesSystem));
        originalAllocKeyValuesMemory = reinterpret_cast<decltype(originalAllocKeyValuesMemory)>(hookImpl.hook(2, std::uintptr_t(&allocKeyValuesMemory)));
    }

    void uninstall(csgo::KeyValuesSystemPOD* keyValuesSystem)
    {
        hookImpl.uninstall(*reinterpret_cast<std::uintptr_t**>(keyValuesSystem));
    }

    [[nodiscard]] auto getOriginalAllocKeyValuesMemory() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalAllocKeyValuesMemory };
    }

    static void* FASTCALL_CONV allocKeyValuesMemory(FASTCALL_THIS(csgo::KeyValuesSystemPOD* thisptr), int size) noexcept;

private:
    HookType hookImpl;

    void* (THISCALL_CONV* originalAllocKeyValuesMemory)(csgo::KeyValuesSystemPOD* thisptr, int size);
};
