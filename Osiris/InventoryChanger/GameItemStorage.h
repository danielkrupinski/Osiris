#pragma once

#include <algorithm>
#include <cstddef>
#include <span>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

#include "StaticData.h"
#include "../SDK/WeaponId.h"

using StaticData::Type;

class GameItemStorage {
public:
    void addGloves(int rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath)
    {
        addItem(Type::Glove, rarity, weaponID, dataIndex, iconPath);
    }

    void addSkin(int rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath)
    {
        addItem(Type::Skin, rarity, weaponID, dataIndex, iconPath);
    }

    void addSticker(int rarity, std::size_t dataIndex, std::string_view iconPath)
    {
        addItem(Type::Sticker, rarity, WeaponId::Sticker, dataIndex, iconPath);
    }

    void addPatch(int rarity, std::size_t dataIndex, std::string_view iconPath)
    {
        addItem(Type::Patch, rarity, WeaponId::Patch, dataIndex, iconPath);
    }

    void addGraffiti(int rarity, std::size_t dataIndex, std::string_view iconPath)
    {
        addItem(Type::Graffiti, rarity, WeaponId::Graffiti, dataIndex, iconPath);
    }

    void addSealedGraffiti(int rarity, std::size_t dataIndex, std::string_view iconPath)
    {
        addItem(Type::SealedGraffiti, rarity, WeaponId::SealedGraffiti, dataIndex, iconPath);
    }

    void addMusicKit(int rarity, std::size_t dataIndex, std::string_view iconPath)
    {
        addItem(Type::Music, rarity, WeaponId::MusicKit, dataIndex, iconPath);
    }

    void addServiceMedal(int rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath)
    {
        addItem(Type::ServiceMedal, rarity, weaponID, dataIndex, iconPath);
    }

    void addTournamentCoin(int rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath)
    {
        addItem(Type::TournamentCoin, rarity, weaponID, dataIndex, iconPath);
    }

    void addCollectible(int rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath)
    {
        addItem(Type::Collectible, rarity, weaponID, dataIndex, iconPath);
    }

    void addNameTag(int rarity, WeaponId weaponID, std::string_view iconPath)
    {
        addItem(Type::NameTag, rarity, weaponID, 0, iconPath);
    }

    void addAgent(int rarity, WeaponId weaponID, std::string_view iconPath)
    {
        addItem(Type::Agent, rarity, weaponID, 0, iconPath);
    }

    void addCase(int rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath)
    {
        addItem(Type::Case, rarity, weaponID, dataIndex, iconPath);
    }

    void addCaseKey(int rarity, WeaponId weaponID, std::string_view iconPath)
    {
        addItem(Type::CaseKey, rarity, weaponID, 0, iconPath);
    }

    void addOperationPass(int rarity, WeaponId weaponID, std::string_view iconPath)
    {
        addItem(Type::OperationPass, rarity, weaponID, 0, iconPath);
    }

    void addStatTrakSwapTool(int rarity, WeaponId weaponID, std::string_view iconPath)
    {
        addItem(Type::StatTrakSwapTool, rarity, weaponID, 0, iconPath);
    }

    void addSouvenirToken(int rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath)
    {
        addItem(Type::SouvenirToken, rarity, weaponID, dataIndex, iconPath);
    }

    void addViewerPass(int rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath)
    {
        addItem(Type::ViewerPass, rarity, weaponID, dataIndex, iconPath);
    }

    std::span<StaticData::GameItem> get() noexcept
    {
        return gameItems;
    }

    const StaticData::GameItem& get(std::size_t index) const noexcept
    {
        return gameItems[index];
    }

    std::span<const StaticData::GameItem> get() const noexcept
    {
        return gameItems;
    }

    void shrinkToFit()
    {
        gameItems.shrink_to_fit();
    }

    void reserve(std::size_t capacity)
    {
        gameItems.reserve(capacity);
    }

    [[nodiscard]] std::size_t size() const
    {
        return gameItems.size();
    }

    [[nodiscard]] std::unordered_set<WeaponId> getUniqueWeaponIDs() const
    {
        std::unordered_set<WeaponId> uniqueWeaponIDs;
        for (const auto& gameItem : gameItems)
            uniqueWeaponIDs.emplace(gameItem.weaponID);
        return uniqueWeaponIDs;
    }

    template <typename Comparator>
    void sort(Comparator comparator)
    {
        std::ranges::sort(gameItems, comparator);
    }

private:
    void addItem(Type type, int rarity, WeaponId weaponID, std::size_t dataIndex, std::string_view iconPath)
    {
        gameItems.emplace_back(type, rarity, weaponID, dataIndex, iconPath);
    }

    std::vector<StaticData::GameItem> gameItems;
};
