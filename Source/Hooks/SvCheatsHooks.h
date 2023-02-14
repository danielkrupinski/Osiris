#pragma once

#include <Endpoints.h>
#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>

namespace csgo { struct ConVarPOD; }

class SvCheatsHooks {
public:
    void install(csgo::ConVarPOD* svCheats)
    {
        hookImpl.init(svCheats);
        originalSvCheatsGetInt = reinterpret_cast<decltype(originalSvCheatsGetInt)>(hookImpl.hookAt(WIN32_LINUX(13, 16), &getInt));
    }

    void uninstall()
    {
        hookImpl.restore();
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
