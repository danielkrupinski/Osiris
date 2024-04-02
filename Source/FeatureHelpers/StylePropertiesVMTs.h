#pragma once

#include <CS2/Classes/Panorama.h>
#include <Platform/VmtFinder.h>

struct StylePropertiesVMTs {
    explicit StylePropertiesVMTs(const VmtFinder& panoramaVmtFinder) noexcept
        : widthPropertyVmt{panoramaVmtFinder.findVmt(cs2::CStylePropertyWidth::kMangledTypeName)}
        , opacityPropertyVmt{panoramaVmtFinder.findVmt(cs2::CStylePropertyOpacity::kMangledTypeName)}
        , zIndexPropertyVmt{panoramaVmtFinder.findVmt(cs2::CStylePropertyZIndex::kMangledTypeName)}
        , heightPropertyVmt{panoramaVmtFinder.findVmt(cs2::CStylePropertyHeight::kMangledTypeName)}
        , imageShadowPropertyVmt{panoramaVmtFinder.findVmt(cs2::CStylePropertyImageShadow::kMangledTypeName)}
        , positionPropertyVmt{panoramaVmtFinder.findVmt(cs2::CStylePropertyPosition::kMangledTypeName)}
        , transformOriginPropertyVmt{panoramaVmtFinder.findVmt(cs2::CStylePropertyTransformOrigin::kMangledTypeName)}
        , alignPropertyVmt{panoramaVmtFinder.findVmt(cs2::CStylePropertyAlign::kMangledTypeName)}
        , washColorVmt{panoramaVmtFinder.findVmt(cs2::CStylePropertyWashColor::kMangledTypeName)}
        , flowChildrenVmt{panoramaVmtFinder.findVmt(cs2::CStylePropertyFlowChildren::kMangledTypeName)}
        , fontVmt{panoramaVmtFinder.findVmt(cs2::CStylePropertyFont::kMangledTypeName)}
        , textShadowVmt{panoramaVmtFinder.findVmt(cs2::CStylePropertyTextShadow::kMangledTypeName)}
        , marginVmt{panoramaVmtFinder.findVmt(cs2::CStylePropertyMargin::kMangledTypeName)}
    {
    }

    const void* widthPropertyVmt;
    const void* opacityPropertyVmt;
    const void* zIndexPropertyVmt;
    const void* heightPropertyVmt;
    const void* imageShadowPropertyVmt;
    const void* positionPropertyVmt;
    const void* transformOriginPropertyVmt;
    const void* alignPropertyVmt;
    const void* washColorVmt;
    const void* flowChildrenVmt;
    const void* fontVmt;
    const void* textShadowVmt;
    const void* marginVmt;
};
