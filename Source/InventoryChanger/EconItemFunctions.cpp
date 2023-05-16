#include <CSGO/Functions.h>
#include "EconItemFunctions.h"
#include <Platform/Macros/IsPlatform.h>

[[nodiscard]] EconItemFunctions createEconItemFunctions(const ClientPatternFinder& clientPatternFinder)
{
    return EconItemFunctions{
        .setDynamicAttributeValue = clientPatternFinder.setDynamicAttributeValue(),
        .removeDynamicAttribute = clientPatternFinder.removeDynamicAttribute(),
        .setCustomName = clientPatternFinder.setCustomName()
    };
}
