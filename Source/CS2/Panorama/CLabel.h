#pragma once

#include "CPanel2D.h"

namespace cs2
{

struct CLabel : CPanel2D {
    using Constructor = void(CLabel* thisptr, CPanel2D* parent, const char* id);
    using setTextInternal = void (*)(CLabel* thisptr, const char* value, int textType, bool trustedSource);
};

}
