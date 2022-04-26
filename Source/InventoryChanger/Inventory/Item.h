#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <variant>

#include <InventoryChanger/GameItems/Item.h>
#include "Structs.h"

namespace inventory
{

template <std::size_t MaxStaticSize, typename... Types>
class SmallVariant {
public:
    SmallVariant() = default;

    template <typename T>
    SmallVariant(const T& object) : variant{ ValueWrapper<T>{ object } } {}

    template <typename T>
    [[nodiscard]] T* get()
    {
        if (const auto item = std::get_if<ValueWrapper<T>>(&variant))
            return &item->get();
        return nullptr;
    }

    template <typename T>
    [[nodiscard]] const T* get() const
    {
        if (const auto item = std::get_if<ValueWrapper<T>>(&variant))
            return &item->get();
        return nullptr;
    }

private:
    template <typename T, typename = void>
    struct ValueWrapper {
        explicit ValueWrapper(const T& t) : object{ std::make_unique<T>(t) } {}

        T& get() noexcept { return *object.get(); }
        const T& get() const noexcept { return *object.get(); }
    private:
        std::unique_ptr<T> object;
    };

    template <typename T>
    struct ValueWrapper<T, std::enable_if_t<std::is_same_v<T, std::monostate> || sizeof(T) <= MaxStaticSize>> {
        ValueWrapper() = default;
        explicit ValueWrapper(const T& t) : object{ t } { }

        T& get() noexcept { return object; }
        const T& get() const noexcept { return object; }
    private:
        T object;
    };

    std::variant<ValueWrapper<Types>...> variant;
};

using ItemData = SmallVariant<32,
    std::monostate,
    Skin,
    Glove,
    Agent,
    Music,
    Graffiti,
    ServiceMedal,
    SouvenirPackage,
    TournamentCoin
>;

class Item {
public:
    explicit Item(const game_items::Item& item, ItemData data) noexcept : item{ item }, data{ std::move(data) } {}
    explicit Item(const game_items::Item& item) noexcept : item{ item } {}

    [[nodiscard]] const game_items::Item& gameItem() const noexcept { return item; }

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
};

[[nodiscard]] inline int* getStatTrak(Item& item)
{
    if (const auto skin = item.get<Skin>())
        return &skin->statTrak;

    if (const auto music = item.get<Music>())
        return &music->statTrak;

    return nullptr;
}

[[nodiscard]] inline const int* getStatTrak(const Item& item)
{
    if (const auto skin = item.get<Skin>())
        return &skin->statTrak;

    if (const auto music = item.get<Music>())
        return &music->statTrak;

    return nullptr;
}

}
