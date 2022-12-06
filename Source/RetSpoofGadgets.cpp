#include "RetSpoofGadgets.h"

#include "Memory.h"

RetSpoofGadgets::RetSpoofGadgets(const helpers::PatternFinder& clientPatternFinder, const helpers::PatternFinder& enginePatternFinder)
#if IS_WIN32()
    : client{ clientPatternFinder("\x1B\xFF\x23\xF8\xF6\x87").add(1).get() },
      engine{ enginePatternFinder("\xFF\x23").get() }
#endif
{
}
