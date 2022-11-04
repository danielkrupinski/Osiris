#include <cstdint>
#include <type_traits>

#include <Utils/TypeHint.h>

namespace
{

static_assert(std::is_same_v<TypeHint<std::uintptr_t, void(*)(int)>, std::uintptr_t>, "Type of TypeHint<T, U> should be T");

enum class Enum {};
static_assert(std::is_same_v<TypeHint<int, Enum>, int>, "Hint can be an enum class");

}
