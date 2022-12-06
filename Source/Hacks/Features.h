#pragma once

#include "Aimbot.h"
#include "Backtrack.h"
#include "Glow.h"
#include "Visuals.h"
#include "Misc.h"
#include <InventoryChanger/InventoryChanger.h>

#include <Interfaces/OtherInterfaces.h>

struct Features {
    Aimbot aimbot;
    Backtrack backtrack;
    Glow glow;
    Visuals visuals;
    inventory_changer::InventoryChanger inventoryChanger;
    Misc misc;
};

[[nodiscard]] inline Features createFeatures(const Memory& memory, const ClientInterfaces& clientInterfaces, const EngineInterfaces& engineInterfaces, const OtherInterfaces& otherInterface, const helpers::PatternFinder& clientPatternFinder, const helpers::PatternFinder& enginePatternFinder)
{
    return Features{
        .backtrack = Backtrack{ otherInterface.getCvar() },
        .visuals{ memory, otherInterface, clientInterfaces, engineInterfaces, clientPatternFinder, enginePatternFinder },
        .inventoryChanger{ inventory_changer::createInventoryChanger(otherInterface, memory) }
    };
}
