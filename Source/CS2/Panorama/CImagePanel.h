#pragma once

#include <cstddef>
#include <cstdint>
#include <CS2/Classes/CUtlString.h>
#include <Utils/Pad.h>

#include "CPanel2D.h"

namespace cs2
{

enum class SvgAttributeType {
    FillColor = 0,

    Count = 11
};

struct SvgAttribute {
    std::byte value[4];
};

struct ImageProperties {
    PAD(16);
    int textureWidth;
    int textureHeight;
    float scale;
    SvgAttribute svgAttributes[static_cast<std::size_t>(SvgAttributeType::Count)];
    std::uint32_t presentSvgAttributes;
};

struct CImagePanel : CPanel2D {
    using m_strSource = CUtlString;

    using Constructor = void(CImagePanel* thisptr, CPanel2D* parent, const char* id);
    using SetImage = void(CImagePanel* thisptr, const char* imageUrl, const char* defaultImageUrl, ImageProperties* properties);
};

}
