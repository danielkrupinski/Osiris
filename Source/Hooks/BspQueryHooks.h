#pragma once

#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>
#include <Vmt/VmtLengthCalculator.h>

namespace csgo { struct Vector; }

class BspQueryHooks {
public:
    explicit BspQueryHooks(const VmtLengthCalculator& vmtLengthCalculator)
        : hookImpl{ vmtLengthCalculator }
    {
    }

    void install(void* engineSpatialQuery)
    {
        hookImpl.install(*reinterpret_cast<std::uintptr_t**>(engineSpatialQuery));
        originalListLeavesInBox = reinterpret_cast<decltype(originalListLeavesInBox)>(hookImpl.hook(6, std::uintptr_t(&listLeavesInBox)));
    }

    void uninstall(void* engineSpatialQuery)
    {
        hookImpl.uninstall(*reinterpret_cast<std::uintptr_t**>(engineSpatialQuery));
    }

    [[nodiscard]] auto getOriginalListLeavesInBox() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalListLeavesInBox };
    }

    static int FASTCALL_CONV listLeavesInBox(FASTCALL_THIS(void* thisptr), const csgo::Vector& mins, const csgo::Vector& maxs, unsigned short* list, int listMax) noexcept;

private:
    HookType hookImpl;

    int (THISCALL_CONV* originalListLeavesInBox)(void* engineSpatialQuery, const csgo::Vector* mins, const csgo::Vector* maxs, unsigned short* list, int listMax);
};
