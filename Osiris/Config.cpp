#include <fstream>
#include <ShlObj.h>

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
    load();
}

void Config::load()
{
    std::ifstream in{ path };

    if (!in.is_open())
        return;

    Json::Value config;
    in >> config;

    glow.enabled = config["glow"]["enabled"].asBool();

    glow.thickness = config["glow"]["thickness"].asFloat();
    glow.alpha = config["glow"]["alpha"].asFloat();
    glow.style = config["glow"]["style"].asInt();

    glow.allies = config["glow"]["allies"].asBool();
    glow.alliesColor[0] = config["glow"]["alliesColor"][0].asFloat();
    glow.alliesColor[1] = config["glow"]["alliesColor"][1].asFloat();
    glow.alliesColor[2] = config["glow"]["alliesColor"][2].asFloat();

    glow.enemies = config["glow"]["enemies"].asBool();
    glow.enemiesColor[0] = config["glow"]["enemiesColor"][0].asFloat();
    glow.enemiesColor[1] = config["glow"]["enemiesColor"][1].asFloat();
    glow.enemiesColor[2] = config["glow"]["enemiesColor"][2].asFloat();

    glow.weapons = config["glow"]["weapons"].asBool();
    glow.weaponsColor[0] = config["glow"]["weaponsColor"][0].asFloat();
    glow.weaponsColor[1] = config["glow"]["weaponsColor"][1].asFloat();
    glow.weaponsColor[2] = config["glow"]["weaponsColor"][2].asFloat();

    glow.C4 = config["glow"]["C4"].asBool();
    glow.C4Color[0] = config["glow"]["C4Color"][0].asFloat();
    glow.C4Color[1] = config["glow"]["C4Color"][1].asFloat();
    glow.C4Color[2] = config["glow"]["C4Color"][2].asFloat();

    glow.plantedC4 = config["glow"]["plantedC4"].asBool();
    glow.plantedC4Color[0] = config["glow"]["plantedC4Color"][0].asFloat();
    glow.plantedC4Color[1] = config["glow"]["plantedC4Color"][1].asFloat();
    glow.plantedC4Color[2] = config["glow"]["plantedC4Color"][2].asFloat();

    glow.chickens = config["glow"]["chickens"].asBool();
    glow.chickensColor[0] = config["glow"]["chickensColor"][0].asFloat();
    glow.chickensColor[1] = config["glow"]["chickensColor"][1].asFloat();
    glow.chickensColor[2] = config["glow"]["chickensColor"][2].asFloat();

    chams.enabled = config["chams"]["enabled"].asBool();
    chams.alpha = config["chams"]["alpha"].asFloat();
    chams.wireframe = config["chams"]["wireframe"].asBool();

    chams.allies = config["chams"]["allies"].asBool();
    chams.alliesColor[0] = config["chams"]["alliesColor"][0].asFloat();
    chams.alliesColor[1] = config["chams"]["alliesColor"][1].asFloat();
    chams.alliesColor[2] = config["chams"]["alliesColor"][2].asFloat();

    chams.visibleAllies = config["chams"]["visibleAllies"].asBool();
    chams.visibleAlliesColor[0] = config["chams"]["visibleAlliesColor"][0].asFloat();
    chams.visibleAlliesColor[1] = config["chams"]["visibleAlliesColor"][1].asFloat();
    chams.visibleAlliesColor[2] = config["chams"]["visibleAlliesColor"][2].asFloat();

    chams.enemies = config["chams"]["enemies"].asBool();
    chams.enemiesColor[0] = config["chams"]["enemiesColor"][0].asFloat();
    chams.enemiesColor[1] = config["chams"]["enemiesColor"][1].asFloat();
    chams.enemiesColor[2] = config["chams"]["enemiesColor"][2].asFloat();

    chams.visibleEnemies = config["chams"]["visibleEnemies"].asBool();
    chams.visibleEnemiesColor[0] = config["chams"]["visibleEnemiesColor"][0].asFloat();
    chams.visibleEnemiesColor[1] = config["chams"]["visibleEnemiesColor"][1].asFloat();
    chams.visibleEnemiesColor[2] = config["chams"]["visibleEnemiesColor"][2].asFloat();

    triggerbot.enabled = config["triggerbot"]["enabled"].asBool();
    triggerbot.onKey = config["triggerbot"]["onKey"].asBool();
    triggerbot.shotDelay = config["triggerbot"]["shotDelay"].asInt();

    misc.bunnyHop = config["misc"]["bunnyHop"].asBool();
    misc.clanTag = config["misc"]["clanTag"].asString();
    misc.disablePostProcessing = config["misc"]["disablePostProcessing"].asBool();
    misc.flashReduction = config["misc"]["flashReduction"].asInt();
    misc.inverseRagdollGravity = config["misc"]["inverseRagdollGravity"].asBool();
    misc.noCrouchCooldown = config["misc"]["noCrouchCooldown"].asBool();
    misc.noBlood = config["misc"]["noBlood"].asBool();
    misc.noSmoke = config["misc"]["noSmoke"].asBool();
    misc.wireframeSmoke = config["misc"]["wireframeSmoke"].asBool();
    misc.skybox = config["misc"]["skybox"].asInt();
    misc.viewmodelFov = config["misc"]["viewmodelFov"].asInt();
    misc.worldColor[0] = config["misc"]["worldColor"][0].asFloat();
    misc.worldColor[1] = config["misc"]["worldColor"][1].asFloat();
    misc.worldColor[2] = config["misc"]["worldColor"][2].asFloat();

    in.close();
}

