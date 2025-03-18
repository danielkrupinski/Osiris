#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

#include "Meta.h"
#include "TypeIndex.h"

template <typename... Types>
struct TypeList {
    template <typename... OtherTypes>
    static TypeList<OtherTypes...> typeListFromTuple(std::tuple<OtherTypes...>);

    using TypesTuple = std::tuple<Types...>;

    static constexpr std::size_t size() noexcept
    {
        return sizeof...(Types);
    }

    template <typename T>
    static constexpr std::size_t indexOf() noexcept
    {
        return utils::typeIndex<T, std::tuple<Types...>>();
    }

    template <typename T>
    static constexpr bool contains() noexcept
    {
        return (std::is_same_v<T, Types> || ...);
    }

    template <template <typename> typename ValueOfType>
    static constexpr auto (min)() noexcept
    {
        return (std::min)({ValueOfType<Types>::value...});
    }

    template <template <typename> typename ValueOfType>
    static constexpr auto (max)() noexcept
    {
        return (std::max)({ValueOfType<Types>::value...});
    }

    template <template <typename> typename Predicate>
    using filter = decltype(typeListFromTuple(std::tuple_cat(std::declval<std::conditional_t<Predicate<Types>::value, std::tuple<Types>, std::tuple<>>>()...)));

    template <typename T>
    using add = TypeList<Types..., T>;

    template <typename OtherTypeList>
    using concat = decltype(typeListFromTuple(std::tuple_cat(std::declval<std::tuple<Types...>>(), std::declval<typename OtherTypeList::TypesTuple>())));

    template <template <typename> typename ValueOfType>
    struct Sorter {
        template <decltype(min<ValueOfType>())... Values>
        static auto sorted(std::integer_sequence<decltype(min<ValueOfType>()), Values...>) -> decltype(std::tuple_cat(std::declval<typename filter<WithValue<ValueOfType, Values + min<ValueOfType>()>::template Equal>::TypesTuple>()...));
    };

    template <template <typename> typename ValueOfType>
    using sortBy = decltype(typeListFromTuple(std::declval<decltype(Sorter<ValueOfType>::template sorted<>(std::make_integer_sequence<decltype(min<ValueOfType>()), max<ValueOfType>() - min<ValueOfType>() + 1>{}))>()));

    static constexpr void forEach(auto&& callable) noexcept
    {
        (callable(std::type_identity<Types>{}), ...);
    }
};
