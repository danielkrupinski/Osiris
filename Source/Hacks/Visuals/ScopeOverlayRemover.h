#pragma once

#include <MemorySearch/PatternFinder.h>
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

[[nodiscard]] inline ScopeOverlayRemover createScopeOverlayRemover(const PatternFinder& clientPatternFinder)
{
#if IS_WIN32()
    return ScopeOverlayRemover{
        clientPatternFinder("FF 50 3C 8B 4C 24 20"_pat).add(3).asReturnAddress(),
        clientPatternFinder("8B 0D ? ? ? ? FF B7 ? ? ? ? 8B 01 FF 90 ? ? ? ? 8B 7C 24 1C"_pat).asReturnAddress(),
        clientPatternFinder("0F 11 05 ? ? ? ? F3 0F 7E 87"_pat).add(3).deref().add(4).as<float*>()
    };
#elif IS_LINUX()
    return ScopeOverlayRemover{
        clientPatternFinder("41 FF 51 70 43 8D 14 3E"_pat).add(4).asReturnAddress(),
        clientPatternFinder("49 8B 3C 24 8B B3 ? ? ? ? 48 8B 07 FF 90 ? ? ? ? 49 8B 3C 24 66 0F EF E4"_pat).asReturnAddress(),
        clientPatternFinder("F3 0F 10 05 ? ? ? ? FF 50 20 48 8B BB ? ? ? ? 48 85 FF 74 17"_pat).add(4).relativeToAbsolute().add(4).as<float*>()
    };
#endif
}
