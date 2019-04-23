#include <fstream>
#include <ShlObj.h>

#include "ArchiveX.h"

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

    ArchiveX<std::ifstream>{ in } >> aimbot >> triggerbot >> glow >> chams >> esp >> visuals >> knifeChanger >> misc;
    in.close();
}

void Config::save(size_t id) const noexcept
{
    std::ofstream out{ path / configs[id] };

    if (!out.good())
        return;

    ArchiveX<std::ofstream>{ out } << aimbot << triggerbot << glow << chams << esp << visuals << knifeChanger << misc;
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
    esp = { };
    visuals = { };
    knifeChanger = { };
    misc = { };
}
