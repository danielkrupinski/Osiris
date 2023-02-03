#pragma once

#include <Endpoints.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>

namespace csgo
{
    struct CSPlayerInventoryPOD;
    struct SharedObjectPOD;
    struct SOID;
}

void FASTCALL_CONV soUpdated(FASTCALL_THIS(csgo::CSPlayerInventoryPOD* thisptr), csgo::SOID owner, csgo::SharedObjectPOD* object, int event) noexcept;

template <typename HookImpl>
class CSPlayerInventoryHooks {
public:
    void install(csgo::CSPlayerInventoryPOD* inventory)
    {
        hookImpl.init(inventory);
        originalSoUpdated = reinterpret_cast<decltype(originalSoUpdated)>(hookImpl.hookAt(1, &soUpdated));
    }

    void uninstall()
    {
        hookImpl.restore();
    }

    [[nodiscard]] auto getOriginalSoUpdated() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalSoUpdated };
    }

private:
    HookImpl hookImpl;

    void (THISCALL_CONV* originalSoUpdated)(csgo::CSPlayerInventoryPOD* thisptr, csgo::SOID owner, csgo::SharedObjectPOD* object, int event);
};
