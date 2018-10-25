#pragma once

#include "UtlMemory.h"

template <typename T, typename A = UtlMemory<T>>
class UtlVector {
public:
    typedef A Allocator;

    T& operator[](int i) {
        return memory[i];
    };

    Allocator memory;
    int size;
    T* elements;
};
