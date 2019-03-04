#include <fstream>
#include <ShlObj.h>

#include "cereal/archives/binary.hpp"

#include "Config.h"

Config::Config(const char* name) noexcept
{
    PWSTR pathToDocuments;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &pathToDocuments))) {
        path = pathToDocuments;
        path /= name;
        CoTaskMemFree(pathToDocuments);
    }
}

void Config::load() noexcept
{
    std::ifstream in{ path };

    if (!in.good())
        return;

    cereal::BinaryInputArchive inputArchive{ in };
    inputArchive(aimbot, triggerbot, glow, chams, visuals, knifeChanger, misc);
    in.close();
}

void Config::save() const noexcept
{
    std::ofstream out{ path };

    if (!out.good())
        return;

    cereal::BinaryOutputArchive outputArchive{ out };
    outputArchive(aimbot, triggerbot, glow, chams, visuals, knifeChanger, misc);
    out.close();
}

void Config::reset() noexcept
{
    aimbot = { };
    triggerbot = { };
    glow = { };
    chams = { };
    visuals = { };
    knifeChanger = { };
    misc = { };
}
