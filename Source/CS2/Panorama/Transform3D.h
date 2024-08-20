#pragma once

#include <CS2/Classes/Vector.h>
#include "CUILength.h"

namespace cs2
{

struct CTransform3D {
    const void* vmt;
};

struct CTransformTranslate3D : CTransform3D {
    CUILength m_x;
    CUILength m_y;
    CUILength m_z;
};

struct CTransformScale3D : CTransform3D {
    Vector m_VecScale;
};

}
