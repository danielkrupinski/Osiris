#pragma once

#include <CS2/Classes/CUtlMap.h>
#include <CS2/Classes/CUtlString.h>
#include "CStyleSymbol.h"

namespace cs2
{

struct CStyleProperty;

struct CPanelStyle {
    using SetProperty = void(CPanelStyle* thisptr, CStyleProperty* styleProperty, bool transition);

    using StylePropertySymbols = CUtlMap<CUtlString, CStyleSymbol>;
};

}
