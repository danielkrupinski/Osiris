#pragma once

#include <CS2/Classes/CUtlVector.h>

template <typename T>
struct UtlVector {
    explicit UtlVector(cs2::CUtlVector<T>* vector) noexcept
        : vector{vector}
    {
    }

    template <typename F>
    void forEach(F&& f) noexcept
    {
        if (!vector)
            return;

        for (int i = 0; i < vector->size; ++i)
            f(vector->memory[i]);
    }

    cs2::CUtlVector<T>* vector;
};
    