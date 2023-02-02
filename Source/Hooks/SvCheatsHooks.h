#pragma once

#include <Endpoints.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>

namespace csgo { struct ConVarPOD; }

int FASTCALL_CONV svCheatsGetInt(csgo::ConVarPOD* thisptr) noexcept;

template <typename HookImpl>
class SvCheatsHooks {
public:
    void install(csgo::ConVarPOD* svCheats)
    {
        hookImpl.init(svCheats);
        originalSvCheatsGetInt = reinterpret_cast<decltype(originalSvCheatsGetInt)>(hookImpl.hookAt(WIN32_LINUX(13, 16), &svCheatsGetInt));
    }

    void uninstall()
    {
        hookImpl.restore();
    }

    [[nodiscard]] auto getOriginalSvCheatsGetInt() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalSvCheatsGetInt };
    }

private:
    HookImpl hookImpl;

    int (THISCALL_CONV* originalSvCheatsGetInt)(csgo::ConVarPOD* thisptr);
};