void Config::save() const
{
    std::ofstream out{ path };

    if (!out.is_open())
        return;

    Json::Value config;

    config["glow"]["enabled"] = glow.enabled;

    config["glow"]["thickness"] = glow.thickness;
    config["glow"]["alpha"] = glow.alpha;
    config["glow"]["style"] = glow.style;

    config["glow"]["allies"] = glow.allies;
    config["glow"]["alliesColor"][0] = glow.alliesColor[0];
    config["glow"]["alliesColor"][1] = glow.alliesColor[1];
    config["glow"]["alliesColor"][2] = glow.alliesColor[2];

    config["glow"]["enemies"] = glow.enemies;
    config["glow"]["enemiesColor"][0] = glow.enemiesColor[0];
    config["glow"]["enemiesColor"][1] = glow.enemiesColor[1];
    config["glow"]["enemiesColor"][2] = glow.enemiesColor[2];

    config["glow"]["weapons"] = glow.weapons;
    config["glow"]["weaponsColor"][0] = glow.weaponsColor[0];
    config["glow"]["weaponsColor"][1] = glow.weaponsColor[1];
    config["glow"]["weaponsColor"][2] = glow.weaponsColor[2];

    config["glow"]["C4"] = glow.C4;
    config["glow"]["C4Color"][0] = glow.C4Color[0];
    config["glow"]["C4Color"][1] = glow.C4Color[1];
    config["glow"]["C4Color"][2] = glow.C4Color[2];

    config["glow"]["plantedC4"] = glow.plantedC4;
    config["glow"]["plantedC4Color"][0] = glow.plantedC4Color[0];
    config["glow"]["plantedC4Color"][1] = glow.plantedC4Color[1];
    config["glow"]["plantedC4Color"][2] = glow.plantedC4Color[2];

    config["glow"]["chickens"] = glow.chickens;
    config["glow"]["chickensColor"][0] = glow.chickensColor[0];
    config["glow"]["chickensColor"][1] = glow.chickensColor[1];
    config["glow"]["chickensColor"][2] = glow.chickensColor[2];

    config["chams"]["enabled"] = chams.enabled;
    config["chams"]["alpha"] = chams.alpha;
    config["chams"]["wireframe"] = chams.wireframe;

    config["chams"]["allies"] = chams.allies;
    config["chams"]["alliesColor"][0] = chams.alliesColor[0];
    config["chams"]["alliesColor"][1] = chams.alliesColor[1];
    config["chams"]["alliesColor"][2] = chams.alliesColor[2];

    config["chams"]["visibleAllies"] = chams.visibleAllies;
    config["chams"]["visibleAlliesColor"][0] = chams.visibleAlliesColor[0];
    config["chams"]["visibleAlliesColor"][1] = chams.visibleAlliesColor[1];
    config["chams"]["visibleAlliesColor"][2] = chams.visibleAlliesColor[2];

    config["chams"]["enemies"] = chams.enemies;
    config["chams"]["enemiesColor"][0] = chams.enemiesColor[0];
    config["chams"]["enemiesColor"][1] = chams.enemiesColor[1];
    config["chams"]["enemiesColor"][2] = chams.enemiesColor[2];

    config["chams"]["visibleEnemies"] = chams.visibleEnemies;
    config["chams"]["visibleEnemiesColor"][0] = chams.visibleEnemiesColor[0];
    config["chams"]["visibleEnemiesColor"][1] = chams.visibleEnemiesColor[1];
    config["chams"]["visibleEnemiesColor"][2] = chams.visibleEnemiesColor[2];

    config["triggerbot"]["enabled"] = triggerbot.enabled;
    config["triggerbot"]["onKey"] = triggerbot.onKey;
    config["triggerbot"]["shotDelay"] = triggerbot.shotDelay;

    config["misc"]["bunnyHop"] = misc.bunnyHop;
    config["misc"]["clanTag"] = misc.clanTag;
    config["misc"]["disablePostProcessing"] = misc.disablePostProcessing;
    config["misc"]["flashReduction"] = misc.flashReduction;
    config["misc"]["inverseRagdollGravity"] = misc.inverseRagdollGravity;
    config["misc"]["noCrouchCooldown"] = misc.noCrouchCooldown;
    config["misc"]["noBlood"] = misc.noBlood;
    config["misc"]["noSmoke"] = misc.noSmoke;
    config["misc"]["wireframeSmoke"] = misc.wireframeSmoke;
    config["misc"]["skybox"] = misc.skybox;
    config["misc"]["viewmodelFov"] = misc.viewmodelFov;
    config["misc"]["worldColor"][0] = misc.worldColor[0];
    config["misc"]["worldColor"][1] = misc.worldColor[1];
    config["misc"]["worldColor"][2] = misc.worldColor[2];

    out << config;
    out.close();
}

