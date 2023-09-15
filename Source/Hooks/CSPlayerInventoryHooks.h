#pragma once

#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>
#include <Utils/RefCountedHook.h>
#include <Vmt/VmtLengthCalculator.h>

namespace csgo
{
    struct CSPlayerInventoryPOD;
    struct SharedObjectPOD;
    struct SOID;
}

class CSPlayerInventoryHooks : public RefCountedHook<CSPlayerInventoryHooks> {
public:
    explicit CSPlayerInventoryHooks(const VmtLengthCalculator& vmtLengthCalculator, csgo::CSPlayerInventoryPOD* inventory)
        : hookImpl{ vmtLengthCalculator }, inventory{ inventory }
    {
    }

    [[nodiscard]] auto getOriginalSoUpdated() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalSoUpdated };
    }

    static void FASTCALL_CONV soUpdated(FASTCALL_THIS(csgo::CSPlayerInventoryPOD* thisptr), csgo::SOID owner, csgo::SharedObjectPOD* object, int event) noexcept;

private:
    void install()
    {
        hookImpl.install(*reinterpret_cast<std::uintptr_t**>(inventory));
        originalSoUpdated = hookImpl.hook(1, &soUpdated);
    }

    void uninstall()
    {
        hookImpl.uninstall(*reinterpret_cast<std::uintptr_t**>(inventory));
    }

    [[nodiscard]] bool isInstalled() const noexcept
    {
        return hookImpl.isInstalled(*reinterpret_cast<std::uintptr_t**>(inventory));
    }

    friend RefCountedHook;

    HookType hookImpl;
    csgo::CSPlayerInventoryPOD* inventory;

    void (THISCALL_CONV* originalSoUpdated)(csgo::CSPlayerInventoryPOD* thisptr, csgo::SOID owner, csgo::SharedObjectPOD* object, int event);
};
