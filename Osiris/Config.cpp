#include <fstream>
#include <ShlObj.h>

#include "json/json.h"
#include "xorstr.hpp"

#include "Config.h"

Config::Config(const std::string& name)
{
    char buff[MAX_PATH]{ };
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, buff))) {
        path = buff + std::string{ '\\' } +name;
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

    visuals.glow.enabled = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("enabled")].asBool();

    visuals.glow.thickness = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("thickness")].asFloat();
    visuals.glow.alpha = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("alpha")].asFloat();
    visuals.glow.style = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("style")].asInt();

    visuals.glow.players = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("players")].asBool();
    visuals.glow.enemiesOnly = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("enemiesOnly")].asBool();

    visuals.glow.alliesColor[0] = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("alliesColor")][0].asFloat();
    visuals.glow.alliesColor[1] = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("alliesColor")][1].asFloat();
    visuals.glow.alliesColor[2] = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("alliesColor")][2].asFloat();

    visuals.glow.enemiesColor[0] = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("enemiesColor")][0].asFloat();
    visuals.glow.enemiesColor[1] = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("enemiesColor")][1].asFloat();
    visuals.glow.enemiesColor[2] = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("enemiesColor")][2].asFloat();

    visuals.glow.weapons = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("weapons")].asBool();
    visuals.glow.weaponsColor[0] = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("weaponsColor")][0].asFloat();
    visuals.glow.weaponsColor[1] = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("weaponsColor")][1].asFloat();
    visuals.glow.weaponsColor[2] = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("weaponsColor")][2].asFloat();

    visuals.glow.plantedC4 = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("plantedC4")].asBool();
    visuals.glow.plantedC4Color[0] = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("plantedC4Color")][0].asFloat();
    visuals.glow.plantedC4Color[1] = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("plantedC4Color")][1].asFloat();
    visuals.glow.plantedC4Color[2] = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("plantedC4Color")][2].asFloat();

    visuals.glow.chickens = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("chickens")].asBool();
    visuals.glow.chickensColor[0] = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("chickensColor")][0].asFloat();
    visuals.glow.chickensColor[1] = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("chickensColor")][1].asFloat();
    visuals.glow.chickensColor[2] = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("chickensColor")][2].asFloat();

    misc.bunnyHop = config[xorstr_("misc")][xorstr_("bunnyHop")].asBool();
    misc.disablePostProcessing = config[xorstr_("misc")][xorstr_("disablePostProcessing")].asBool();
    misc.skybox = config[xorstr_("misc")][xorstr_("skybox")].asString();

    in.close();
}

void Config::save() const
{
    std::ofstream out{ path };

    if (!out.is_open())
        return;

    Json::Value config;

    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("enabled")] = visuals.glow.enabled;

    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("thickness")] = visuals.glow.thickness;
    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("alpha")] = visuals.glow.alpha;
    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("style")] = visuals.glow.style;

    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("players")] = visuals.glow.players;
    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("enemiesOnly")] = visuals.glow.enemiesOnly;

    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("alliesColor")][0] = visuals.glow.alliesColor[0];
    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("alliesColor")][1] = visuals.glow.alliesColor[1];
    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("alliesColor")][2] = visuals.glow.alliesColor[2];

    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("enemiesColor")][0] = visuals.glow.enemiesColor[0];
    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("enemiesColor")][1] = visuals.glow.enemiesColor[1];
    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("enemiesColor")][2] = visuals.glow.enemiesColor[2];

    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("weapons")] = visuals.glow.weapons;
    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("weaponsColor")][0] = visuals.glow.weaponsColor[0];
    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("weaponsColor")][1] = visuals.glow.weaponsColor[1];
    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("weaponsColor")][2] = visuals.glow.weaponsColor[2];

    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("plantedC4")] = visuals.glow.plantedC4;
    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("plantedC4Color")][0] = visuals.glow.plantedC4Color[0];
    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("plantedC4Color")][1] = visuals.glow.plantedC4Color[1];
    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("plantedC4Color")][2] = visuals.glow.plantedC4Color[2];

    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("chickens")] = visuals.glow.chickens;
    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("chickensColor")][0] = visuals.glow.chickensColor[0];
    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("chickensColor")][1] = visuals.glow.chickensColor[1];
    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("chickensColor")][2] = visuals.glow.chickensColor[2];

    config[xorstr_("misc")][xorstr_("bunnyHop")] = misc.bunnyHop;
    config[xorstr_("misc")][xorstr_("disablePostProcessing")] = misc.disablePostProcessing;
    config[xorstr_("misc")][xorstr_("skybox")] = misc.skybox;

    out << config;
    out.close();
}

void Config::reset()
{
    visuals.glow.enabled = false;

    visuals.glow.thickness = 1.0f;
    visuals.glow.alpha = 1.0f;
    visuals.glow.style = 0;

    visuals.glow.players = false;
    visuals.glow.enemiesOnly = false;

    visuals.glow.alliesColor[0] = 0.0f;
    visuals.glow.alliesColor[1] = 1.0f;
    visuals.glow.alliesColor[2] = 0.0f;

    visuals.glow.enemiesColor[0] = 1.0f;
    visuals.glow.enemiesColor[1] = 0.0f;
    visuals.glow.enemiesColor[2] = 0.0f;

    visuals.glow.weapons = false;
    visuals.glow.weaponsColor[0] = 1.0f;
    visuals.glow.weaponsColor[1] = 1.0f;
    visuals.glow.weaponsColor[2] = 1.0f;

    visuals.glow.plantedC4 = false;
    visuals.glow.plantedC4Color[0] = 1.0f;
    visuals.glow.plantedC4Color[1] = 0.5f;
    visuals.glow.plantedC4Color[2] = 0.31f;

    visuals.glow.chickens = false;
    visuals.glow.chickensColor[0] = 1.0f;
    visuals.glow.chickensColor[1] = 0.08f;
    visuals.glow.chickensColor[2] = 0.58f;

    misc.bunnyHop = false;
    misc.disablePostProcessing = false;
    misc.skybox = xorstr_("Default");
}
