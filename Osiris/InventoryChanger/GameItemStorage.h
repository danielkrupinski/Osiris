#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "StaticData.h"
#include "../SDK/WeaponId.h"

using StaticData::Type;

class GameItemStorage {
public:
    void addGloves(int rarity, WeaponId weaponID, std::size_t dataIndex, std::string&& iconPath)
    {
        addItem(Type::Glove, rarity, weaponID, dataIndex, std::move(iconPath));
    }

    void addSkin(int rarity, WeaponId weaponID, std::size_t dataIndex, std::string&& iconPath)
    {
        addItem(Type::Skin, rarity, weaponID, dataIndex, std::move(iconPath));
    }

    void addSticker(int rarity, std::size_t dataIndex, std::string&& iconPath)
    {
        addItem(Type::Sticker, rarity, WeaponId::Sticker, dataIndex, std::move(iconPath));
    }

    void addPatch(int rarity, std::size_t dataIndex, std::string&& iconPath)
    {
        addItem(Type::Patch, rarity, WeaponId::Patch, dataIndex, std::move(iconPath));
    }

    void addGraffiti(int rarity, std::size_t dataIndex, std::string&& iconPath)
    {
        addItem(Type::Graffiti, rarity, WeaponId::Graffiti, dataIndex, std::move(iconPath));
    }

    void addSealedGraffiti(int rarity, std::size_t dataIndex, std::string&& iconPath)
    {
        addItem(Type::SealedGraffiti, rarity, WeaponId::SealedGraffiti, dataIndex, std::move(iconPath));
    }

    void addMusicKit(int rarity, std::size_t dataIndex, std::string&& iconPath)
    {
        addItem(Type::Music, rarity, WeaponId::MusicKit, dataIndex, std::move(iconPath));
    }

    void addServiceMedal(int rarity, WeaponId weaponID, std::size_t dataIndex, std::string&& iconPath)
    {
        addItem(Type::ServiceMedal, rarity, weaponID, dataIndex, std::move(iconPath));
    }

    void addTournamentCoin(int rarity, WeaponId weaponID, std::size_t dataIndex, std::string&& iconPath)
    {
        addItem(Type::TournamentCoin, rarity, weaponID, dataIndex, std::move(iconPath));
    }

    void addCollectible(int rarity, WeaponId weaponID, std::size_t dataIndex, std::string&& iconPath)
    {
        addItem(Type::Collectible, rarity, weaponID, dataIndex, std::move(iconPath));
    }

    void addNameTag(int rarity, WeaponId weaponID, std::string&& iconPath)
    {
        addItem(Type::NameTag, rarity, weaponID, 0, std::move(iconPath));
    }

    void addAgent(int rarity, WeaponId weaponID, std::string&& iconPath)
    {
        addItem(Type::Agent, rarity, weaponID, 0, std::move(iconPath));
    }

    void addCase(int rarity, WeaponId weaponID, std::size_t dataIndex, std::string&& iconPath)
    {
        addItem(Type::Case, rarity, weaponID, dataIndex, std::move(iconPath));
    }

    void addCaseKey(int rarity, WeaponId weaponID, std::string&& iconPath)
    {
        addItem(Type::CaseKey, rarity, weaponID, 0, std::move(iconPath));
    }

    void addOperationPass(int rarity, WeaponId weaponID, std::string&& iconPath)
    {
        addItem(Type::OperationPass, rarity, weaponID, 0, std::move(iconPath));
    }

    void addStatTrakSwapTool(int rarity, WeaponId weaponID, std::string&& iconPath)
    {
        addItem(Type::StatTrakSwapTool, rarity, weaponID, 0, std::move(iconPath));
    }

    void addSouvenirToken(int rarity, WeaponId weaponID, std::size_t dataIndex, std::string&& iconPath)
    {
        addItem(Type::SouvenirToken, rarity, weaponID, dataIndex, std::move(iconPath));
    }

    void addViewerPass(int rarity, WeaponId weaponID, std::size_t dataIndex, std::string&& iconPath)
    {
        addItem(Type::ViewerPass, rarity, weaponID, dataIndex, std::move(iconPath));
    }

    std::span<StaticData::GameItem> get() noexcept
    {
        return gameItems;
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

private:
    void addItem(Type type, int rarity, WeaponId weaponID, std::size_t dataIndex, std::string&& iconPath)
    {
        gameItems.emplace_back(type, rarity, weaponID, dataIndex, std::move(iconPath));
    }

    std::vector<StaticData::GameItem> gameItems;
};
