#pragma once

#include <CS2/Classes/CGlowProperty.h>
#include "C_BaseEntity.h"

namespace cs2
{

struct C_BaseModelEntity : C_BaseEntity {
    using m_Glow = CGlowProperty;
};

}
