#pragma once

#include <array>
#include <cassert>
#include <string>
#include <vector>

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
public:
    explicit InventoryItem(std::size_t itemIndex, std::size_t dynamicDataIndex) noexcept : itemIndex{ itemIndex }, dynamicDataIndex{ dynamicDataIndex } {}

    void markAsDeleted() noexcept { itemIndex = static_cast<std::size_t>(-1); }
    bool isDeleted() const noexcept { return itemIndex == static_cast<std::size_t>(-1); }
    void markToDelete() noexcept { itemIndex = static_cast<std::size_t>(-2); }
    bool shouldDelete() const noexcept { return itemIndex == static_cast<std::size_t>(-2); }
    bool isValid() const noexcept { return !isDeleted() && !shouldDelete(); }

    bool isSticker() const noexcept { return isValid() && get().isSticker(); }
    bool isSkin() const noexcept { return isValid() && get().isSkin(); }
    bool isGlove() const noexcept { return isValid() && get().isGlove(); }
    bool isMusic() const noexcept { return isValid() && get().isMusic(); }
    bool isAgent() const noexcept { return isValid() && get().isAgent(); }
    bool isCollectible() const noexcept { return isValid() && get().isCollectible(); }
    bool isCase() const noexcept { return isValid() && get().isCase(); }
    bool isCaseKey() const noexcept { return isValid() && get().isCaseKey(); }
    bool isSealedGraffiti() const noexcept { return isValid() && get().isSealedGraffiti(); }
    bool isOperationPass() const noexcept { return isValid() && get().isOperationPass(); }
    bool isNameTag() const noexcept { return isValid() && get().isNameTag(); }
    bool isPatch() const noexcept { return isValid() && get().isPatch(); }
    bool isStatTrakSwapTool() const noexcept { return isValid() && get().isStatTrakSwapTool(); }
    bool isViewerPass() const noexcept { return isValid() && get().isViewerPass(); }

    std::size_t getDynamicDataIndex() const noexcept { assert(dynamicDataIndex != static_cast<std::size_t>(-1)); return dynamicDataIndex; }

    const StaticData::GameItem& get() const noexcept { assert(isValid()); return StaticData::gameItems()[itemIndex]; }
};

namespace Inventory
{
    constexpr auto INVALID_DYNAMIC_DATA_IDX = static_cast<std::size_t>(-1);

    std::vector<InventoryItem>& get() noexcept;
    void addItemUnacknowledged(std::size_t gameItemIndex, std::size_t dynamicDataIdx) noexcept;
    void addItemAcknowledged(std::size_t gameItemIndex, std::size_t dynamicDataIdx) noexcept;
    std::uint64_t addItemNow(std::size_t gameItemIndex, std::size_t dynamicDataIdx, bool asUnacknowledged) noexcept;
    void deleteItemNow(std::uint64_t itemID) noexcept;
    void runFrame() noexcept;
    InventoryItem* getItem(std::uint64_t itemID) noexcept;
    std::uint64_t recreateItem(std::uint64_t itemID) noexcept;
    void clear() noexcept;
    void equipItem(Team team, int slot, std::size_t index) noexcept;
    std::size_t getItemIndex(std::uint64_t itemID) noexcept;

    DynamicSkinData& dynamicSkinData(std::size_t index) noexcept;
    DynamicGloveData& dynamicGloveData(std::size_t index) noexcept;
    DynamicAgentData& dynamicAgentData(std::size_t index) noexcept;
    DynamicMusicData& dynamicMusicData(std::size_t index) noexcept;

    std::size_t emplaceDynamicData(DynamicSkinData&& data) noexcept;
    std::size_t emplaceDynamicData(DynamicGloveData&& data) noexcept;
    std::size_t emplaceDynamicData(DynamicAgentData&& data) noexcept;
    std::size_t emplaceDynamicData(DynamicMusicData&& data) noexcept;
}
