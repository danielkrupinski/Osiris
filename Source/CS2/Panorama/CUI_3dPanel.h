#pragma once

#include <CS2/Classes/CCS_PortraitWorld.h>
#include <CS2/Panorama/CPanel2D.h>

namespace cs2
{

struct CUI_3dPanel : CPanel2D {
    using PortraitWorld = CCS_PortraitWorld*;
    using Fov = float;
    using FovWeight = float;
};

}
