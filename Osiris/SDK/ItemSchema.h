#pragma once

#include <cstddef>

template <typename Key, typename Value>
struct Node {
    int previousId;
    int nextId;
    void* _unknownPtr;
    int _unknown;
    Key key;
    Value value;
};

template <typename Key, typename Value>
struct Head {
    Node<Key, Value>* memory;
    int allocationCount;
    int growSize;
    int startElement;
    int nextAvailable;
    int _unknown;
    int lastElement;
};

struct String {
    char* buffer;
    int capacity;
    int growSize;
    int length;
};

struct PaintKit {
    int id;
    String name;
    String description;
    String itemName;
};

struct StickerKit {
    int id;
    std::byte pad[36];
    String itemName;
};

class ItemSchema {
    void* vmt;
    std::byte pad[0x28C];
public:
    Head<int, PaintKit*> paintKits;
private:
    std::byte pad1[0x8];
public:
    Head<int, StickerKit*> stickerKits;
};
