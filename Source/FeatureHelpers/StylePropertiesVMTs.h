#pragma once

#include <CS2/Classes/Panorama.h>
#include <Platform/VmtFinder.h>

struct StylePropertiesVMTs {
    explicit StylePropertiesVMTs(const VmtFinder& panoramaVmtFinder) noexcept
        : widthPropertyVmt{panoramaVmtFinder.findVmt(cs2::CStylePropertyWidth::kMangledTypeName)}
        , opacityPropertyVmt{panoramaVmtFinder.findVmt(cs2::CStylePropertyOpacity::kMangledTypeName)}
        , zIndexPropertyVmt{panoramaVmtFinder.findVmt(cs2::CStylePropertyZIndex::kMangledTypeName)}
        , heightPropertyVmt{panoramaVmtFinder.findVmt(cs2::CStylePropertyHeight::kMangledTypeName)}
    {
    }

    const void* widthPropertyVmt;
    const void* opacityPropertyVmt;
    const void* zIndexPropertyVmt;
    const void* heightPropertyVmt;
};
