#pragma once

#include <Helpers/PatternFinder.h>
#include <MemorySearch/BytePatternLiteral.h>
#include <Platform/Macros/IsPlatform.h>
#include <Utils/ReturnAddress.h>

class ScopeOverlayRemover {
public:
    bool enabled;

    ScopeOverlayRemover(ReturnAddress scopeDust, ReturnAddress scopeArc, float* vignette)
        : scopeDust{ scopeDust }, scopeArc{ scopeArc }, vignette{ vignette }
    {
    }

    void setDrawColorHook(ReturnAddress hookReturnAddress, int& alpha) const noexcept
    {
        if (enabled && (hookReturnAddress == scopeDust || hookReturnAddress == scopeArc))
            alpha = 0;
    }

    void updateColorCorrectionWeightsHook() const noexcept
    {
        if (enabled && vignette)
            *vignette = 0.0f;
    }

    template <typename Configurator>
    void configure(Configurator& configurator)
    {
        configurator("Enabled", enabled).def(false);
    }

private:
    ReturnAddress scopeDust;
    ReturnAddress scopeArc;
    float* vignette;
};

[[nodiscard]] inline ScopeOverlayRemover createScopeOverlayRemover(const helpers::PatternFinder& clientPatternFinder)
{
#if IS_WIN32()
    return ScopeOverlayRemover{
        clientPatternFinder("FF 50 3C 8B 4C 24 20"_pat).add(3).asReturnAddress(),
        clientPatternFinder("8B 0D ? ? ? ? FF B7 ? ? ? ? 8B 01 FF 90 ? ? ? ? 8B 7C 24 1C"_pat).asReturnAddress(),
        clientPatternFinder("0F 11 05 ? ? ? ? F3 0F 7E 87"_pat).add(3).deref().add(4).as<float*>()
    };
#elif IS_LINUX()
    return ScopeOverlayRemover{
        clientPatternFinder("8B 85 ? ? ? ? 43 8D 14 2E"_pat).asReturnAddress(),
        clientPatternFinder("49 8B 3C 24 8B B3 ? ? ? ? 48 8B 07 FF 90 ? ? ? ? 49 8B 3C 24 4C 89 EA"_pat).asReturnAddress(),
        clientPatternFinder("48 8B 07 0F 2F 05"_pat).add(6).relativeToAbsolute().as<float*>()
    };
#endif
}
