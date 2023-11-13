#pragma once

#include "GlobalVarsProvider.h"
#include <Helpers/HudProvider.h>
#include <Helpers/PanoramaTransformFactory.h>
#include <Helpers/PlantedC4Provider.h>
#include "Hud/BombTimerHelpers.h"
#include "Hud/DefusingAlertHelpers.h"
#include "Hud/KillfeedPreserverHelpers.h"
#include "Sound/BombPlantVisualizerHelpers.h"
#include "Sound/FootstepVisualizerHelpers.h"
#include "WorldToClipSpaceConverter.h"

struct FeatureHelpers {
    [[nodiscard]] BombPlantVisualizerHelpers getBombPlantVisualizerHelpers() noexcept
    {
        return BombPlantVisualizerHelpers{ HudInWorldPanelFactory{hudProvider}, globalVarsProvider, transformFactory, worldtoClipSpaceConverter };
    }

    [[nodiscard]] FootstepVisualizerHelpers getFootstepVisualizerHelpers() noexcept
    {
        return FootstepVisualizerHelpers{ HudInWorldPanelFactory{hudProvider}, globalVarsProvider, transformFactory, worldtoClipSpaceConverter };
    }

    [[nodiscard]] BombTimerHelpers getBombTimerHelpers() noexcept
    {
        return BombTimerHelpers{ plantedC4Provider, hudProvider, globalVarsProvider };
    }

    [[nodiscard]] DefusingAlertHelpers getDefusingAlertHelpers() noexcept
    {
        return DefusingAlertHelpers{ plantedC4Provider, hudProvider, globalVarsProvider };
    }

    [[nodiscard]] KillfeedPreserverHelpers getKillfeedPreserverHelpers() noexcept
    {
        return KillfeedPreserverHelpers{ hudProvider, globalVarsProvider };
    }

    HudProvider hudProvider;
    GlobalVarsProvider globalVarsProvider;
    PanoramaTransformFactory transformFactory;
    WorldToClipSpaceConverter worldtoClipSpaceConverter;
    PlantedC4Provider plantedC4Provider;
};
