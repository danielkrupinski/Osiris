#pragma once

#include <Endpoints.h>
#include <Platform/Macros/CallingConventions.h>
#include <RetSpoof/FunctionInvoker.h>

template <typename HookImpl>
class BspQueryHooks {
public:
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

private:
    HookImpl hookImpl;

    int (THISCALL_CONV* originalListLeavesInBox)(void* engineSpatialQuery, const csgo::Vector* mins, const csgo::Vector* maxs, unsigned short* list, int listMax);
};
