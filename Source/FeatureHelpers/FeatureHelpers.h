#pragma once

#include "GlobalVarsProvider.h"
#include <Helpers/HudProvider.h>
#include <Helpers/PanoramaTransformFactory.h>
#include <Helpers/PlantedC4Provider.h>
#include "Hud/BombTimerHelpers.h"
#include "Hud/DefusingAlertHelpers.h"
#include "Hud/KillfeedPreserverHelpers.h"
#include "Sound/SoundVisualizationHelpers.h"
#include "StylePropertiesSymbols.h"
#include "StylePropertiesVMTs.h"
#include "StylePropertySymbolMap.h"
#include "ViewToProjectionMatrix.h"
#include "Visuals/SniperScopeBlurRemover.h"
#include "WorldToClipSpaceConverter.h"

struct FeatureHelpers {
    explicit FeatureHelpers(const ClientPatterns& clientPatterns, const PanelStylePatterns& panelStylePatterns, const VmtFinder& panoramaVmtFinder) noexcept
        : hudProvider{clientPatterns}
        , globalVarsProvider{clientPatterns}
        , transformFactory{clientPatterns}
        , worldtoClipSpaceConverter{clientPatterns}
        , plantedC4Provider{clientPatterns}
        , viewToProjectionMatrix{clientPatterns}
        , sniperScopeBlurRemover{clientPatterns}
        , stylePropertiesVMTs{panoramaVmtFinder}
        , stylePropertiesSymbols{StylePropertySymbolMap{panelStylePatterns.stylePropertiesSymbols()}}
    {
    }

    [[nodiscard]] SoundVisualizationHelpers getSoundVisualizationHelpers() noexcept
    {
        return SoundVisualizationHelpers{HudInWorldPanelFactory{hudInWorldPanelContainer, hudProvider}, globalVarsProvider, transformFactory, worldtoClipSpaceConverter, viewToProjectionMatrix};
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
    StylePropertiesVMTs stylePropertiesVMTs;
    StylePropertiesSymbols stylePropertiesSymbols;
};
