#pragma once

#include <cstdint>

namespace cs2
{

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
struct CUtlMap {
    std::uint8_t unknown;
    Node<Key, Value>* memory;
    int allocationCount;
    int growSize;
    int root;
    int numElements;
};

}
