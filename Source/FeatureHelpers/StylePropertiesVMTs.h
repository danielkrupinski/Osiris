#pragma once

#include <CS2/Classes/Panorama.h>
#include <Platform/VmtFinder.h>

struct StylePropertiesVMTs {
    explicit StylePropertiesVMTs(const VmtFinder& panoramaVmtFinder) noexcept
        : widthPropertyVmt{panoramaVmtFinder.findVmt(cs2::CStylePropertyWidth::kMangledTypeName)}
        , opacityPropertyVmt{panoramaVmtFinder.findVmt(cs2::CStylePropertyOpacity::kMangledTypeName)}
    {
    }

    const void* widthPropertyVmt;
    const void* opacityPropertyVmt;
};
