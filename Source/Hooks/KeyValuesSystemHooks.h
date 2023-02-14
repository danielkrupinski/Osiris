#pragma once

#include <Endpoints.h>
#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>

namespace csgo { struct KeyValuesSystemPOD; }

class KeyValuesSystemHooks {
public:
    void install(csgo::KeyValuesSystemPOD* keyValuesSystem)
    {
        hookImpl.init(keyValuesSystem);
        originalAllocKeyValuesMemory = reinterpret_cast<decltype(originalAllocKeyValuesMemory)>(hookImpl.hookAt(2, &allocKeyValuesMemory));
    }

    void uninstall()
    {
        hookImpl.restore();
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
