#pragma once

#include <Helpers/PatternFinder.h>
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

[[nodiscard]] inline PostProcessingDisabler createPostProcessingDisabler(const helpers::PatternFinder& clientPatternFinder)
{
#if IS_WIN32()
    return PostProcessingDisabler{ clientPatternFinder("83 EC 4C 80 3D"_pat).add(5).deref().as<bool*>() };
#elif IS_LINUX()
    return PostProcessingDisabler{ clientPatternFinder("80 3D ? ? ? ? ? 89 B5"_pat).add(2).relativeToAbsolute().as<bool*>() };
#endif
}
