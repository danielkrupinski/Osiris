#pragma once

#include <Helpers/PatternFinder.h>
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
    return PostProcessingDisabler{ clientPatternFinder("\x83\xEC\x4C\x80\x3D").add(5).deref().as<bool*>() };
#elif IS_LINUX()
    return PostProcessingDisabler{ clientPatternFinder("\x80\x3D?????\x89\xB5").add(2).relativeToAbsolute().as<bool*>() };
#endif
}
