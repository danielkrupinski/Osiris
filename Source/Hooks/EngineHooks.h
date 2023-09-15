#pragma once

#include <HookType.h>
#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <Utils/RefCountedHook.h>
#include <Vmt/VmtLengthCalculator.h>

namespace csgo { struct DemoPlaybackParameters; }
namespace csgo { struct EnginePOD; }

class EngineHooks : public RefCountedHook<EngineHooks> {
public:
    explicit EngineHooks(const VmtLengthCalculator& vmtLengthCalculator, csgo::EnginePOD* engine)
        : hookImpl{ vmtLengthCalculator }, engine{ engine }
    {
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

    static bool FASTCALL_CONV isPlayingDemo(FASTCALL_THIS(csgo::EnginePOD* thisptr)) noexcept;
    static float FASTCALL_CONV getScreenAspectRatio(FASTCALL_THIS(csgo::EnginePOD* thisptr), int width, int height) noexcept;
    static const csgo::DemoPlaybackParameters* FASTCALL_CONV getDemoPlaybackParameters(FASTCALL_THIS(csgo::EnginePOD* thisptr)) noexcept;

private:    
    void install()
    {
        hookImpl.install(*reinterpret_cast<std::uintptr_t**>(engine));

        originalIsPlayingDemo = hookImpl.hook(82, &isPlayingDemo);
        originalGetScreenAspectRatio = hookImpl.hook(101, &getScreenAspectRatio);
        originalGetDemoPlaybackParameters = hookImpl.hook(WIN32_LINUX(218, 219), &getDemoPlaybackParameters);
    }

    void uninstall()
    {
        hookImpl.uninstall(*reinterpret_cast<std::uintptr_t**>(engine));
    }

    [[nodiscard]] bool isInstalled() const noexcept
    {
        return hookImpl.isInstalled(*reinterpret_cast<std::uintptr_t**>(engine));
    }

    friend RefCountedHook;

    HookType hookImpl;
    csgo::EnginePOD* engine;

    bool (THISCALL_CONV* originalIsPlayingDemo)(csgo::EnginePOD* thisptr);
    float (THISCALL_CONV* originalGetScreenAspectRatio)(csgo::EnginePOD* thisptr, int width, int height);
    const csgo::DemoPlaybackParameters* (THISCALL_CONV* originalGetDemoPlaybackParameters)(csgo::EnginePOD* thisptr);
};
