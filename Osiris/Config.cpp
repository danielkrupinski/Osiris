#include <fstream>
#include <ShlObj.h>

#include "json/json.h"
#include "xorstr.hpp"

#include "Config.h"

Config::Config(const std::string& name)
{
    char buff[MAX_PATH]{ };
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, buff))) {
        path = buff + std::string{ '\\' } + name;
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

    visuals.glow.color[0] = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("color")][0].asFloat();
    visuals.glow.color[1] = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("color")][1].asFloat();
    visuals.glow.color[2] = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("color")][2].asFloat();

    visuals.glow.thickness = config[xorstr_("visuals")][xorstr_("glow")][xorstr_("thickness")].asFloat();

    misc.bunnyHop = config[xorstr_("misc")][xorstr_("bunnyHop")].asBool();
    misc.autoStrafe = config[xorstr_("misc")][xorstr_("autoStrafe")].asBool();

    in.close();
}

void Config::save() const
{
    std::ofstream out{ path };

    if (!out.is_open())
        return;

    Json::Value config;

    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("enabled")] = visuals.glow.enabled;

    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("color")][0] = visuals.glow.color[0];
    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("color")][1] = visuals.glow.color[1];
    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("color")][2] = visuals.glow.color[2];

    config[xorstr_("visuals")][xorstr_("glow")][xorstr_("thickness")] = visuals.glow.thickness;

    config[xorstr_("misc")][xorstr_("bunnyHop")] = misc.bunnyHop;
    config[xorstr_("misc")][xorstr_("autoStrafe")] = misc.autoStrafe;

    out << config;
    out.close();
}
