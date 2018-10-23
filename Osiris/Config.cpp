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
    visuals.glow.players = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("players")].asBool();
    visuals.glow.enemiesOnly = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("enemiesOnly")].asBool();

    visuals.glow.alliesColor[0] = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("alliesColor")][0].asFloat();
    visuals.glow.alliesColor[1] = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("alliesColor")][1].asFloat();
    visuals.glow.alliesColor[2] = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("alliesColor")][2].asFloat();

    visuals.glow.enemiesColor[0] = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("enemiesColor")][0].asFloat();
    visuals.glow.enemiesColor[1] = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("enemiesColor")][1].asFloat();
    visuals.glow.enemiesColor[2] = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("enemiesColor")][2].asFloat();

    visuals.glow.thickness = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("thickness")].asFloat();
    visuals.glow.alpha = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("alpha")].asFloat();
    visuals.glow.style = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("style")].asInt();

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
    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("players")] = visuals.glow.players;
    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("enemiesOnly")] = visuals.glow.enemiesOnly;

    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("alliesColor")][0] = visuals.glow.alliesColor[0];
    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("alliesColor")][1] = visuals.glow.alliesColor[1];
    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("alliesColor")][2] = visuals.glow.alliesColor[2];

    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("enemiesColor")][0] = visuals.glow.enemiesColor[0];
    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("enemiesColor")][1] = visuals.glow.enemiesColor[1];
    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("enemiesColor")][2] = visuals.glow.enemiesColor[2];

    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("thickness")] = visuals.glow.thickness;
    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("alpha")] = visuals.glow.alpha;
    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("style")] = visuals.glow.style;

    config[xorstr_("misc")][xorstr_("bunnyHop")] = misc.bunnyHop;
    config[xorstr_("misc")][xorstr_("disablePostProcessing")] = misc.disablePostProcessing;
    config[xorstr_("misc")][xorstr_("skybox")] = misc.skybox;

    out << config;
    out.close();
}

void Config::reset()
{
    visuals.glow.enabled = false;
    visuals.glow.players = false;
    visuals.glow.enemiesOnly = false;

    visuals.glow.alliesColor[0] = 0.0f;
    visuals.glow.alliesColor[1] = 1.0f;
    visuals.glow.alliesColor[2] = 0.0f;

    visuals.glow.enemiesColor[0] = 1.0f;
    visuals.glow.enemiesColor[1] = 0.0f;
    visuals.glow.enemiesColor[2] = 0.0f;

    visuals.glow.thickness = 1.0f;
    visuals.glow.alpha = 1.0f;
    visuals.glow.style = 0;

    misc.bunnyHop = false;
    misc.disablePostProcessing = false;
    misc.skybox = "Default";
}
