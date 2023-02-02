#pragma once

#include <CSGO/Constants/ItemId.h>
#include <Endpoints.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>

namespace csgo
{
    struct InventoryManagerPOD;
    enum class Team;
}

void STDCALL_CONV updateInventoryEquippedState(LINUX_ARGS(void* thisptr, ) std::uintptr_t inventory, csgo::ItemId itemID, csgo::Team team, int slot, bool swap) noexcept;

template <typename HookImpl>
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

private:
    HookImpl hookImpl;

    void (THISCALL_CONV* originalUpdateInventoryEquippedState)(csgo::InventoryManagerPOD* thisptr, std::uintptr_t inventory, csgo::ItemId itemID, csgo::Team team, int slot, bool swap);
};
