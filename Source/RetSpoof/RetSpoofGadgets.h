#pragma once

#include <cstdint>
#include <optional>

#include <MemorySearch/PatternFinder.h>
#include <RetSpoof/RetSpoofInvoker.h>

struct RetSpoofGadgets {
    RetSpoofGadgets(const PatternFinder& clientPatternFinder, const PatternFinder& enginePatternFinder);

    RetSpoofInvoker client;
    RetSpoofInvoker engine;
};

inline std::optional<RetSpoofGadgets> retSpoofGadgets;
