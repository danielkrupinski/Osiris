#pragma once

#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>
#include <Utils/RefCountedHook.h>
#include <Vmt/VmtLengthCalculator.h>

namespace csgo
{
    struct ClientMode;
    struct UserCmd;
    struct ViewSetup;
}

class ClientModeHooks : public RefCountedHook<ClientModeHooks> {
public:
    explicit ClientModeHooks(const VmtLengthCalculator& vmtLengthCalculator, csgo::ClientMode* clientMode)
        : hookImpl{ vmtLengthCalculator }, clientMode{ clientMode }
    {
    }

    [[nodiscard]] auto getOriginalShouldDrawFog() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalShouldDrawFog };
    }

    [[nodiscard]] auto getOriginalOverrideView() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalOverrideView };
    }

    [[nodiscard]] auto getOriginalCreateMove() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalCreateMove };
    }

    [[nodiscard]] auto getOriginalShouldDrawViewModel() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalShouldDrawViewModel };
    }

    [[nodiscard]] auto getOriginalGetViewModelFov() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalGetViewModelFov };
    }

    [[nodiscard]] auto getOriginalDoPostScreenEffects() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalDoPostScreenEffects };
    }

    [[nodiscard]] auto getOriginalUpdateColorCorrectionWeights() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalUpdateColorCorrectionWeights };
    }
    
    static bool FASTCALL_CONV shouldDrawFog(FASTCALL_THIS(csgo::ClientMode* thisptr)) noexcept;
    static void FASTCALL_CONV overrideView(FASTCALL_THIS(csgo::ClientMode* thisptr), csgo::ViewSetup* setup) noexcept;
    static bool FASTCALL_CONV createMove(FASTCALL_THIS(csgo::ClientMode* thisptr), float inputSampleTime, csgo::UserCmd* cmd) noexcept;
    static bool FASTCALL_CONV shouldDrawViewModel(FASTCALL_THIS(csgo::ClientMode* thisptr)) noexcept;
    static float FASTCALL_CONV getViewModelFov(FASTCALL_THIS(csgo::ClientMode* thisptr)) noexcept;
    static void FASTCALL_CONV doPostScreenEffects(FASTCALL_THIS(csgo::ClientMode* thisptr), void* param) noexcept;
    static void FASTCALL_CONV updateColorCorrectionWeights(FASTCALL_THIS(csgo::ClientMode* thisptr)) noexcept;

private:
    void install() noexcept
    {
        hookImpl.install(*reinterpret_cast<std::uintptr_t**>(clientMode));
        originalShouldDrawFog = reinterpret_cast<decltype(originalShouldDrawFog)>(hookImpl.hook(WIN32_LINUX(17, 18), std::uintptr_t(&shouldDrawFog)));
        originalOverrideView = reinterpret_cast<decltype(originalOverrideView)>(hookImpl.hook(WIN32_LINUX(18, 19), std::uintptr_t(&overrideView)));
        originalCreateMove = reinterpret_cast<decltype(originalCreateMove)>(hookImpl.hook(WIN32_LINUX(24, 25), std::uintptr_t(&createMove)));
        originalShouldDrawViewModel = reinterpret_cast<decltype(originalShouldDrawViewModel)>(hookImpl.hook(WIN32_LINUX(27, 28), std::uintptr_t(&shouldDrawViewModel)));
        originalGetViewModelFov = reinterpret_cast<decltype(originalGetViewModelFov)>(hookImpl.hook(WIN32_LINUX(35, 36), std::uintptr_t(&getViewModelFov)));
        originalDoPostScreenEffects = reinterpret_cast<decltype(originalDoPostScreenEffects)>(hookImpl.hook(WIN32_LINUX(44, 45), std::uintptr_t(&doPostScreenEffects)));
        originalUpdateColorCorrectionWeights = reinterpret_cast<decltype(originalUpdateColorCorrectionWeights)>(hookImpl.hook(WIN32_LINUX(58, 61), std::uintptr_t(&updateColorCorrectionWeights)));
    }

    void uninstall() noexcept
    {
        hookImpl.uninstall(*reinterpret_cast<std::uintptr_t**>(clientMode));
    }

    [[nodiscard]] bool isInstalled() const noexcept
    {
        return hookImpl.isInstalled(*reinterpret_cast<std::uintptr_t**>(clientMode));
    }

    friend RefCountedHook;

    HookType hookImpl;
    csgo::ClientMode* clientMode;

    bool (THISCALL_CONV* originalShouldDrawFog)(csgo::ClientMode* thisptr);
    void (THISCALL_CONV* originalOverrideView)(csgo::ClientMode* thisptr, csgo::ViewSetup* setup);
    bool (THISCALL_CONV* originalCreateMove)(csgo::ClientMode* thisptr, float inputSampleTime, csgo::UserCmd* cmd);
    bool (THISCALL_CONV* originalShouldDrawViewModel)(csgo::ClientMode* thisptr);
    float (THISCALL_CONV* originalGetViewModelFov)(csgo::ClientMode* thisptr);
    void (THISCALL_CONV* originalDoPostScreenEffects)(csgo::ClientMode* thisptr, void* param);
    void (THISCALL_CONV* originalUpdateColorCorrectionWeights)(csgo::ClientMode* thisptr);
};
