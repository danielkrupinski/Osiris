#pragma once

#include <CSGO/Constants/ItemId.h>
#include <HookType.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>
#include <Vmt/VmtLengthCalculator.h>

namespace csgo
{
    struct InventoryManagerPOD;
    enum class Team;
}

class InventoryManagerHooks {
public:
    explicit InventoryManagerHooks(const VmtLengthCalculator& vmtLengthCalculator)
        : hookImpl{ vmtLengthCalculator }
    {
    }

    void install(csgo::InventoryManagerPOD* inventoryManager)
    {
        hookImpl.install(*reinterpret_cast<std::uintptr_t**>(inventoryManager));
        originalUpdateInventoryEquippedState = reinterpret_cast<decltype(originalUpdateInventoryEquippedState)>(hookImpl.hook(WIN32_LINUX(29, 30), std::uintptr_t(&updateInventoryEquippedState)));
    }

    void uninstall(csgo::InventoryManagerPOD* inventoryManager)
    {
        hookImpl.uninstall(*reinterpret_cast<std::uintptr_t**>(inventoryManager));
    }

    [[nodiscard]] auto getOriginalUpdateInventoryEquippedState() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalUpdateInventoryEquippedState };
    }

    static void FASTCALL_CONV updateInventoryEquippedState(FASTCALL_THIS(csgo::InventoryManagerPOD* thisptr), std::uintptr_t inventory, csgo::ItemId itemID, csgo::Team team, int slot, bool swap) noexcept;

private:
    HookType hookImpl;

    void (THISCALL_CONV* originalUpdateInventoryEquippedState)(csgo::InventoryManagerPOD* thisptr, std::uintptr_t inventory, csgo::ItemId itemID, csgo::Team team, int slot, bool swap);
};
