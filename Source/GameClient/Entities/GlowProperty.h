#pragma once

#include <CS2/Classes/CGlowProperty.h>
#include <MemoryPatterns/PatternTypes/GlowPropertyPatternTypes.h>

template <typename HookContext>
class GlowProperty {
public:
    GlowProperty(HookContext& hookContext, cs2::CGlowProperty* glowProperty) noexcept
        : hookContext{hookContext}
        , glowProperty{glowProperty}
    {
    }

    [[nodiscard]] decltype(auto) isGlowing() const noexcept
    {
        return hookContext.clientPatternSearchResults().template get<OffsetToIsGlowing>().of(glowProperty).toOptional();
    }

private:
    HookContext& hookContext;
    cs2::CGlowProperty* glowProperty;
};
