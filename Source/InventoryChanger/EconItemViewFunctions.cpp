#include "EconItemViewFunctions.h"
#include <MemorySearch/BytePatternLiteral.h>
#include <Platform/Macros/IsPlatform.h>

[[nodiscard]] EconItemViewFunctions createEconItemViewFunctions(const ClientPatternFinder& clientPatternFinder)
{
    return EconItemViewFunctions{
        .clearInventoryImageRGBA = clientPatternFinder.clearInventoryImageRGBA(),
        .getSOCData = clientPatternFinder.getSOCData()
    };
}
