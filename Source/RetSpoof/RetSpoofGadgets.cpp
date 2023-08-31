#include "RetSpoofGadgets.h"
#include <MemorySearch/BytePatternLiteral.h>
#include <MemorySearch/PatternFinder.h>

RetSpoofGadgets::RetSpoofGadgets(const PatternFinder& clientPatternFinder, const PatternFinder& enginePatternFinder)
#if IS_WIN32()
    : client{ clientPatternFinder("1B FF 23 F8 F6 87"_pat).add(1).as<std::uintptr_t>() },
      engine{ enginePatternFinder("FF 23"_pat).as<std::uintptr_t>() }
#endif
{
}
