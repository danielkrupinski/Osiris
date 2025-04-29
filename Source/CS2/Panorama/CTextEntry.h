#pragma once

#include "CPanel2D.h"

namespace cs2
{

struct CTextEntry : CPanel2D {
    using SetText = void(CTextEntry* thisptr, const char* text);
};

}
