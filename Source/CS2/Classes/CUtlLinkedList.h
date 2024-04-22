#pragma once

#include <cstdint>

namespace cs2
{

template <typename ElementType, typename IndexType = std::uint16_t>
struct CUtlLinkedList {
    struct UtlLinkedListElem_t {
        ElementType m_Element;
        IndexType m_Previous;
        IndexType m_Next;
    };

    static constexpr auto kInvalidIndex{static_cast<IndexType>(-1)};

    UtlLinkedListElem_t* memory;
    int allocationCount;
    int growSize;
    IndexType m_Head;
};

}
