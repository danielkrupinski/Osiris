#include <fstream>
#include <ShlObj.h>

#include "json/json.h"

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

    misc.bunnyHop = config["misc"]["bunnyHop"].asBool();
    misc.autoStrafe = config["misc"]["autoStrafe"].asBool();

    in.close();
}

void Config::save()
{
    std::ofstream out{ path };

    if (!out.is_open())
        return;

    Json::Value config;

    config["misc"]["bunnyHop"] = misc.bunnyHop;
    config["misc"]["autoStrafe"] = misc.autoStrafe;

    out << config;
    out.close();
}
