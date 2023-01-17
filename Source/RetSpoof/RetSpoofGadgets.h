#pragma once

#include <cstdint>
#include <optional>

#include <RetSpoof/RetSpoofInvoker.h>

namespace helpers { struct PatternFinder; }

struct RetSpoofGadgets {
    RetSpoofGadgets(const helpers::PatternFinder& clientPatternFinder, const helpers::PatternFinder& enginePatternFinder);

    RetSpoofInvoker client;
    RetSpoofInvoker engine;
};

inline std::optional<RetSpoofGadgets> retSpoofGadgets;
