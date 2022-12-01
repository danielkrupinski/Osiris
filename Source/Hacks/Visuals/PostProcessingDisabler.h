#pragma once

#include <SDK/Constants/FrameStage.h>

class PostProcessingDisabler {
public:
    bool enabled;

    explicit PostProcessingDisabler(bool* disablePostProcessing)
        : disablePostProcessing{ disablePostProcessing }
    {
    }

    void run(csgo::FrameStage stage) noexcept
    {
        if (stage != csgo::FrameStage::RENDER_START && stage != csgo::FrameStage::RENDER_END)
            return;

        *disablePostProcessing = (stage == csgo::FrameStage::RENDER_START && enabled);
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
    return PostProcessingDisabler{ reinterpret_cast<bool*>(clientPatternFinder("\x83\xEC\x4C\x80\x3D").add(5).deref().get()) };
#elif IS_LINUX()
    return PostProcessingDisabler{ reinterpret_cast<bool*>(clientPatternFinder("\x80\x3D?????\x89\xB5").add(2).relativeToAbsolute().get()) };
#endif
}
