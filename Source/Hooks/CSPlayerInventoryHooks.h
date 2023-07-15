#pragma once

#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>
#include <Vmt/VmtLengthCalculator.h>

namespace csgo
{
    struct CSPlayerInventoryPOD;
    struct SharedObjectPOD;
    struct SOID;
}

class CSPlayerInventoryHooks {
public:
    explicit CSPlayerInventoryHooks(const VmtLengthCalculator& vmtLengthCalculator)
        : hookImpl{ vmtLengthCalculator }
    {
    }

    void install(csgo::CSPlayerInventoryPOD* inventory)
    {
        hookImpl.install(*reinterpret_cast<std::uintptr_t**>(inventory));
        originalSoUpdated = reinterpret_cast<decltype(originalSoUpdated)>(hookImpl.hook(1, std::uintptr_t(&soUpdated)));
    }

    void uninstall(csgo::CSPlayerInventoryPOD* inventory)
    {
        hookImpl.uninstall(*reinterpret_cast<std::uintptr_t**>(inventory));
    }

    [[nodiscard]] auto getOriginalSoUpdated() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalSoUpdated };
    }

    static void FASTCALL_CONV soUpdated(FASTCALL_THIS(csgo::CSPlayerInventoryPOD* thisptr), csgo::SOID owner, csgo::SharedObjectPOD* object, int event) noexcept;

private:
    HookType hookImpl;

    void (THISCALL_CONV* originalSoUpdated)(csgo::CSPlayerInventoryPOD* thisptr, csgo::SOID owner, csgo::SharedObjectPOD* object, int event);
};
