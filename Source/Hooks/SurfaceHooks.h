#pragma once

#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/IsPlatform.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>
#include <Utils/RefCountedHook.h>
#include <Vmt/VmtLengthCalculator.h>

namespace csgo { struct SurfacePOD; }

class SurfaceHooks : public RefCountedHook<SurfaceHooks> {
public:
    explicit SurfaceHooks(const VmtLengthCalculator& vmtLengthCalculator, csgo::SurfacePOD* surface)
        : hookImpl{ vmtLengthCalculator }, surface{ surface }
    {
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
    void install()
    {
        hookImpl.install(*reinterpret_cast<std::uintptr_t**>(surface));
        originalSetDrawColor = hookImpl.hook(WIN32_LINUX(15, 14), &setDrawColor);
#if IS_WIN32()
        originalLockCursor = hookImpl.hook(67, &lockCursor);
#endif
    }

    void uninstall()
    {
        hookImpl.uninstall(*reinterpret_cast<std::uintptr_t**>(surface));
    }

    [[nodiscard]] bool isInstalled() const noexcept
    {
        return hookImpl.isInstalled(*reinterpret_cast<std::uintptr_t**>(surface));
    }

    friend RefCountedHook;

    HookType hookImpl;
    csgo::SurfacePOD* surface;

    void (THISCALL_CONV* originalSetDrawColor)(csgo::SurfacePOD* thisptr, int r, int g, int b, int a);
#if IS_WIN32()
    void (THISCALL_CONV* originalLockCursor)(csgo::SurfacePOD* thisptr);
#endif
};
