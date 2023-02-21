#include "KeyValuesFactory.h"
#include <MemorySearch/BytePatternLiteral.h>

[[nodiscard]] KeyValuesFunctions createKeyValuesFunctions(const PatternFinder& clientPatternFinder)
{
    return KeyValuesFunctions{
#if IS_WIN32()
        .findKey = clientPatternFinder("E8 ? ? ? ? F7 45"_pat).add(1).relativeToAbsolute().as<csgo::KeyValuesFindKey>(),
        .setString = clientPatternFinder("E8 ? ? ? ? 89 77 38"_pat).add(1).relativeToAbsolute().as<csgo::KeyValuesSetString>()
#elif IS_LINUX()
        .findKey = clientPatternFinder("E8 ? ? ? ? 48 85 C0 75 1E"_pat).add(1).relativeToAbsolute().as<csgo::KeyValuesFindKey>(),
        .setString = clientPatternFinder("E8 ? ? ? ? 48 89 DE 4C 89 FF E8 ? ? ? ? 49 8B 04 24"_pat).add(1).relativeToAbsolute().as<csgo::KeyValuesSetString>()
#endif
    };
}

[[nodiscard]] KeyValuesFactory createKeyValuesFactory(RetSpoofInvoker retSpoofInvoker, const PatternFinder& clientPatternFinder)
{
    return KeyValuesFactory{
        retSpoofInvoker,
#if IS_WIN32()
        clientPatternFinder("E8 ? ? ? ? 83 C4 04 89 45 D8"_pat).add(1).relativeToAbsolute().get(),
#elif IS_LINUX()
        clientPatternFinder("E8 ? ? ? ? 48 89 DF 48 89 45 E0"_pat).add(1).relativeToAbsolute().get(),
#endif
        createKeyValuesFunctions(clientPatternFinder)
    };
}
