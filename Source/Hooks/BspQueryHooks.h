#pragma once

#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>
#include <Utils/MemorySection.h>
#include <Vmt/VmtLengthCalculator.h>

namespace csgo { struct Vector; }

class BspQueryHooks {
public:
    explicit BspQueryHooks(MemorySection engineCodeSection)
        : hookImpl{ VmtLengthCalculator{ engineCodeSection } }
    {
    }

    void install(void* engineSpatialQuery)
    {
        hookImpl.init(engineSpatialQuery);
        originalListLeavesInBox = reinterpret_cast<decltype(originalListLeavesInBox)>(hookImpl.hookAt(6, &listLeavesInBox));
    }

    void uninstall()
    {
        hookImpl.restore();
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
