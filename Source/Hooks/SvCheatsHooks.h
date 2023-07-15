#pragma once

#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>
#include <Vmt/VmtLengthCalculator.h>

namespace csgo { struct ConVarPOD; }

class SvCheatsHooks {
public:
    explicit SvCheatsHooks(const VmtLengthCalculator& vmtLengthCalculator)
        : hookImpl{ vmtLengthCalculator }
    {
    }

    void install(csgo::ConVarPOD* svCheats)
    {
        hookImpl.install(*reinterpret_cast<std::uintptr_t**>(svCheats));
        originalSvCheatsGetInt = reinterpret_cast<decltype(originalSvCheatsGetInt)>(hookImpl.hook(WIN32_LINUX(13, 16), std::uintptr_t(&getInt)));
    }

    void uninstall(csgo::ConVarPOD* svCheats)
    {
        hookImpl.uninstall(*reinterpret_cast<std::uintptr_t**>(svCheats));
    }

    [[nodiscard]] auto getOriginalSvCheatsGetInt() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalSvCheatsGetInt };
    }

    static int FASTCALL_CONV getInt(csgo::ConVarPOD* thisptr) noexcept;

private:
    HookType hookImpl;

    int (THISCALL_CONV* originalSvCheatsGetInt)(csgo::ConVarPOD* thisptr);
};
