#pragma once

#include <cstddef>

template <template <typename> typename ValueOfType, auto Value>
struct WithValue {
    template <typename T>
    struct Equal {
        static constexpr auto value = (ValueOfType<T>::value == Value);
    };

    template <typename T>
    struct LowerEqual {
        static constexpr auto value = (ValueOfType<T>::value <= Value);
    };

    template <typename T>
    struct Greater {
        static constexpr auto value = (ValueOfType<T>::value > Value);
    };
};

template <typename T>
struct SizeOf {
    static constexpr auto value = sizeof(T);
};

template <std::size_t N>
using WithSizeOf = WithValue<SizeOf, N>;

template <template <typename> typename TypeProjection, template <typename> typename ValueOfType>
struct Projected {
    template <typename T>
    struct Value {
        static constexpr auto value = ValueOfType<typename TypeProjection<T>::type>::value;
    };
};
