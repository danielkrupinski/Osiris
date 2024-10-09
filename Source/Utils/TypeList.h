#pragma once

#include <tuple>

#include "TypeIndex.h"

template <typename... Types>
struct TypeList {
private:
    template <typename... OtherTypes>
    static TypeList<OtherTypes...> typeListFromTuple(std::tuple<OtherTypes...>);

public:
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

    template <template <typename> typename Predicate>
    using filter = decltype(typeListFromTuple(std::tuple_cat(std::declval<std::conditional_t<Predicate<Types>::value, std::tuple<Types>, std::tuple<>>>()...)));

    template <template <typename> typename TransformOperation, template <typename> typename Predicate>
    using filterTransformed = decltype(typeListFromTuple(std::tuple_cat(std::declval<std::conditional_t<Predicate<typename TransformOperation<Types>::type>::value, std::tuple<Types>, std::tuple<>>>()...)));

    template <typename T>
    using add = TypeList<Types..., T>;

    template <typename OtherTypeList>
    using concat = decltype(typeListFromTuple(std::tuple_cat(std::declval<std::tuple<Types...>>(), std::declval<typename OtherTypeList::TypesTuple>())));
};
