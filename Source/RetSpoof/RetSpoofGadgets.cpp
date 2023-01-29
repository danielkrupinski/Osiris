#include "RetSpoofGadgets.h"
#include <Helpers/PatternFinder.h>
#include <MemorySearch/BytePatternLiteral.h>

RetSpoofGadgets::RetSpoofGadgets(const helpers::PatternFinder& clientPatternFinder, const helpers::PatternFinder& enginePatternFinder)
#if IS_WIN32()
    : client{ clientPatternFinder("1B FF 23 F8 F6 87"_pat).add(1).get() },
      engine{ enginePatternFinder("FF 23"_pat).get() }
#endif
{
}
