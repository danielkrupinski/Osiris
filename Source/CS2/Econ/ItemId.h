#pragma once

#include <cstdint>

namespace cs2
{

// itemid_t
enum class ItemId : std::uint64_t {};

static constexpr auto kFauxItemIdMask{0xF000000000000000ull};
static constexpr auto kFauxItemIdPaintKitIndexShift{16};

}
