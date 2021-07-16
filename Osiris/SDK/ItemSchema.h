#pragma once

#include <cstddef>

#include "Inconstructible.h"
#include "Pad.h"
#include "Entity.h"
#include "UtlVector.h"
#include "VirtualMethod.h"

#include "../Memory.h"

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

struct UtlString {
    UtlMemory<char> buffer;
    int length;

    const char* data() const noexcept { return buffer.memory; }
};

struct PaintKit {
    int id;
    UtlString name;
    UtlString description;
    UtlString itemName;
    UtlString sameNameFamilyAggregate;
    UtlString pattern;
    UtlString normal;
    UtlString logoMaterial;
    bool baseDiffuseOverride;
    int rarity;
    PAD(40)
    float wearRemapMin;
    float wearRemapMax;
};

struct StickerKit {
    int id;
    int rarity;
    UtlString name;
    UtlString description;
    UtlString itemName;
    PAD(2 * sizeof(UtlString))
    UtlString inventoryImage;
};

union AttributeDataUnion {
    float asFloat;
    std::uint32_t asUint32;
    char* asBlobPointer;
};

struct StaticAttrib {
    std::uint16_t defIndex;
    AttributeDataUnion value;
    bool forceGCToGenerate;
};
static_assert(sizeof(StaticAttrib) == WIN32_LINUX(12, 24));

struct EconTool {
    INCONSTRUCTIBLE(EconTool)

    PAD(sizeof(std::uintptr_t))
    const char* typeName;
};

class EconItemDefinition {
public:
    INCONSTRUCTIBLE(EconItemDefinition)

    VIRTUAL_METHOD(WeaponId, getWeaponId, 0, (), (this))
    VIRTUAL_METHOD(const char*, getItemBaseName, 2, (), (this))
    VIRTUAL_METHOD(const char*, getItemTypeName, 3, (), (this))
    VIRTUAL_METHOD(const char*, getInventoryImage, 5, (), (this))
    VIRTUAL_METHOD(const char*, getPlayerDisplayModel, 6, (), (this))
    VIRTUAL_METHOD(const char*, getWorldDisplayModel, 7, (), (this))
    VIRTUAL_METHOD(std::uint8_t, getRarity, 12, (), (this))

    std::uint8_t getQuality() noexcept
    {
        return *reinterpret_cast<std::uint8_t*>(std::uintptr_t(this) + WIN32_LINUX(0x2B, 0x4B));
    }

    int getCapabilities() noexcept
    {
        return *reinterpret_cast<int*>(this + WIN32_LINUX(0x148, 0x1F8));
    }

    const UtlVector<StaticAttrib>& getStaticAttributes() noexcept
    {
        return *reinterpret_cast<const UtlVector<StaticAttrib>*>(std::uintptr_t(this) + WIN32_LINUX(0x30, 0x50));
    }

    std::uint32_t getCrateSeriesNumber() noexcept
    {
        const auto& staticAttributes = getStaticAttributes();
        for (int i = 0; i < staticAttributes.size; ++i)
            if (staticAttributes[i].defIndex == 68 /* "set supply crate series" */)
                return staticAttributes[i].value.asUint32;
        return 0;
    }

    bool hasCrateSeries() noexcept
    {
        return getCrateSeriesNumber() != 0;
    }

    std::uint32_t getTournamentEventID() noexcept
    {
        const auto& staticAttributes = getStaticAttributes();
        for (int i = 0; i < staticAttributes.size; ++i)
            if (staticAttributes[i].defIndex == 137 /* "tournament event id" */)
                return staticAttributes[i].value.asUint32;
        return 0;
    }

    bool hasTournamentEventID() noexcept
    {
        return getTournamentEventID() != 0;
    }

    bool isPaintable() noexcept { return getCapabilities() & 1; /* ITEM_CAP_PAINTABLE */ }
    bool isPatchable() noexcept { return getCapabilities() & (1 << 22); /* ITEM_CAP_CAN_PATCH */ }

    const char* getDefinitionName() noexcept
    {
        return *reinterpret_cast<const char**>(this + WIN32_LINUX(0x1DC, 0x2E0));
    }

    EconTool* getEconTool() noexcept
    {
        return *reinterpret_cast<EconTool**>(std::uintptr_t(this) + WIN32_LINUX(0x140, 0x1E8));
    }

    int getLoadoutSlot(Team team) noexcept
    {
        if (team >= Team::None && team <= Team::CT)
            return reinterpret_cast<int*>(std::uintptr_t(this) + WIN32_LINUX(0x28C, 0x3F4))[static_cast<int>(team)];
        return *reinterpret_cast<int*>(std::uintptr_t(this) + WIN32_LINUX(0x268, 0x3BC));
    }
};

