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
