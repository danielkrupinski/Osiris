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

class Item {
public:
    struct CommonProperties {
        std::uint32_t tradableAfterDate = 0;
        bool purchasedFromStore = false;
    };

    using VariantProperties = SmallVariant<32,
        std::monostate,
        Skin,
        Gloves,
        Agent,
        Music,
        Graffiti,
        ServiceMedal,
        SouvenirPackage,
        TournamentCoin,
        StorageUnit
    >;

    struct Properties {
        CommonProperties common;
        VariantProperties variant;
    };

    explicit Item(const game_items::Item& item, VariantProperties data) noexcept : item{ item }, properties{ {}, std::move(data) } {}
    explicit Item(const game_items::Item& item, Properties properties) noexcept : item{ item }, properties{ std::move(properties) } {}
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

    [[nodiscard]] Properties& getProperties() noexcept
    {
        return properties;
    }

    [[nodiscard]] const Properties& getProperties() const noexcept
    {
        return properties;
    }

    template <typename T>
    [[nodiscard]] T* getOrCreate()
    {
        if (const auto got = properties.variant.get<T>())
            return got;
        properties.variant = T{};
        return properties.variant.get<T>();
    }

private:
    std::reference_wrapper<const game_items::Item> item;
    Properties properties;
    State state = State::Default;
};

template <typename T>
[[nodiscard]] T* get(Item& item)
{
    return item.getProperties().variant.get<T>();
}

template <typename T>
[[nodiscard]] const T* get(const Item& item)
{
    return item.getProperties().variant.get<T>();
}

[[nodiscard]] inline int* getStatTrak(Item& item)
{
    if (const auto skin = get<Skin>(item))
        return &skin->statTrak;

    if (const auto music = get<Music>(item))
        return &music->statTrak;

    return nullptr;
}

[[nodiscard]] inline std::optional<int> getStatTrak(const Item& item)
{
    if (const auto skin = get<Skin>(item))
        return skin->statTrak;

    if (const auto music = get<Music>(item))
        return music->statTrak;

    return std::nullopt;
}

}
