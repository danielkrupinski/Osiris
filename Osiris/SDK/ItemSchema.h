#pragma once

#include <cstddef>

#include "Pad.h"
#include "VirtualMethod.h"

enum class WeaponId : short;

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

class EconItemDefintion {
public:
    VIRTUAL_METHOD(WeaponId, getWeaponId, 0, (), (this))
};

class ItemSchema {
public:
    PAD(0x28C)
    Head<int, PaintKit*> paintKits;
    PAD(0x8)
    Head<int, StickerKit*> stickerKits;

    VIRTUAL_METHOD(EconItemDefintion*, getItemDefinitionByName, 42, (const char* name), (this, name))
};

class ItemSystem {
public:
    VIRTUAL_METHOD(ItemSchema*, getItemSchema, 0, (), (this))
};
