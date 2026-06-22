#pragma once

#include <cstdint>

#include "ConfigVariable.h"

namespace lang_vars
{

enum class Language : std::uint8_t {
    English = 0,
    Chinese = 1
};

}

// CONFIG_VARIABLE must be outside the namespace to avoid struct/enum name collision
CONFIG_VARIABLE(Language, lang_vars::Language, lang_vars::Language::English);
