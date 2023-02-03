#pragma once

#include <Endpoints.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>

namespace csgo { struct DemoPlaybackParameters; }
namespace csgo { struct EnginePOD; }

bool FASTCALL_CONV isPlayingDemo(FASTCALL_THIS(csgo::EnginePOD* thisptr)) noexcept;
float FASTCALL_CONV getScreenAspectRatio(FASTCALL_THIS(csgo::EnginePOD* thisptr), int width, int height) noexcept;
const csgo::DemoPlaybackParameters* FASTCALL_CONV getDemoPlaybackParameters(FASTCALL_THIS(csgo::EnginePOD* thisptr)) noexcept;

template <typename HookImpl>
class EngineHooks {
public:
    void install(csgo::EnginePOD* engine)
    {
        hookImpl.init(engine);

        originalIsPlayingDemo = reinterpret_cast<decltype(originalIsPlayingDemo)>(hookImpl.hookAt(82, &isPlayingDemo));
        originalGetScreenAspectRatio = reinterpret_cast<decltype(originalGetScreenAspectRatio)>(hookImpl.hookAt(101, &getScreenAspectRatio));
        originalGetDemoPlaybackParameters = reinterpret_cast<decltype(originalGetDemoPlaybackParameters)>(hookImpl.hookAt(WIN32_LINUX(218, 219), &getDemoPlaybackParameters));
    }

    void uninstall()
    {
        hookImpl.restore();
    }

    [[nodiscard]] auto getOriginalIsPlayingDemo() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalIsPlayingDemo };
    }

    [[nodiscard]] auto getOriginalGetScreenAspectRatio() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalGetScreenAspectRatio };
    }

    [[nodiscard]] auto getOriginalGetDemoPlaybackParameters() const
    {
        return FunctionInvoker{ retSpoofGadgets->client, originalGetDemoPlaybackParameters };
    }

private:
    HookImpl hookImpl;

    bool (THISCALL_CONV* originalIsPlayingDemo)(csgo::EnginePOD* thisptr);
    float (THISCALL_CONV* originalGetScreenAspectRatio)(csgo::EnginePOD* thisptr, int width, int height);
    const csgo::DemoPlaybackParameters* (THISCALL_CONV* originalGetDemoPlaybackParameters)(csgo::EnginePOD* thisptr);
};
