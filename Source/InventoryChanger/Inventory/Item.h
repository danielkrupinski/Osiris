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

class StructWrapper {
public:
    StructWrapper() = default;

    template <typename T>
    StructWrapper(const T& object) : variant{ ValueWrapper{ object } } {}

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
    struct AlwaysFalse : std::false_type {};

    // yet to be enabled for some types
    template <typename T>
    struct ValueWrapper<T, std::enable_if_t<AlwaysFalse<T>::value>> {
        explicit ValueWrapper(const T& t) : object{ t } { }

        T& get() noexcept { return object; }
        const T& get() const noexcept { return object; }
    private:
        T object;
    };

    std::variant<
        std::monostate,
        ValueWrapper<Skin>,
        ValueWrapper<Glove>,
        ValueWrapper<Agent>,
        ValueWrapper<Music>,
        ValueWrapper<Graffiti>,
        ValueWrapper<ServiceMedal>,
        ValueWrapper<SouvenirPackage>,
        ValueWrapper<TournamentCoin>
    > variant;
};

class Item {
public:
    explicit Item(const game_items::Item& item, StructWrapper data) noexcept : item{ item }, data{ std::move(data) } {}
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
    StructWrapper data;
};

}
