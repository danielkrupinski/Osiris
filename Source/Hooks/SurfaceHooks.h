#pragma once

#include <Endpoints.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/IsPlatform.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>

namespace csgo { struct SurfacePOD; }

void STDCALL_CONV setDrawColor(LINUX_ARGS(void* thisptr, ) int r, int g, int b, int a) noexcept;
#if IS_WIN32()
void STDCALL_CONV lockCursor() noexcept;
#endif

template <typename HookImpl>
class SurfaceHooks {
public:
    void install(csgo::SurfacePOD* surface)
    {
        hookImpl.init(surface);
        originalSetDrawColor = reinterpret_cast<decltype(originalSetDrawColor)>(hookImpl.hookAt(WIN32_LINUX(15, 14), &setDrawColor));
#if IS_WIN32()
        originalLockCursor = reinterpret_cast<decltype(originalLockCursor)>(hookImpl.hookAt(67, &lockCursor));
#endif
    }

    void uninstall()
    {
        hookImpl.restore();
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

private:
    HookImpl hookImpl;

    void (THISCALL_CONV* originalSetDrawColor)(csgo::SurfacePOD* thisptr, int r, int g, int b, int a);
#if IS_WIN32()
    void (THISCALL_CONV* originalLockCursor)(csgo::SurfacePOD* thisptr);
#endif
};
