#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <utility>

template <typename T>
class Optional;

template <>
class Optional<bool> {
public:
    Optional() = default;

    Optional(std::nullopt_t) noexcept
        : _value{Value::Unknown}
    {
    }

    Optional(bool value) noexcept
        : _value{value ? Value::True : Value::False}
    {
    }

    constexpr bool operator==(const Optional&) const = default;

    [[nodiscard]] bool hasValue() const noexcept
    {
        return _value != Value::Unknown;
    }

    [[nodiscard]] bool value() const noexcept
    {
        return _value == Value::True;
    }

    [[nodiscard]] bool valueOr(bool defaultValue) const noexcept
    {
        if (_value != Value::Unknown)
            return _value == Value::True;
        return defaultValue;
    }

private:
    enum class Value : std::uint8_t {
        Unknown,
        False,
        True
    };

    Value _value{Value::Unknown};
};

template <typename T>
class Optional {
public:
    Optional() = default;

    template <typename U>
        requires std::is_constructible_v<std::optional<T>, U&&>
    Optional(U&& value) noexcept
        : optional{std::forward<U>(value)}
    {
    }

    constexpr bool operator==(const Optional&) const = default;

    [[nodiscard]] bool hasValue() const noexcept
    {
        return optional.has_value();
    }

    [[nodiscard]] const T& value() const noexcept
    {
        return *optional;
    }

    template <typename... Args>
    [[nodiscard]] T valueOr(Args&&... args) const noexcept
    {
        return optional.value_or(std::forward<Args>(args)...);
    }

    template <typename U>
    [[nodiscard]] Optional<bool> equal(const U& other) const noexcept
    {
        return compare(other, std::equal_to{});
    }

    template <typename U>
    [[nodiscard]] Optional<bool> notEqual(const U& other) const noexcept
    {
        return compare(other, std::not_equal_to{});
    }

    template <typename U>
    [[nodiscard]] Optional<bool> lessThan(const U& other) const noexcept
    {
        return compare(other, std::less{});
    }

    template <typename U>
    [[nodiscard]] Optional<bool> greaterThan(const U& other) const noexcept
    {
        return compare(other, std::greater{});
    }

    template <typename U>
    [[nodiscard]] Optional<bool> lessEqual(const U& other) const noexcept
    {
        return compare(other, std::less_equal{});
    }

private:
    template <typename U, typename Comparator>
    [[nodiscard]] Optional<bool> compare(const U& value, Comparator comparator) const noexcept
    {
        if (this->hasValue())
            return comparator(this->value(), value);
        return {};
    }

    template <typename U, typename Comparator>
    [[nodiscard]] Optional<bool> compare(const Optional<U>& other, Comparator comparator) const noexcept
    {
        if (hasValue() && other.hasValue())
            return comparator(value(), other.value());
        return {};
    }

    std::optional<T> optional;
};

template <typename T, typename U>
[[nodiscard]] auto operator-(const Optional<T>& lhs, const Optional<U>& rhs) noexcept -> Optional<decltype(lhs.value() - rhs.value())> 
{
    if (lhs.hasValue() && rhs.hasValue())
        return lhs.value() - rhs.value();
    return {};
}

template <typename T, typename U>
[[nodiscard]] auto operator-(const Optional<T>& optional, const U& value) noexcept -> Optional<decltype(optional.value() - value)>
{
    if (optional.hasValue())
        return optional.value() - value;
    return {};
}

template <typename T, typename U>
[[nodiscard]] auto operator&(const Optional<T>& lhs, const Optional<U>& rhs) noexcept -> Optional<decltype(lhs.value() & rhs.value())> 
{
    if (lhs.hasValue() && rhs.hasValue())
        return lhs.value() & rhs.value();
    return {};
}

template <typename T, typename U>
[[nodiscard]] auto operator&(const Optional<T>& optional, const U& value) noexcept -> Optional<decltype(optional.value() & value)>
{
    if (optional.hasValue())
        return optional.value() & value;
    return {};
}
