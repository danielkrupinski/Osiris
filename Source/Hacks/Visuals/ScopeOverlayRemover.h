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
