#pragma once

#include <cstdint>
#include <CS2/Panorama/CPanelStyle.h>
#include <CS2/Panorama/CStyleSymbol.h>
#include <Platform/Macros/FunctionAttributes.h>
#include <Platform/Macros/IsPlatform.h>

#if IS_LINUX()
#include <cstring>
#endif

struct StylePropertySymbolMap {
#if IS_WIN64()
    StylePropertySymbolMap() = default;

    explicit StylePropertySymbolMap(cs2::CPanelStyle::FindStylePropertySymbol findStylePropertySymbol) noexcept
        : findStylePropertySymbol{findStylePropertySymbol}
    {
    }
#else
    StylePropertySymbolMap() = default;

    explicit StylePropertySymbolMap(const cs2::CPanelStyle::StylePropertySymbols* symbols) noexcept
        : symbols{symbols}
    {
    }
#endif

    [[nodiscard]] [[NOINLINE]] cs2::CStyleSymbol findSymbol(const char* stylePropertyName) const noexcept
    {
#if IS_WIN64()
        if (!findStylePropertySymbol || !stylePropertyName || !*stylePropertyName)
            return {};

        std::uint8_t symbolId{cs2::CStyleSymbol::kInvalidId};
        if (findStylePropertySymbol(&symbolId, stylePropertyName) != &symbolId || symbolId == cs2::CStyleSymbol::kInvalidId)
            return {};
        return cs2::CStyleSymbol{symbolId};
#else
        if (!symbols || !stylePropertyName)
            return {};
        for (std::int32_t i = 0; i < symbols->numElements; ++i) {
            if (std::strcmp(symbols->memory[i].key.m_pString, stylePropertyName) == 0)
                return symbols->memory[i].value;
        }
        return {};
#endif
    }

#if IS_WIN64()
    cs2::CPanelStyle::FindStylePropertySymbol findStylePropertySymbol{};
#else
    const cs2::CPanelStyle::StylePropertySymbols* symbols{};
#endif
};