void Config::reset()
{
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
    chams.wireframe = false;

    chams.allies = false;
    chams.alliesColor[0] = 0.0f;
    chams.alliesColor[1] = 1.0f;
    chams.alliesColor[2] = 0.0f;

    chams.visibleAllies = false;
    chams.visibleAlliesColor[0] = 0.0f;
    chams.visibleAlliesColor[1] = 1.0f;
    chams.visibleAlliesColor[2] = 0.0f;

    chams.enemies = false;
    chams.enemiesColor[0] = 1.0f;
    chams.enemiesColor[1] = 0.0f;
    chams.enemiesColor[2] = 0.0f;

    chams.visibleEnemies = false;
    chams.visibleEnemiesColor[0] = 1.0f;
    chams.visibleEnemiesColor[1] = 0.0f;
    chams.visibleEnemiesColor[2] = 0.0f;

    triggerbot.enabled = false;
    triggerbot.onKey = false;
    triggerbot.shotDelay = 0;

    misc.bunnyHop = false;
    misc.clanTag = "";
    misc.disablePostProcessing = false;
    misc.flashReduction = 0;
    misc.inverseRagdollGravity = false;
    misc.noCrouchCooldown = false;
    misc.noBlood = false;
    misc.noSmoke = false;
    misc.wireframeSmoke = false;
    misc.skybox = 0;
    misc.viewmodelFov = 60;
    misc.worldColor[0] = 0.0f;
    misc.worldColor[1] = 0.0f;
    misc.worldColor[2] = 0.0f;
};
