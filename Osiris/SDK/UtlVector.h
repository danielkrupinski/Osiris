#pragma once

#include "UtlMemory.h"

template <typename T, typename A = UtlMemory<T>>
class UtlVector {
public:
    typedef A Allocator;

    T& operator[](int i) {
        return m_Memory[i];
    };

    Allocator m_Memory;
    int m_Size;
    T* m_pElements;
};
