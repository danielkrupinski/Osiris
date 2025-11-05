#pragma once

#include "C_BaseEntity.h"
#include <CS2/Classes/Vector.h>

namespace cs2
{

struct C_CSPlayerResource : C_BaseEntity {
    using m_bombsiteCenterA = Vector;
    using m_bombsiteCenterB = Vector;
};

}
