#include "KeyValuesFactory.h"
#include <MemorySearch/BytePatternLiteral.h>

[[nodiscard]] KeyValuesFactory createKeyValuesFactory(const PatternFinder& clientPatternFinder)
{
    return KeyValuesFactory{
#if IS_WIN32()
        clientPatternFinder("E8 ? ? ? ? 83 C4 04 89 45 D8"_pat).add(1).relativeToAbsolute().get()
#elif IS_LINUX()
        clientPatternFinder("E8 ? ? ? ? 48 89 DF 48 89 45 E0"_pat).add(1).relativeToAbsolute().get()
#endif
    };
}
