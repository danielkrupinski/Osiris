#pragma once

#include <CS2/Classes/CUtlString.h>
#include <Utils/Pad.h>

#include "CPanel2D.h"

namespace cs2
{

struct ImageProperties {
    PAD(16);
    int textureWidth;
    int textureHeight;
    float scale;
};

struct CImagePanel : CPanel2D {
    using m_strSource = CUtlString;

    using Constructor = void(CImagePanel* thisptr, CPanel2D* parent, const char* id);
    using SetImage = void(CImagePanel* thisptr, const char* imageUrl, const char* defaultImageUrl, ImageProperties* properties);
};

}
