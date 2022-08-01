#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <variant>

#include <SmallVariant.h>
#include <InventoryChanger/GameItems/Item.h>
#include "Structs.h"

namespace inventory_changer::inventory
{

using ItemData = SmallVariant<32,
    std::monostate,
    Skin,
    Glove,
    Agent,
    Music,
    Graffiti,
    ServiceMedal,
    SouvenirPackage,
    TournamentCoin,
    StorageUnit
>;

class Item {
public:
    explicit Item(const game_items::Item& item, ItemData data) noexcept : item{ item }, data{ std::move(data) } {}
    explicit Item(const game_items::Item& item) noexcept : item{ item } {}

    Item(Item&&) = default;

    Item& operator=(const Item&) = delete;
    Item& operator=(Item&&) = delete;

    [[nodiscard]] const game_items::Item& gameItem() const noexcept { return item; }

    enum class State : std::uint8_t {
        Default = 0,
        InXrayScanner,
        InStorageUnit
    };

    [[nodiscard]] State getState() const noexcept { return state; }
    void setState(State newState) noexcept { state = newState; }

    template <typename T>
    [[nodiscard]] T* get() { return data.get<T>(); }

    template <typename T>
    [[nodiscard]] const T* get() const { return data.get<T>(); }

    template <typename T>
    [[nodiscard]] T* getOrCreate()
    {
        if (const auto got = data.get<T>())
            return got;
        data = T{};
        return data.get<T>();
    }

private:
    std::reference_wrapper<const game_items::Item> item;
    ItemData data;
    State state = State::Default;
};

[[nodiscard]] inline int* getStatTrak(Item& item)
{
    if (const auto skin = item.get<Skin>())
        return &skin->statTrak;

    if (const auto music = item.get<Music>())
        return &music->statTrak;

    return nullptr;
}

[[nodiscard]] inline std::optional<int> getStatTrak(const Item& item)
{
    if (const auto skin = item.get<Skin>())
        return skin->statTrak;

    if (const auto music = item.get<Music>())
        return music->statTrak;

    return std::nullopt;
}

}
