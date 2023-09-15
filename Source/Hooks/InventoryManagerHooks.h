#pragma once

#include <CSGO/Constants/ItemId.h>
#include <HookType.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>
#include <Utils/RefCountedHook.h>
#include <Vmt/VmtLengthCalculator.h>

namespace csgo
{
    struct InventoryManagerPOD;
    enum class Team;
}

class InventoryManagerHooks : public RefCountedHook<InventoryManagerHooks> {
public:
    explicit InventoryManagerHooks(const VmtLengthCalculator& vmtLengthCalculator, csgo::InventoryManagerPOD* inventoryManager)
        : hookImpl{ vmtLengthCalculator }, inventoryManager{ inventoryManager }
    {
    }

    [[nodiscard]] auto getOriginalUpdateInventoryEquippedState() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalUpdateInventoryEquippedState };
    }

    static void FASTCALL_CONV updateInventoryEquippedState(FASTCALL_THIS(csgo::InventoryManagerPOD* thisptr), std::uintptr_t inventory, csgo::ItemId itemID, csgo::Team team, int slot, bool swap) noexcept;

private:
    void install()
    {
        hookImpl.install(*reinterpret_cast<std::uintptr_t**>(inventoryManager));
        originalUpdateInventoryEquippedState = hookImpl.hook(WIN32_LINUX(29, 30), &updateInventoryEquippedState);
    }

    void uninstall()
    {
        hookImpl.uninstall(*reinterpret_cast<std::uintptr_t**>(inventoryManager));
    }

    [[nodiscard]] bool isInstalled() const noexcept
    {
        return hookImpl.isInstalled(*reinterpret_cast<std::uintptr_t**>(inventoryManager));
    }

    friend RefCountedHook;

    HookType hookImpl;
    csgo::InventoryManagerPOD* inventoryManager;

    void (THISCALL_CONV* originalUpdateInventoryEquippedState)(csgo::InventoryManagerPOD* thisptr, std::uintptr_t inventory, csgo::ItemId itemID, csgo::Team team, int slot, bool swap);
};
