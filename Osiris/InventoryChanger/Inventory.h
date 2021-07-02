#pragma once

#include "StaticData.h"

enum class Team;

struct StickerConfig {
    int stickerID = 0;
    float wear = 0.0f;
};

struct DynamicSkinData {
    bool isSouvenir = false;
    float wear = 0.0f;
    int seed = 1;
    int statTrak = -1;
    std::array<StickerConfig, 5> stickers;
    std::string nameTag;
};

struct PatchConfig {
    int patchID = 0;
};

struct DynamicAgentData {
    std::array<PatchConfig, 5> patches;
};

struct DynamicGloveData {
    float wear = 0.0f;
    int seed = 1;
};

struct DynamicMusicData {
    int statTrak = -1;
};

struct InventoryItem {
private:
    std::size_t itemIndex;
    std::size_t dynamicDataIndex = static_cast<std::size_t>(-1);

    // TODO: This should be moved to where items are generated from cases
    /*
    static float generateWear(bool factoryNewOnly) noexcept
    {
        if (factoryNewOnly)
            return randomFloat(0.0f, 0.07f);

        float wear;
        if (const auto condition = randomInt(1, 10000); condition <= 1471)
            wear = randomFloat(0.0f, 0.07f);
        else if (condition <= 3939)
            wear = randomFloat(0.07f, 0.15f);
        else if (condition <= 8257)
            wear = randomFloat(0.15f, 0.38f);
        else if (condition <= 9049)
            wear = randomFloat(0.38f, 0.45f);
        else
            wear = randomFloat(0.45f, 1.0f);
        return wear;
    }
    */
public:
    explicit InventoryItem(std::size_t itemIndex, std::size_t dynamicDataIndex) noexcept : itemIndex{ itemIndex }, dynamicDataIndex{ dynamicDataIndex } {}

    void markAsDeleted() noexcept { itemIndex = static_cast<std::size_t>(-1); }
    bool isDeleted() const noexcept { return itemIndex == static_cast<std::size_t>(-1); }
    void markToDelete() noexcept { itemIndex = static_cast<std::size_t>(-2); }
    bool shouldDelete() const noexcept { return itemIndex == static_cast<std::size_t>(-2); }

    bool isSticker() const noexcept { return !isDeleted() && !shouldDelete() && get().isSticker(); }
    bool isSkin() const noexcept { return !isDeleted() && !shouldDelete() && get().isSkin(); }
    bool isGlove() const noexcept { return !isDeleted() && !shouldDelete() && get().isGlove(); }
    bool isMusic() const noexcept { return !isDeleted() && !shouldDelete() && get().isMusic(); }
    bool isAgent() const noexcept { return !isDeleted() && !shouldDelete() && get().isAgent(); }
    bool isCollectible() const noexcept { return !isDeleted() && !shouldDelete() && get().isCollectible(); }
    bool isCase() const noexcept { return !isDeleted() && !shouldDelete() && get().isCase(); }
    bool isCaseKey() const noexcept { return !isDeleted() && !shouldDelete() && get().isCaseKey(); }

    std::size_t getDynamicDataIndex() const noexcept { assert(dynamicDataIndex != static_cast<std::size_t>(-1)); return dynamicDataIndex; }

    const StaticData::GameItem& get() const noexcept { assert(!isDeleted() && !shouldDelete()); return StaticData::gameItems()[itemIndex]; }
};

namespace Inventory
{
    constexpr auto INVALID_DYNAMIC_DATA_IDX = static_cast<std::size_t>(-1);

    std::vector<InventoryItem>& get() noexcept;
    void addItem(std::size_t gameItemIndex, std::size_t dynamicDataIdx, bool asUnacknowledged) noexcept;
    std::uint64_t addItemNow(std::size_t gameItemIndex, std::size_t dynamicDataIdx, bool asUnacknowledged) noexcept;
    void deleteItemNow(std::uint64_t itemID) noexcept;
    void runFrame() noexcept;
    InventoryItem* getItem(std::uint64_t itemID) noexcept;
    std::uint64_t recreateItem(std::uint64_t itemID) noexcept;
    void clear() noexcept;
    void equipItem(Team team, int slot, std::size_t index) noexcept;
    std::size_t getItemIndex(std::uint64_t itemID) noexcept;

    std::vector<DynamicSkinData>& dynamicSkinData() noexcept;
    std::vector<DynamicGloveData>& dynamicGloveData() noexcept;
    std::vector<DynamicAgentData>& dynamicAgentData() noexcept;
    std::vector<DynamicMusicData>& dynamicMusicData() noexcept;
}
