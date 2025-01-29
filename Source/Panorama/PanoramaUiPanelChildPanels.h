#pragma once

#include <cstddef>
#include <utility>

#include <CS2/Classes/CUtlVector.h>
#include <CS2/Panorama/CUIPanel.h>

#include "PanoramaUiPanelIterator.h"

template <typename HookContext>
struct PanoramaUiPanelChildPanels {
    PanoramaUiPanelChildPanels(HookContext& hookContext, cs2::CUtlVector<cs2::CUIPanel*>* vector) noexcept
        : hookContext{hookContext}
        , vector{vector}
    {
    }

    [[nodiscard]] decltype(auto) begin() noexcept
    {
        if (vector && vector->memory)
            return hookContext.template make<PanoramaUiPanelIterator>(vector->memory);
        return hookContext.template make<PanoramaUiPanelIterator>(nullptr);
    }

    [[nodiscard]] decltype(auto) end() noexcept
    {
        if (vector && vector->memory)
            return hookContext.template make<PanoramaUiPanelIterator>(vector->memory + vector->size);
        return hookContext.template make<PanoramaUiPanelIterator>(nullptr);
    }

    [[nodiscard]] decltype(auto) operator[](std::size_t index) noexcept
    {
        if (vector && std::cmp_less(index, vector->size))
            return hookContext.template make<PanoramaUiPanel>(vector->memory[index]);
        return hookContext.template make<PanoramaUiPanel>(nullptr);
    }

    template <typename F>
    void forEach(F&& f) noexcept
    {
        if (!vector)
            return;

        for (int i = 0; i < vector->size; ++i)
            f(hookContext.template make<PanoramaUiPanel>(vector->memory[i]));
    }

    HookContext& hookContext;
    cs2::CUtlVector<cs2::CUIPanel*>* vector;
};
