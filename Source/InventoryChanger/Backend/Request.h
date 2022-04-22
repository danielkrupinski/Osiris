#pragma once

#include <variant>

#include "RequestTypes.h"

namespace inventory_changer::backend
{
    using Request = std::variant<
        request::ApplySticker,
        request::WearSticker
    >;
}
