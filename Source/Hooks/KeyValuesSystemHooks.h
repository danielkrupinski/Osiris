#pragma once

#include <Endpoints.h>
#include <Platform/Macros/CallingConventions.h>
#include <RetSpoof/FunctionInvoker.h>

namespace csgo { struct KeyValuesSystemPOD; }

void* STDCALL_CONV allocKeyValuesMemory(LINUX_ARGS(void* thisptr, ) int size) noexcept;

template <typename HookImpl>
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

private:
    HookImpl hookImpl;

    void* (THISCALL_CONV* originalAllocKeyValuesMemory)(csgo::KeyValuesSystemPOD* thisptr, int size);
};
