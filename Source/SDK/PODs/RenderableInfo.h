#pragma once

#include <cstddef>
#include <cstdint>

namespace csgo::pod
{

struct RenderableInfo {
    std::uintptr_t renderable;
    std::byte pad[18];
    std::uint16_t flags;
    std::uint16_t flags2;
};

}
