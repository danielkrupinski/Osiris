#pragma once

#include <array>
#include <cassert>
#include <optional>
#include <string>
#include <vector>

#include "../SDK/ItemSchema.h"
#include "StaticData.h"

#include "GameItems/Item.h"

enum class Team;

enum TournamentTeam : std::uint8_t;
enum TournamentStage : std::uint8_t;
enum ProPlayer;

namespace inventory
{

struct Skin {
    struct Sticker {
        int stickerID = 0;
        float wear = 0.0f;
    };

    float wear = 0.0f;
    int seed = 1;
    int statTrak = -1;
    std::uint32_t tournamentID = 0;
    std::array<Sticker, 5> stickers;
    std::string nameTag;
    TournamentStage tournamentStage{};
    TournamentTeam tournamentTeam1{};
    TournamentTeam tournamentTeam2{};
    ProPlayer proPlayer{};

    [[nodiscard]] bool isSouvenir() const noexcept { return tournamentID != 0; }
};

struct Agent {
    struct Patch {
        int patchID = 0;
    };

    std::array<Patch, 5> patches;
};

struct Glove {
    float wear = 0.0f;
    int seed = 1;
};

struct Music {
    int statTrak = -1;
};

struct SouvenirPackage {
    TournamentStage tournamentStage{};
    TournamentTeam tournamentTeam1{};
    TournamentTeam tournamentTeam2{};
    ProPlayer proPlayer{};
};

struct ServiceMedal {
    std::uint32_t issueDateTimestamp = 0;
};

struct TournamentCoin {
    std::uint32_t dropsAwarded = 0;
};

struct Graffiti {
    std::int8_t usesLeft = -1;
};

struct Item;
}

namespace Inventory
{
    constexpr auto InvalidDynamicDataIdx = static_cast<std::size_t>(-1);
    constexpr auto BASE_ITEMID = 1152921504606746975;

    std::vector<inventory::Item>& get() noexcept;
    void addItemUnacknowledged(const game_items::Item& gameItem, std::size_t dynamicDataIdx) noexcept;
    void addItemAcknowledged(const game_items::Item& gameItem, std::size_t dynamicDataIdx) noexcept;
    std::uint64_t addItemNow(const game_items::Item& gameItem, std::size_t dynamicDataIdx, bool asUnacknowledged) noexcept;
    void deleteItemNow(std::uint64_t itemID) noexcept;
    void runFrame() noexcept;
    inventory::Item* getItem(std::uint64_t itemID) noexcept;
    std::uint64_t recreateItem(std::uint64_t itemID) noexcept;
    void clear() noexcept;
    void equipItem(Team team, int slot, std::size_t index) noexcept;
    std::size_t getItemIndex(std::uint64_t itemID) noexcept;

    inventory::Skin& dynamicSkinData(const inventory::Item& item) noexcept;
    inventory::Glove& dynamicGloveData(const inventory::Item& item) noexcept;
    inventory::Agent& dynamicAgentData(const inventory::Item& item) noexcept;
    inventory::Music& dynamicMusicData(const inventory::Item& item) noexcept;
    inventory::SouvenirPackage& dynamicSouvenirPackageData(const inventory::Item& item) noexcept;
    inventory::ServiceMedal& dynamicServiceMedalData(const inventory::Item& item) noexcept;
    inventory::TournamentCoin& dynamicTournamentCoinData(const inventory::Item& item) noexcept;
    inventory::Graffiti& dynamicGraffitiData(const inventory::Item& item) noexcept;

    std::size_t emplaceDynamicData(inventory::Skin&& data) noexcept;
    std::size_t emplaceDynamicData(inventory::Glove&& data) noexcept;
    std::size_t emplaceDynamicData(inventory::Agent&& data) noexcept;
    std::size_t emplaceDynamicData(inventory::Music&& data) noexcept;
    std::size_t emplaceDynamicData(inventory::SouvenirPackage&& data) noexcept;
    std::size_t emplaceDynamicData(inventory::ServiceMedal&& data) noexcept;
    std::size_t emplaceDynamicData(inventory::TournamentCoin&& data) noexcept;
    std::size_t emplaceDynamicData(inventory::Graffiti&& data) noexcept;
}
