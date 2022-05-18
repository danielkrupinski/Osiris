#pragma once

#include <cstddef>
#include <memory>
#include <variant>

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
        explicit ValueWrapper(const T& t) : object{ t } {}

        T& get() noexcept { return object; }
        const T& get() const noexcept { return object; }
    private:
        T object;
    };

    std::variant<ValueWrapper<Types>...> variant;
};
