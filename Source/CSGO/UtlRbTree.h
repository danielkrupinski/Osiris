#pragma once

#include "UtlMemory.h"

namespace csgo
{

template <typename ElementType, typename IndexType>
struct UtlRbTreeNode {
    IndexType left;
    IndexType right;
    IndexType parent;
    IndexType type;
    ElementType element;
};

template <typename ElementType, typename IndexType>
struct UtlRbTree {
    bool isIndexUsed(IndexType index) const
    {
        return memory[index].left != index;
    }

    void* lessFunc;
    UtlMemory<UtlRbTreeNode<ElementType, IndexType>> memory;
    IndexType root;
    IndexType numElements;
    IndexType firstFree;
    IndexType lastAlloc;
    UtlRbTreeNode<ElementType, IndexType>* elements;
};

}
