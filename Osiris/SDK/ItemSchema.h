#pragma once

#include <cstddef>

#include "Pad.h"
#include "VirtualMethod.h"

enum class WeaponId : short;

template <typename T>
struct UtlMemory {
	T& operator[](int i) noexcept { return memory[i]; };

	T* memory;
	int allocationCount;
	int growSize;
};

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
	void* lessFunc;
	UtlMemory<Node<Key, Value>> memory;
    int root;
	int numElements;
    int firstFree;
    int lastAlloc;
	Node<Key, Value>* elements;
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
    VIRTUAL_METHOD(const char*, getItemBaseName, 2, (), (this))
};

class ItemSchema {
public:
    PAD(0x288)
    UtlMap<int, PaintKit*> paintKits;
    UtlMap<int, StickerKit*> stickerKits;

    VIRTUAL_METHOD(EconItemDefintion*, getItemDefinitionByName, 42, (const char* name), (this, name))
};

class ItemSystem {
public:
    VIRTUAL_METHOD(ItemSchema*, getItemSchema, 0, (), (this))
};
