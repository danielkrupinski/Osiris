#pragma once

#include <CSGO/Constants/ItemId.h>
#include <Endpoints.h>
#include <HookType.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>

namespace csgo
{
    struct InventoryManagerPOD;
    enum class Team;
}

class InventoryManagerHooks {
public:
    void install(csgo::InventoryManagerPOD* inventoryManager)
    {
        hookImpl.init(inventoryManager);
        originalUpdateInventoryEquippedState = reinterpret_cast<decltype(originalUpdateInventoryEquippedState)>(hookImpl.hookAt(WIN32_LINUX(29, 30), &updateInventoryEquippedState));
    }

    void uninstall()
    {
        hookImpl.restore();
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
