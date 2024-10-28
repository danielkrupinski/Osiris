#pragma once

#define STRONG_TYPE_ALIAS(name, ...) \
struct name { using type = __VA_ARGS__; };

template <typename StrongTypeAlias>
struct UnpackStrongTypeAlias {
    using type = typename StrongTypeAlias::type;
};

template <typename StrongTypeAlias>
using UnpackStrongTypeAliasT = typename UnpackStrongTypeAlias<StrongTypeAlias>::type;
