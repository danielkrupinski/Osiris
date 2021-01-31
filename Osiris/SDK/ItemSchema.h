#pragma once

#include <cstddef>

#include "Pad.h"
#include "UtlVector.h"
#include "VirtualMethod.h"

enum class WeaponId : short;

template <typename T>
struct UtlMemory {
    T& operator[](int i) const noexcept { return memory[i]; };

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

struct String {
    UtlMemory<char> buffer;
    int length;

    const char* data() const noexcept { return buffer.memory; }
};

struct PaintKit {
    int id;
    String name;
    String description;
    String itemName;
    String sameNameFamilyAggregate;
    String pattern;
    String normal;
    String logoMaterial;
    bool baseDiffuseOverride;
    int rarity;
};

struct StickerKit {
    int id;
    int rarity;
    String name;
    String description;
    String itemName;
    PAD(2 * sizeof(String))
    String inventoryImage;
};

class EconItemDefinition {
public:
    VIRTUAL_METHOD(WeaponId, getWeaponId, 0, (), (this))
    VIRTUAL_METHOD(const char*, getItemBaseName, 2, (), (this))
    VIRTUAL_METHOD(const char*, getItemTypeName, 3, (), (this))
    VIRTUAL_METHOD(const char*, getPlayerDisplayModel, 6, (), (this))
    VIRTUAL_METHOD(const char*, getWorldDisplayModel, 7, (), (this))
    VIRTUAL_METHOD(std::uint8_t, getRarity, 12, (), (this))

    int getCapabilities() noexcept
    {
        return *reinterpret_cast<int*>(this + WIN32_LINUX(0x148, 0x1F8));
    }

    bool isPaintable() noexcept
    {
        return getCapabilities() & 1; // ITEM_CAP_PAINTABLE
    }

    const char* getDefinitionName() noexcept
    {
        return *reinterpret_cast<const char**>(this + WIN32_LINUX(0x1BC, 0x2B0));
    }
};

struct ItemListEntry {
    int itemDef;
    int paintKit;
    PAD(20)

    auto weaponId() const noexcept
    {
        return static_cast<WeaponId>(itemDef);
    }
};

class EconLootListDefinition {
public:
    VIRTUAL_METHOD(const char*, getName, 0, (), (this))
    VIRTUAL_METHOD(const UtlVector<ItemListEntry>&, getLootListContents, 1, (), (this))
};

class EconItemSetDefinition {
public:
    VIRTUAL_METHOD(const char*, getLocKey, 1, (), (this))
    VIRTUAL_METHOD(int, getItemCount, 4, (), (this))
    VIRTUAL_METHOD(WeaponId, getItemDef, 5, (int index), (this, index))
    VIRTUAL_METHOD(int, getItemPaintKit, 6, (int index), (this, index))
};

struct EconItemQualityDefinition {
    int value;
    const char* name;
    unsigned weight;
    bool explicitMatchesOnly;
    bool canSupportSet;
    const char* hexColor;
};

struct AlternateIconData {
    String simpleName;
    String largeSimpleName;
    String iconURLSmall;
    String iconURLLarge;
};

class ItemSchema {
public:
    PAD(WIN32_LINUX(0x88, 0xB8))
    UtlMap<int, EconItemQualityDefinition> qualities;
    PAD(WIN32_LINUX(0x48, 0x60))
    UtlMap<int, EconItemDefinition*> itemsSorted;
    PAD(WIN32_LINUX(0x104, 0x168))
    UtlMap<std::uint64_t, AlternateIconData> alternateIcons;
    PAD(WIN32_LINUX(0x48, 0x60))
    UtlMap<int, PaintKit*> paintKits;
    UtlMap<int, StickerKit*> stickerKits;

    VIRTUAL_METHOD(EconItemDefinition*, getItemDefinitionInterface, 4, (WeaponId id), (this, id))
    VIRTUAL_METHOD(const char*, getRarityName, 19, (uint8_t rarity), (this, rarity))
    VIRTUAL_METHOD(int, getItemSetCount, 28, (), (this))
    VIRTUAL_METHOD(EconItemSetDefinition*, getItemSet, 29, (int index), (this, index))
    VIRTUAL_METHOD(EconLootListDefinition*, getLootList, 32, (int index), (this, index))
    VIRTUAL_METHOD(int, getLootListCount, 34, (), (this))
    VIRTUAL_METHOD(EconItemDefinition*, getItemDefinitionByName, 42, (const char* name), (this, name))
};

class ItemSystem {
public:
    VIRTUAL_METHOD(ItemSchema*, getItemSchema, 0, (), (this))
};
