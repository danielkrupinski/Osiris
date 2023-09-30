#pragma once

namespace cs2
{

template <typename T>
struct CUtlVector {
    int size;
    T* memory;
    int allocationCount;
    int growSize;
};

}
