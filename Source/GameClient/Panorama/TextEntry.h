#pragma once

#include <CS2/Panorama/CTextEntry.h>
#include <MemoryPatterns/PatternTypes/TextEntryPatternTypes.h>

template <typename HookContext>
class TextEntry {
public:
    using RawType = cs2::CTextEntry;

    TextEntry(HookContext& hookContext, cs2::CTextEntry* textEntry) noexcept
        : hookContext{hookContext}
        , textEntry{textEntry}
    {
    }

    void setText(const char* text) noexcept
    {
        if (textEntry && setTextFunction())
            setTextFunction()(textEntry, text);
    }

private:
    [[nodiscard]] decltype(auto) setTextFunction() const noexcept
    {
        return hookContext.patternSearchResults().template get<TextEntrySetTextFunction>();
    }

    HookContext& hookContext;
    cs2::CTextEntry* textEntry;
};
