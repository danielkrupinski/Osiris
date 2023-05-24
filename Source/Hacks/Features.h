#pragma once

#include "Aimbot.h"
#include "Backtrack.h"
#include "Chams.h"
#include "Glow.h"
#include "Visuals.h"
#include "Misc.h"
#include "Sound.h"
#include <InventoryChanger/InventoryChanger.h>

#include <Interfaces/OtherInterfaces.h>

struct Features {
    Aimbot aimbot;
    Backtrack backtrack;
    Chams chams;
    Glow glow;
    Visuals visuals;
    inventory_changer::InventoryChanger inventoryChanger;
    Misc misc;
    Sound sound;
};

[[nodiscard]] inline Features createFeatures(const Memory& memory, const ClientInterfaces& clientInterfaces, const EngineInterfaces& engineInterfaces, const OtherInterfaces& otherInterfaces, const ClientPatternFinder& clientPatternFinder, const EnginePatternFinder& enginePatternFinder, Helpers::RandomGenerator& randomGenerator)
{
    return Features{
        .backtrack = Backtrack{ otherInterfaces.getCvar() },
        .chams{ clientInterfaces, engineInterfaces, otherInterfaces, memory, clientPatternFinder },
        .glow{ clientPatternFinder },
        .visuals{ memory, otherInterfaces, clientInterfaces, engineInterfaces, clientPatternFinder, enginePatternFinder },
        .inventoryChanger{ inventory_changer::createInventoryChanger(engineInterfaces, clientInterfaces, otherInterfaces, memory, clientPatternFinder, randomGenerator) },
        .misc{ clientInterfaces, engineInterfaces, otherInterfaces, memory, clientPatternFinder, enginePatternFinder },
        .sound{ clientInterfaces, memory }
    };
}