struct ItemListEntry {
    int itemDef;
    int paintKit;
    int paintKitSeed;
    float paintKitWear;
    std::uint32_t stickerKit;
    std::uint32_t musicKit;
    bool isNestedList;
    bool isUnusualList;
    PAD(2)

    auto weaponId() const noexcept
    {
        return static_cast<WeaponId>(itemDef);
    }
};

class EconLootListDefinition {
public:
    INCONSTRUCTIBLE(EconLootListDefinition)

    VIRTUAL_METHOD(const char*, getName, 0, (), (this))
    VIRTUAL_METHOD(const UtlVector<ItemListEntry>&, getLootListContents, 1, (), (this))

    bool willProduceStatTrak() noexcept
    {
        return *reinterpret_cast<bool*>(std::uintptr_t(this) + WIN32_LINUX(0x36, 0x56));
    }
};

class EconItemSetDefinition {
public:
    INCONSTRUCTIBLE(EconItemSetDefinition)

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
    UtlString simpleName;
    UtlString largeSimpleName;
    UtlString iconURLSmall;
    UtlString iconURLLarge;
    PAD(WIN32_LINUX(28, 48))
};

struct EconMusicDefinition {
    int id;
    const char* name;
    const char* nameLocalized;
    PAD(2 * sizeof(const char*))
    const char* inventoryImage;
};

class EconItemAttributeDefinition;

class ItemSchema {
public:
    INCONSTRUCTIBLE(ItemSchema)

    PAD(WIN32_LINUX(0x88, 0xB8))
    UtlMap<int, EconItemQualityDefinition> qualities;
    PAD(WIN32_LINUX(0x48, 0x60))
    UtlMap<int, EconItemDefinition*> itemsSorted;
    PAD(WIN32_LINUX(0x60, 0x88))
    UtlMap<int, const char*> revolvingLootLists;
    PAD(WIN32_LINUX(0x80, 0xB0))
    UtlMap<std::uint64_t, AlternateIconData> alternateIcons;
    PAD(WIN32_LINUX(0x48, 0x60))
    UtlMap<int, PaintKit*> paintKits;
    UtlMap<int, StickerKit*> stickerKits;
    PAD(WIN32_LINUX(0x11C, 0x1A0))
    UtlMap<int, EconMusicDefinition*> musicKits;

    VIRTUAL_METHOD(EconItemDefinition*, getItemDefinitionInterface, 4, (WeaponId id), (this, id))
    VIRTUAL_METHOD(const char*, getRarityName, 19, (uint8_t rarity), (this, rarity))
    VIRTUAL_METHOD(EconItemAttributeDefinition*, getAttributeDefinitionInterface, 27, (int index), (this, index))
    VIRTUAL_METHOD(int, getItemSetCount, 28, (), (this))
    VIRTUAL_METHOD(EconItemSetDefinition*, getItemSet, 29, (int index), (this, index))
    VIRTUAL_METHOD(EconLootListDefinition*, getLootList, 31, (const char* name, int* index = nullptr), (this, name, index))
    VIRTUAL_METHOD(EconLootListDefinition*, getLootList, 32, (int index), (this, index))
    VIRTUAL_METHOD(int, getLootListCount, 34, (), (this))
    VIRTUAL_METHOD(EconItemDefinition*, getItemDefinitionByName, 42, (const char* name), (this, name))
};

class ItemSystem {
public:
    INCONSTRUCTIBLE(ItemSystem)

    VIRTUAL_METHOD(ItemSchema*, getItemSchema, 0, (), (this))
};

class EconItem {
public:
    VIRTUAL_METHOD(void, destructor, 0, (bool releaseMemory = true), (this, releaseMemory))

    PAD(2 * sizeof(std::uintptr_t))

    std::uint64_t itemID;
    std::uint64_t originalID;
    void* customDataOptimizedObject;
    std::uint32_t accountID;
    std::uint32_t inventory;
    WeaponId weaponId;

    std::uint16_t origin : 5;
    std::uint16_t quality : 4;
    std::uint16_t level : 2;
    std::uint16_t rarity : 4;
    std::uint16_t dirtybitInUse : 1;

    std::int16_t itemSet;
    int soUpdateFrame;
    std::uint8_t flags;

    void setAttributeValue(int index, void* value) noexcept
    {
        if (const auto attribute = memory->itemSystem()->getItemSchema()->getAttributeDefinitionInterface(index))
            memory->setDynamicAttributeValue(this, attribute, value);
    }

