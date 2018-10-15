#pragma once

template <typename T>
class UtlMemory {
public:
    T& operator[](int i)
    {
        return m_pMemory[i];
    };

    T* m_pMemory;
    int m_nAllocationCount;
    int m_nGrowSize;
};
