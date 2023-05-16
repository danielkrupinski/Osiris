#include "KeyValuesFactory.h"
#include <MemorySearch/BytePatternLiteral.h>
#include <BytePatterns/ClientPatternFinder.h>

[[nodiscard]] KeyValuesFunctions createKeyValuesFunctions(const ClientPatternFinder& clientPatternFinder)
{
    return KeyValuesFunctions{
        .findKey = clientPatternFinder.keyValuesFindKey(),
        .setString = clientPatternFinder.keyValuesSetString()
    };
}

[[nodiscard]] KeyValuesFactory createKeyValuesFactory(RetSpoofInvoker retSpoofInvoker, const ClientPatternFinder& clientPatternFinder)
{
    return KeyValuesFactory{
        retSpoofInvoker,
        clientPatternFinder.keyValuesFromString(),
        createKeyValuesFunctions(clientPatternFinder)
    };
}
