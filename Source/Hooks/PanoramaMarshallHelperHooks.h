#pragma once

#include <Endpoints.h>
#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>

namespace csgo { struct PanoramaMarshallHelperPOD; }

class PanoramaMarshallHelperHooks {
public:
    void install(csgo::PanoramaMarshallHelperPOD* panoramaMarshallHelper)
    {
        hookImpl.init(panoramaMarshallHelper);
        originalGetNumArgs = reinterpret_cast<decltype(originalGetNumArgs)>(hookImpl.hookAt(1, &getNumArgs));
        originalGetArgAsNumber = reinterpret_cast<decltype(originalGetArgAsNumber)>(hookImpl.hookAt(5, &getArgAsNumber));
        originalGetArgAsString = reinterpret_cast<decltype(originalGetArgAsString)>(hookImpl.hookAt(7, &getArgAsString));
        originalSetResultInt = reinterpret_cast<decltype(originalSetResultInt)>(hookImpl.hookAt(WIN32_LINUX(14, 11), &setResultInt));
    }

    void uninstall()
    {
        hookImpl.restore();
    }

    [[nodiscard]] auto getOriginalGetNumArgs() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalGetNumArgs };
    }

    [[nodiscard]] auto getOriginalGetArgAsNumber() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalGetArgAsNumber };
    }

    [[nodiscard]] auto getOriginalGetArgAsString() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalGetArgAsString };
    }

    [[nodiscard]] auto getOriginalSetResultInt() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalSetResultInt };
    }

    static unsigned FASTCALL_CONV getNumArgs(FASTCALL_THIS(csgo::PanoramaMarshallHelperPOD* thisptr), void* params) noexcept;
    static double FASTCALL_CONV getArgAsNumber(FASTCALL_THIS(csgo::PanoramaMarshallHelperPOD* thisptr), void* params, int index) noexcept;
    static const char* FASTCALL_CONV getArgAsString(FASTCALL_THIS(csgo::PanoramaMarshallHelperPOD* thisptr), void* params, int index) noexcept;
    static void FASTCALL_CONV setResultInt(FASTCALL_THIS(csgo::PanoramaMarshallHelperPOD* thisptr), void* params, int result) noexcept;

private:
    HookType hookImpl;

    unsigned (THISCALL_CONV* originalGetNumArgs)(csgo::PanoramaMarshallHelperPOD* thisptr, void* params);
    double (THISCALL_CONV* originalGetArgAsNumber)(csgo::PanoramaMarshallHelperPOD* thisptr, void* params, int index);
    const char* (THISCALL_CONV* originalGetArgAsString)(csgo::PanoramaMarshallHelperPOD* thisptr, void* params, int index);
    void (THISCALL_CONV* originalSetResultInt)(csgo::PanoramaMarshallHelperPOD* thisptr, void* params, int result);
};
