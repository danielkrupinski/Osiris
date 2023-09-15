#pragma once

#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>
#include <Utils/RefCountedHook.h>
#include <Vmt/VmtLengthCalculator.h>

namespace csgo { struct Vector; }

class BspQueryHooks : public RefCountedHook<BspQueryHooks> {
public:
    BspQueryHooks(const VmtLengthCalculator& vmtLengthCalculator, void* engineSpatialQuery)
        : hookImpl{ vmtLengthCalculator }, engineSpatialQuery{ engineSpatialQuery }
    {
    }

    [[nodiscard]] auto getOriginalListLeavesInBox() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalListLeavesInBox };
    }

    static int FASTCALL_CONV listLeavesInBox(FASTCALL_THIS(void* thisptr), const csgo::Vector& mins, const csgo::Vector& maxs, unsigned short* list, int listMax) noexcept;

private:
    void install() noexcept
    {
        hookImpl.install(*reinterpret_cast<std::uintptr_t**>(engineSpatialQuery));
        originalListLeavesInBox = hookImpl.hook(6, &listLeavesInBox);
    }

    void uninstall() noexcept
    {
        hookImpl.uninstall(*reinterpret_cast<std::uintptr_t**>(engineSpatialQuery));
    }

    [[nodiscard]] bool isInstalled() const noexcept
    {
        return hookImpl.isInstalled(*reinterpret_cast<std::uintptr_t**>(engineSpatialQuery));
    }

    friend RefCountedHook;

    HookType hookImpl;
    void* engineSpatialQuery;

    int (THISCALL_CONV* originalListLeavesInBox)(void* engineSpatialQuery, const csgo::Vector* mins, const csgo::Vector* maxs, unsigned short* list, int listMax);
};
