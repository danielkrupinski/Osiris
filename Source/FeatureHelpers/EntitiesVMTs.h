#pragma once

#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/CCSPlayerController.h>

struct EntitiesVMTs {
    explicit EntitiesVMTs(const VmtFinder& clientVmtFinder) noexcept
        : playerControllerVmt{clientVmtFinder.findVmt(cs2::CCSPlayerController::kMangledTypeName)}
        , playerPawnVmt{clientVmtFinder.findVmt(cs2::C_CSPlayerPawn::kMangledTypeName)}
    {
    }

    [[nodiscard]] bool isPlayerController(const void* vmt) const noexcept
    {
        return playerControllerVmt && vmt == playerControllerVmt;
    }

    [[nodiscard]] bool isPlayerPawn(const void* vmt) const noexcept
    {
        return playerPawnVmt && vmt == playerPawnVmt;
    }

private:
    const void* playerControllerVmt;
    const void* playerPawnVmt;
};
