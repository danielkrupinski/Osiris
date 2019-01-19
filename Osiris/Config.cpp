#include <fstream>
#include <ShlObj.h>

#include "cereal/archives/binary.hpp"
#include "json/json.h"

#include "Config.h"

Config::Config(const std::string_view name)
{
    PWSTR pathToDocuments;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &pathToDocuments))) {
        path = pathToDocuments;
        path /= name;
        CoTaskMemFree(pathToDocuments);
    }
    std::remove("Osiris.json");
    load();
}

void Config::load()
{
    std::ifstream in{ path };

    if (!in.good())
        return;

    cereal::BinaryInputArchive inputArchive{ in };
    inputArchive(aimbot, triggerbot, glow, chams, visuals, misc);
    in.close();
}

void Config::save() const
{
    std::ofstream out{ path };

    if (!out.good())
        return;

    cereal::BinaryOutputArchive outputArchive{ out };
    outputArchive(aimbot, triggerbot, glow, chams, visuals, misc);
    out.close();
}

void Config::reset()
{
    aimbot.enabled = false;
    aimbot.silent = false;
    aimbot.fov = 0.0f;
    aimbot.smooth = 1.0f;

    triggerbot.enabled = false;
    triggerbot.onKey = false;
    triggerbot.key = 0;
    triggerbot.shotDelay = 0;

    glow.enabled = false;
    glow.thickness = 1.0f;
    glow.alpha = 1.0f;
    glow.style = 0;

    glow.allies = false;
    glow.alliesColor[0] = 0.0f;
    glow.alliesColor[1] = 1.0f;
    glow.alliesColor[2] = 0.0f;

    glow.enemies = false;
    glow.enemiesColor[0] = 1.0f;
    glow.enemiesColor[1] = 0.0f;
    glow.enemiesColor[2] = 0.0f;

    glow.weapons = false;
    glow.weaponsColor[0] = 1.0f;
    glow.weaponsColor[1] = 1.0f;
    glow.weaponsColor[2] = 1.0f;

    glow.C4 = false;
    glow.C4Color[0] = 1.0f;
    glow.C4Color[1] = 1.0f;
    glow.C4Color[2] = 0.0f;

    glow.plantedC4 = false;
    glow.plantedC4Color[0] = 1.0f;
    glow.plantedC4Color[1] = 0.27f;
    glow.plantedC4Color[2] = 0.0f;

    glow.chickens = false;
    glow.chickensColor[0] = 1.0f;
    glow.chickensColor[1] = 0.08f;
    glow.chickensColor[2] = 0.58f;

    chams.enabled = false;
    chams.alpha = 1.0f;
    chams.flat = false;
    chams.wireframe = false;

    chams.visibleAllies = false;
    chams.visibleAlliesColor[0] = 0.0f;
    chams.visibleAlliesColor[1] = 1.0f;
    chams.visibleAlliesColor[2] = 0.0f;

    chams.occludedAllies = false;
    chams.occludedAlliesColor[0] = 0.0f;
    chams.occludedAlliesColor[1] = 1.0f;
    chams.occludedAlliesColor[2] = 0.0f;

    chams.visibleEnemies = false;
    chams.visibleEnemiesColor[0] = 1.0f;
    chams.visibleEnemiesColor[1] = 0.0f;
    chams.visibleEnemiesColor[2] = 0.0f;

    chams.occludedEnemies = false;
    chams.occludedEnemiesColor[0] = 1.0f;
    chams.occludedEnemiesColor[1] = 0.0f;
    chams.occludedEnemiesColor[2] = 0.0f;

    misc.bunnyHop = false;
    misc.disablePostProcessing = false;
    misc.flashReduction = 0;
    misc.inverseRagdollGravity = false;
    misc.fastDuck = false;
    misc.noSmoke = false;
    misc.wireframeSmoke = false;
    misc.skybox = 0;
    misc.viewmodelFov = 0;
    misc.worldColor[0] = 0.0f;
    misc.worldColor[1] = 0.0f;
    misc.worldColor[2] = 0.0f;
    misc.autoPistol = false;
};
