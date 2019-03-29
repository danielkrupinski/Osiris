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

    if (!std::filesystem::exists(path))
        std::filesystem::create_directory(path);

    for (const auto& p : std::filesystem::directory_iterator(path))
        configs.push_back(p.path().filename().string());
}

void Config::load(size_t id) noexcept
{
    std::ifstream in{ path / configs[id] };

    if (!in.good())
        return;

    cereal::BinaryInputArchive inputArchive{ in };
    try {
        inputArchive(aimbot, triggerbot, glow, chams, visuals, knifeChanger, misc);
    }
    catch (cereal::Exception&) { }
    in.close();
}

void Config::save(size_t id) const noexcept
{
    std::ofstream out{ path / configs[id] };

    if (!out.good())
        return;

    cereal::BinaryOutputArchive outputArchive{ out };
    try {
        outputArchive(aimbot, triggerbot, glow, chams, visuals, knifeChanger, misc);
    }
    catch (cereal::Exception&) { }
    out.close();
}

void Config::add(const char* name) noexcept
{
    if (*name && std::find(std::begin(configs), std::end(configs), name) == std::end(configs))
        configs.emplace_back(name);
}

void Config::remove(size_t id) noexcept
{
    if (std::filesystem::exists(path / configs[id]))
        std::filesystem::remove(path / configs[id]);
    configs.erase(configs.cbegin() + id);
}

void Config::rename(size_t item, const char* newName) noexcept
{
    if (std::filesystem::exists(path / configs[item]))
        std::filesystem::rename(path / configs[item], path / newName);
    configs[item] = newName;
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
