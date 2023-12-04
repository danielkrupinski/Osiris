#pragma once

#include <CS2/Classes/CPlantedC4.h>
#include <CS2/Classes/GlobalVars.h>
#include <CS2/Classes/Panorama.h>
#include <CS2/Classes/CUtlVector.h>
#include <CS2/Classes/VMatrix.h>
#include <Utils/ReturnAddress.h>

namespace cs2
{
    struct C_CSGameRules;
    struct ClientModeCSNormal;
    struct CLoopModeGame;
    struct CViewRender;
}

struct ClientPatterns {
    [[nodiscard]] static cs2::CPanel2D** hudScope() noexcept;
    [[nodiscard]] static cs2::CUIEngine** uiEngine() noexcept;
    [[nodiscard]] static cs2::CPanel2D* mainMenuPanel() noexcept;
    [[nodiscard]] static cs2::CPanel2D** hudPanel() noexcept;
    [[nodiscard]] static cs2::CLoopModeGame** loopModeGame() noexcept;
    [[nodiscard]] static cs2::GlobalVars** globalVars() noexcept;
    [[nodiscard]] static cs2::CUtlVector<cs2::CPlantedC4*>* plantedC4s() noexcept;
    [[nodiscard]] static cs2::ClientModeCSNormal* clientMode() noexcept;
    [[nodiscard]] static ReturnAddress getWorldSessionInClientMode() noexcept;
    [[nodiscard]] static cs2::C_CSGameRules** gameRules() noexcept;
    [[nodiscard]] static const void* transformTranslate3dVMT() noexcept;
    [[nodiscard]] static const void* transformScale3dVMT() noexcept;
    [[nodiscard]] static cs2::VMatrix* worldToProjectionMatrix() noexcept;
    [[nodiscard]] static cs2::VMatrix* viewToProjectionMatrix() noexcept;
    [[nodiscard]] static cs2::CViewRender** viewRender() noexcept;
};
