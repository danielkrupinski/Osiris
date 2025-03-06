#pragma once

#include <array>
#include <cstddef>

#include "ConfigParams.h"

struct ConfigStringConversionState {
    config_params::NestingLevelIndexType nestingLevel{0};
    std::array<config_params::ObjectIndexType, config_params::kMaxNestingLevel + 1> indexInNestingLevel{};
    std::size_t offset{0};
};