    void setPaintKit(float paintKit) noexcept { setAttributeValue(6, &paintKit); }
    void setSeed(float seed) noexcept { setAttributeValue(7, &seed); }
    void setWear(float wear) noexcept { setAttributeValue(8, &wear); }
    void setMusicID(int musicID) noexcept { setAttributeValue(166, &musicID); }
    void setStatTrak(int value) noexcept { setAttributeValue(80, &value); }
    void setStatTrakType(int type) noexcept { setAttributeValue(81, &type); }
    void setTournamentStage(int stage) noexcept { setAttributeValue(138, &stage); }
    void setTournamentTeam1(int team) noexcept { setAttributeValue(139, &team); }
    void setTournamentTeam2(int team) noexcept { setAttributeValue(140, &team); }

    void setStickerID(int slot, int stickerID) noexcept
    {
        if (slot >= 0 && slot <= 5)
            setAttributeValue(113 + 4 * slot, &stickerID);
    }

    void setStickerWear(int slot, float wear) noexcept
    {
        if (slot >= 0 && slot <= 5)
            setAttributeValue(114 + 4 * slot, &wear);
    }
};

class SharedObject {
public:
    INCONSTRUCTIBLE(SharedObject)

    VIRTUAL_METHOD_V(int, getTypeID, 1, (), (this))
};

template <typename T>
class SharedObjectTypeCache {
public:
    INCONSTRUCTIBLE(SharedObjectTypeCache)

    VIRTUAL_METHOD_V(void, addObject, 1, (T* object), (this, object))
    VIRTUAL_METHOD_V(void, removeObject, 3, (T* object), (this, object))

    PAD(sizeof(std::uintptr_t))
    T** objects;
    PAD(WIN32_LINUX(16, 24))
    int objectCount;
    PAD(WIN32_LINUX(4, 12))
    int classID; // https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/shared/econ/econ_item_constants.h#L39
};

template <typename T>
class ClientSharedObjectCache {
public:
    INCONSTRUCTIBLE(ClientSharedObjectCache)

    PAD(16)
    UtlVector<SharedObjectTypeCache<T>*> sharedObjectTypeCaches;

    SharedObjectTypeCache<T>* findBaseTypeCache(int classID) noexcept
    {
        return memory->createBaseTypeCache(this, classID);
    }
};

struct SOID {
    std::uint64_t id;
    std::uint32_t type;
    std::uint32_t padding;
};

class EconItemView;

class CSPlayerInventory {
public:
    INCONSTRUCTIBLE(CSPlayerInventory)

    VIRTUAL_METHOD(void, soUpdated, 1, (SOID owner, SharedObject* object, int event), (this, owner, object, event))
    VIRTUAL_METHOD(void, soDestroyed, 2, (SOID owner, SharedObject* object, int event), (this, owner, object, event))
    VIRTUAL_METHOD_V(EconItemView*, getItemInLoadout, 8, (Team team, int slot), (this, team, slot))
    VIRTUAL_METHOD_V(void, removeItem, 15, (std::uint64_t itemID), (this, itemID))

    auto getSOC() noexcept
    {
        return *reinterpret_cast<ClientSharedObjectCache<EconItem>**>(std::uintptr_t(this) + WIN32_LINUX(0x90, 0xC8));
    }

    SharedObjectTypeCache<EconItem>* getItemBaseTypeCache() noexcept
    {
        const auto soc = getSOC();
        if (!soc)
            return nullptr;

        return soc->findBaseTypeCache(1);
    }

    std::pair<std::uint64_t, std::uint32_t> getHighestIDs() noexcept
    {
        const auto soc = getSOC();
        if (!soc)
            return {};

        const auto baseTypeCache = soc->findBaseTypeCache(1);
        if (!baseTypeCache)
            return {};

        std::uint64_t maxItemID = 0;
        std::uint32_t maxInventoryID = 0;
        for (int i = 0; i < baseTypeCache->objectCount; ++i) {
            const auto item = baseTypeCache->objects[i];
            if (const auto isDefaultItem = (item->itemID & 0xF000000000000000) != 0)
                continue;
            maxItemID = (std::max)(maxItemID, item->itemID);
            maxInventoryID = (std::max)(maxInventoryID, item->inventory);
        }

        return std::make_pair(maxItemID, maxInventoryID);
    }

    auto getAccountID() noexcept
    {
        return *reinterpret_cast<std::uint32_t*>(std::uintptr_t(this) + WIN32_LINUX(0x8, 0x10));
    }

    auto getSOID() noexcept
    {
        return *reinterpret_cast<SOID*>(std::uintptr_t(this) + WIN32_LINUX(0x8, 0x10));
    }
};

class InventoryManager {
public:
    INCONSTRUCTIBLE(InventoryManager)

    VIRTUAL_METHOD_V(bool, equipItemInSlot, 20, (Team team, int slot, std::uint64_t itemID, bool swap = false), (this, team, slot, itemID, swap))
    VIRTUAL_METHOD_V(CSPlayerInventory*, getLocalInventory, 23, (), (this))
};
