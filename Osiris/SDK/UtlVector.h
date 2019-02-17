#pragma once

template <typename T>
class UtlVector {
public:
    T& operator[](int i)
    {
        return memory[i];
    };

    T* memory;
    int allocationCount;
    int growSize;
    int size;
    T* elements;
};
