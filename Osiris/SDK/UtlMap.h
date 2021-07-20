#pragma once

#include "UtlMemory.h"

template <typename Key, typename Value>
struct Node {
    int left;
    int right;
    int parent;
    int type;
    Key key;
    Value value;
};

template <typename Key, typename Value>
struct UtlMap {
    auto begin() const noexcept { return memory.memory; }
    auto end() const noexcept { return memory.memory + numElements; }

    int find(Key key) const noexcept
    {
        auto curr = root;

        while (curr != -1) {
            const auto el = memory[curr];

            if (el.key < key)
                curr = el.right;
            else if (el.key > key)
                curr = el.left;
            else
                break;
        }
        return curr;
    }

    void* lessFunc;
    UtlMemory<Node<Key, Value>> memory;
    int root;
    int numElements;
    int firstFree;
    int lastAlloc;
    Node<Key, Value>* elements;
};
