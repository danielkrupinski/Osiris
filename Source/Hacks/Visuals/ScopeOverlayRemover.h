#pragma once

#include <Helpers/PatternFinder.h>
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
    return ScopeOverlayRemover{ ReturnAddress{ clientPatternFinder("\xFF\x50\x3C\x8B\x4C\x24\x20").add(3).get() }, ReturnAddress{ clientPatternFinder("\x8B\x0D????\xFF\xB7????\x8B\x01\xFF\x90????\x8B\x7C\x24\x1C").get() }, clientPatternFinder("\x0F\x11\x05????\xF3\x0F\x7E\x87").add(3).deref().add(4).as<float*>() };
#elif IS_LINUX()
    return ScopeOverlayRemover{ ReturnAddress{ clientPatternFinder("\x8B\x85????\x43\x8D\x14\x2E").get() }, ReturnAddress{ clientPatternFinder("\x49\x8B\x3C\x24\x8B\xB3????\x48\x8B\x07\xFF\x90????\x49\x8B\x3C\x24\x4C\x89\xEA").get() }, clientPatternFinder("\x48\x8B\x07\x0F\x2F\x05").add(6).relativeToAbsolute().as<float*>() };
#endif
}
