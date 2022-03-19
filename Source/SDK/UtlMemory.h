#pragma once

template <typename T>
struct UtlMemory {
    T& operator[](int i) const noexcept { return memory[i]; }

    T* memory;
    int allocationCount;
    int growSize;
};
