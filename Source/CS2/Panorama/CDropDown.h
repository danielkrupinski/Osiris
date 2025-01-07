#pragma once

#include "CPanel2D.h"

namespace cs2
{

struct CDropDown : CPanel2D {
    using SetSelectedIndex = void(CDropDown* thisptr, int index);
};

}
