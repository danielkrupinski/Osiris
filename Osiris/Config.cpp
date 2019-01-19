#include <fstream>
#include <ShlObj.h>

#include "cereal/archives/binary.hpp"

#include "Config.h"

Config::Config(const std::string_view name)
{
    PWSTR pathToDocuments;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &pathToDocuments))) {
        path = pathToDocuments;
        std::remove((path / "Osiris.json").string().c_str());
        path /= name;
        CoTaskMemFree(pathToDocuments);
    }
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
    aimbot = { };
    triggerbot = { };
    glow = { };
    chams = { };
    visuals = { };
    misc = { };
}
