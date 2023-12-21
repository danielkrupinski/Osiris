#pragma once

#include <CS2/Classes/Panorama.h>
#include <GameClasses/MemAlloc.h>
#include "StylePropertiesSymbols.h"
#include "StylePropertiesVMTs.h"

struct PanelStylePropertyFactory {
    PanelStylePropertyFactory(const StylePropertiesVMTs& stylePropertyVMTs, const StylePropertiesSymbols& stylePropertySymbols) noexcept
        : VMTs{stylePropertyVMTs}
        , symbols{stylePropertySymbols}
    {
    }

    [[nodiscard]] cs2::CStylePropertyWidth* width(cs2::CUILength width) noexcept
    {
        return create<cs2::CStylePropertyWidth>(VMTs.widthPropertyVmt, symbols.width, width);
    }


    [[nodiscard]] cs2::CStylePropertyOpacity* opacity(float opacity) const noexcept
    {
        return create<cs2::CStylePropertyOpacity>(VMTs.opacityPropertyVmt, symbols.opacity, opacity);
    }

private:
    template <typename T, typename... Args>
    [[nodiscard]] T* create(const void* vmt, cs2::CStyleSymbol symbol, Args&&... args) const noexcept
    {
        if (vmt && symbol.isValid()) {
            if (const auto memory{MemAlloc::allocate(sizeof(T))})
                return new (memory) T{cs2::CStyleProperty{vmt, symbol, false}, std::forward<Args>(args)...};
        }
        return nullptr;
    }

    const StylePropertiesVMTs& VMTs;
    const StylePropertiesSymbols& symbols;
};
