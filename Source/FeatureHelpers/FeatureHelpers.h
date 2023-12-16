#pragma once

#include "GlobalVarsProvider.h"
#include <Helpers/HudProvider.h>
#include <Helpers/PanoramaTransformFactory.h>
#include <Helpers/PlantedC4Provider.h>
#include "Hud/BombTimerHelpers.h"
#include "Hud/DefusingAlertHelpers.h"
#include "Hud/KillfeedPreserverHelpers.h"
#include "Sound/SoundVisualizationHelpers.h"
#include "ViewToProjectionMatrix.h"
#include "Visuals/SniperScopeBlurRemover.h"
#include "WorldToClipSpaceConverter.h"

struct FeatureHelpers {
    [[nodiscard]] SoundVisualizationHelpers getSoundVisualizationHelpers() noexcept
    {
        return SoundVisualizationHelpers{ HudInWorldPanelFactory{hudInWorldPanelContainer, hudProvider}, globalVarsProvider, transformFactory, worldtoClipSpaceConverter, viewToProjectionMatrix };
    }

    [[nodiscard]] BombTimerHelpers getBombTimerHelpers() const noexcept
    {
        return BombTimerHelpers{ plantedC4Provider, hudProvider, globalVarsProvider };
    }

    [[nodiscard]] DefusingAlertHelpers getDefusingAlertHelpers() const noexcept
    {
        return DefusingAlertHelpers{ plantedC4Provider, hudProvider, globalVarsProvider };
    }

    [[nodiscard]] KillfeedPreserverHelpers getKillfeedPreserverHelpers() const noexcept
    {
        return KillfeedPreserverHelpers{ hudProvider, globalVarsProvider };
    }

    HudProvider hudProvider;
    GlobalVarsProvider globalVarsProvider;
    PanoramaTransformFactory transformFactory;
    WorldToClipSpaceConverter worldtoClipSpaceConverter;
    PlantedC4Provider plantedC4Provider;
    HudInWorldPanelContainer hudInWorldPanelContainer;
    ViewToProjectionMatrix viewToProjectionMatrix;
    SniperScopeBlurRemover sniperScopeBlurRemover;
};
