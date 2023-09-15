#pragma once

#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>
#include <Utils/RefCountedHook.h>
#include <Vmt/VmtLengthCalculator.h>

namespace csgo { struct ConVarPOD; }

class SvCheatsHooks : public RefCountedHook<SvCheatsHooks> {
public:
    explicit SvCheatsHooks(const VmtLengthCalculator& vmtLengthCalculator, csgo::ConVarPOD* svCheats)
        : hookImpl{ vmtLengthCalculator }, svCheats{ svCheats }
    {
    }

    [[nodiscard]] auto getOriginalSvCheatsGetInt() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalSvCheatsGetInt };
    }

    static int FASTCALL_CONV getInt(csgo::ConVarPOD* thisptr) noexcept;

private:
    void install()
    {
        hookImpl.install(*reinterpret_cast<std::uintptr_t**>(svCheats));
        originalSvCheatsGetInt = hookImpl.hook(WIN32_LINUX(13, 16), &getInt);
    }

    void uninstall()
    {
        hookImpl.uninstall(*reinterpret_cast<std::uintptr_t**>(svCheats));
    }

    [[nodiscard]] bool isInstalled() const noexcept
    {
        return hookImpl.isInstalled(*reinterpret_cast<std::uintptr_t**>(svCheats));
    }

    friend RefCountedHook;

    HookType hookImpl;
    csgo::ConVarPOD* svCheats;

    int (THISCALL_CONV* originalSvCheatsGetInt)(csgo::ConVarPOD* thisptr);
};
