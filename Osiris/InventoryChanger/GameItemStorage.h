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

class GameItemStorage {
public:
    const StaticData::GameItem& get(std::size_t index) const noexcept
    {
        return gameItems[index];
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

    auto begin() { return gameItems.begin(); }
    auto begin() const { return gameItems.begin(); }
    auto cbegin() const { return gameItems.cbegin(); }
    auto end() { return gameItems.end(); }
    auto end() const { return gameItems.end(); }
    auto cend() const { return gameItems.cend(); }

    void addItem(const StaticData::GameItem& item)
    {
        gameItems.push_back(item);
    }

private:

    std::vector<StaticData::GameItem> gameItems;
};
