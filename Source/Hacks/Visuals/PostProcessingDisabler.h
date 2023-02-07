#pragma once

#include <MemorySearch/PatternFinder.h>
#include <MemorySearch/BytePatternLiteral.h>
#include <Platform/Macros/IsPlatform.h>
#include <CSGO/Constants/FrameStage.h>

class PostProcessingDisabler {
public:
    bool enabled;

    explicit PostProcessingDisabler(bool* disablePostProcessing)
        : disablePostProcessing{ disablePostProcessing }
    {
    }

    void run(csgo::FrameStage stage) noexcept
    {
        using enum csgo::FrameStage;
        if (stage != RENDER_START && stage != RENDER_END)
            return;

        if (disablePostProcessing)
            *disablePostProcessing = (stage == RENDER_START && enabled);
    }

    template <typename Configurator>
    void configure(Configurator& configurator)
    {
        configurator("Enabled", enabled).def(false);
    }

private:
    bool* disablePostProcessing;
};

[[nodiscard]] inline PostProcessingDisabler createPostProcessingDisabler(const PatternFinder& clientPatternFinder)
{
#if IS_WIN32()
    return PostProcessingDisabler{ clientPatternFinder("83 EC 4C 80 3D"_pat).add(5).deref().as<bool*>() };
#elif IS_LINUX()
    return PostProcessingDisabler{ clientPatternFinder("0F B6 05 ? ? ? ? 84 C0 0F 85 ? ? ? ? 85 D2"_pat).add(3).relativeToAbsolute().as<bool*>() };
#endif
}
