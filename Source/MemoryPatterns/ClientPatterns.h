#pragma once

#include <CS2/Classes/CPlantedC4.h>
#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <CS2/Classes/GlobalVars.h>
#include <CS2/Classes/Panorama.h>
#include <CS2/Classes/CUtlVector.h>
#include <CS2/Classes/VMatrix.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>
#include <Utils/ReturnAddress.h>

namespace cs2
{
    struct C_CSGameRules;
    struct ClientModeCSNormal;
    struct CLoopModeGame;
    struct CViewRender;
}

struct ClientPatterns {
    [[nodiscard]] cs2::CUIEngine** uiEngine() const noexcept;
    [[nodiscard]] cs2::CPanel2D** mainMenuPanel() const noexcept;
    [[nodiscard]] cs2::CPanel2D** hudPanel() const noexcept;
    [[nodiscard]] cs2::CLoopModeGame** loopModeGame() const noexcept;
    [[nodiscard]] cs2::GlobalVars** globalVars() const noexcept;
    [[nodiscard]] cs2::CUtlVector<cs2::CPlantedC4*>* plantedC4s() const noexcept;
    [[nodiscard]] cs2::ClientModeCSNormal* clientMode() const noexcept;
    [[nodiscard]] ReturnAddress getWorldSessionInClientMode() const noexcept;
    [[nodiscard]] cs2::C_CSGameRules** gameRules() const noexcept;
    [[nodiscard]] const void* transformTranslate3dVMT() const noexcept;
    [[nodiscard]] const void* transformScale3dVMT() const noexcept;
    [[nodiscard]] cs2::VMatrix* worldToProjectionMatrix() const noexcept;
    [[nodiscard]] cs2::VMatrix* viewToProjectionMatrix() const noexcept;
    [[nodiscard]] cs2::CViewRender** viewRender() const noexcept;
    [[nodiscard]] cs2::CCSPlayerController** localPlayerController() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
