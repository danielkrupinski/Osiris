#pragma once

#include <cstring>

#include <CS2/Panorama/CPanelStyle.h>
#include <CS2/Panorama/CStyleSymbol.h>
#include <Platform/Macros/FunctionAttributes.h>

struct StylePropertySymbolMap {
    [[nodiscard]] [[NOINLINE]] cs2::CStyleSymbol findSymbol(const char* stylePropertyName) const noexcept
    {
        if (!symbols)
            return {};

        for (int i = 0; i < symbols->numElements; ++i) {
            if (std::strcmp(symbols->memory[i].key.m_pString, stylePropertyName) == 0)
                return symbols->memory[i].value;
        }
        return {};
    }

    const cs2::CPanelStyle::StylePropertySymbols* symbols;
};
