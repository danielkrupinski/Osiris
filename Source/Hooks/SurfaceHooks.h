#pragma once

#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/IsPlatform.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>
#include <Vmt/VmtLengthCalculator.h>

namespace csgo { struct SurfacePOD; }

class SurfaceHooks {
public:
    explicit SurfaceHooks(const VmtLengthCalculator& vmtLengthCalculator)
        : hookImpl{ vmtLengthCalculator }
    {
    }

    void install(csgo::SurfacePOD* surface)
    {
        hookImpl.install(*reinterpret_cast<std::uintptr_t**>(surface));
        originalSetDrawColor = reinterpret_cast<decltype(originalSetDrawColor)>(hookImpl.hook(WIN32_LINUX(15, 14), std::uintptr_t(&setDrawColor)));
#if IS_WIN32()
        originalLockCursor = reinterpret_cast<decltype(originalLockCursor)>(hookImpl.hook(67, std::uintptr_t(&lockCursor)));
#endif
    }

    void uninstall(csgo::SurfacePOD* surface)
    {
        hookImpl.uninstall(*reinterpret_cast<std::uintptr_t**>(surface));
    }

    [[nodiscard]] auto getOriginalSetDrawColor() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalSetDrawColor };
    }

#if IS_WIN32()
    [[nodiscard]] auto getOriginalLockCursor() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalLockCursor };
    }
#endif

    static void FASTCALL_CONV setDrawColor(FASTCALL_THIS(csgo::SurfacePOD* thisptr), int r, int g, int b, int a) noexcept;
#if IS_WIN32()
    static void FASTCALL_CONV lockCursor(FASTCALL_THIS(csgo::SurfacePOD* thisptr)) noexcept;
#endif

private:
    HookType hookImpl;

    void (THISCALL_CONV* originalSetDrawColor)(csgo::SurfacePOD* thisptr, int r, int g, int b, int a);
#if IS_WIN32()
    void (THISCALL_CONV* originalLockCursor)(csgo::SurfacePOD* thisptr);
#endif
};
