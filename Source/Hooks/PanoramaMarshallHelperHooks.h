#pragma once

#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>
#include <Utils/RefCountedHook.h>
#include <Vmt/VmtLengthCalculator.h>

namespace csgo { struct PanoramaMarshallHelperPOD; }

class PanoramaMarshallHelperHooks : public RefCountedHook<PanoramaMarshallHelperHooks> {
public:
    explicit PanoramaMarshallHelperHooks(const VmtLengthCalculator& vmtLengthCalculator, csgo::PanoramaMarshallHelperPOD* panoramaMarshallHelper)
        : hookImpl{ vmtLengthCalculator }, panoramaMarshallHelper{ panoramaMarshallHelper }
    {
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
    void install()
    {
        hookImpl.install(*reinterpret_cast<std::uintptr_t**>(panoramaMarshallHelper));
        originalGetNumArgs = reinterpret_cast<decltype(originalGetNumArgs)>(hookImpl.hook(1, std::uintptr_t(&getNumArgs)));
        originalGetArgAsNumber = reinterpret_cast<decltype(originalGetArgAsNumber)>(hookImpl.hook(5, std::uintptr_t(&getArgAsNumber)));
        originalGetArgAsString = reinterpret_cast<decltype(originalGetArgAsString)>(hookImpl.hook(7, std::uintptr_t(&getArgAsString)));
        originalSetResultInt = reinterpret_cast<decltype(originalSetResultInt)>(hookImpl.hook(WIN32_LINUX(14, 11), std::uintptr_t(&setResultInt)));
    }

    void uninstall()
    {
        hookImpl.uninstall(*reinterpret_cast<std::uintptr_t**>(panoramaMarshallHelper));
    }

    [[nodiscard]] bool isInstalled() const noexcept
    {
        return hookImpl.isInstalled(*reinterpret_cast<std::uintptr_t**>(panoramaMarshallHelper));
    }

    friend RefCountedHook;

    HookType hookImpl;
    csgo::PanoramaMarshallHelperPOD* panoramaMarshallHelper;

    unsigned (THISCALL_CONV* originalGetNumArgs)(csgo::PanoramaMarshallHelperPOD* thisptr, void* params);
    double (THISCALL_CONV* originalGetArgAsNumber)(csgo::PanoramaMarshallHelperPOD* thisptr, void* params, int index);
    const char* (THISCALL_CONV* originalGetArgAsString)(csgo::PanoramaMarshallHelperPOD* thisptr, void* params, int index);
    void (THISCALL_CONV* originalSetResultInt)(csgo::PanoramaMarshallHelperPOD* thisptr, void* params, int result);
};
