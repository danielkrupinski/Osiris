#pragma once

#include <Endpoints.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <RetSpoof/FunctionInvoker.h>

namespace csgo { struct ClientMode; }
namespace csgo { struct ViewSetup; }

template <typename HookImpl>
class ClientModeHooks {
public:
    void install(csgo::ClientMode* clientMode)
    {
        hookImpl.init(clientMode);
        originalShouldDrawFog = reinterpret_cast<decltype(originalShouldDrawFog)>(hookImpl.hookAt(WIN32_LINUX(17, 18), &shouldDrawFog));
        originalOverrideView = reinterpret_cast<decltype(originalOverrideView)>(hookImpl.hookAt(WIN32_LINUX(18, 19), &overrideView));
        originalCreateMove = reinterpret_cast<decltype(originalCreateMove)>(hookImpl.hookAt(WIN32_LINUX(24, 25), &createMove));
        originalShouldDrawViewModel = reinterpret_cast<decltype(originalShouldDrawViewModel)>(hookImpl.hookAt(WIN32_LINUX(27, 28), &shouldDrawViewModel));
        originalGetViewModelFov = reinterpret_cast<decltype(originalGetViewModelFov)>(hookImpl.hookAt(WIN32_LINUX(35, 36), &getViewModelFov));
        originalDoPostScreenEffects = reinterpret_cast<decltype(originalDoPostScreenEffects)>(hookImpl.hookAt(WIN32_LINUX(44, 45), &doPostScreenEffects));
        originalUpdateColorCorrectionWeights = reinterpret_cast<decltype(originalUpdateColorCorrectionWeights)>(hookImpl.hookAt(WIN32_LINUX(58, 61), &updateColorCorrectionWeights));
    }

    void uninstall()
    {
        hookImpl.restore();
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

private:
    HookImpl hookImpl;

    bool (THISCALL_CONV* originalShouldDrawFog)(csgo::ClientMode* thisptr);
    void (THISCALL_CONV* originalOverrideView)(csgo::ClientMode* thisptr, csgo::ViewSetup* setup);
    bool (THISCALL_CONV* originalCreateMove)(csgo::ClientMode* thisptr, float inputSampleTime, csgo::UserCmd* cmd);
    bool (THISCALL_CONV* originalShouldDrawViewModel)(csgo::ClientMode* thisptr);
    float (THISCALL_CONV* originalGetViewModelFov)(csgo::ClientMode* thisptr);
    void (THISCALL_CONV* originalDoPostScreenEffects)(csgo::ClientMode* thisptr, void* param);
    void (THISCALL_CONV* originalUpdateColorCorrectionWeights)(csgo::ClientMode* thisptr);
};
