#include <fstream>
#include <ShlObj.h>
#include "Interfaces.h"
#include "ArchiveX.h"
#include <string>

#include "Config.h"
#include "SDK/Panel.h"
#include "SDK/InputSystem.h"
#include "SDK/GameUI.h"
#include "SDK/Surface.h"

Config::Config(const char* name) noexcept
{
    PWSTR pathToDocuments;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &pathToDocuments))) {
        path = pathToDocuments;
        path /= name;
        CoTaskMemFree(pathToDocuments);
    }

    if (!std::filesystem::is_directory(path)) {
        std::filesystem::remove(path);
        std::filesystem::create_directory(path);
    }

    std::transform(std::filesystem::directory_iterator{ path },
                   std::filesystem::directory_iterator{ },
                   std::back_inserter(configs),
                   [](const auto& entry) { return entry.path().filename().string(); });
}

void Config::load(size_t id) noexcept
{
    if (!std::filesystem::is_directory(path)) {
        std::filesystem::remove(path);
        std::filesystem::create_directory(path);
    }

    std::ifstream in{ path / configs[id] };

    if (!in.good())
        return;
	int checkver = 0;
	ArchiveX<std::ifstream>{ in } >> checkver >> aimbot >> triggerbot >> backtrack >> glow >> chams >> esp >> visuals >> knifeChanger >> misc;
	if (checkver == version) {
		ArchiveX<std::ifstream>{ in } >> version >> aimbot >> triggerbot >> backtrack >> glow >> chams >> esp >> visuals >> knifeChanger >> misc;
	}
	else {
		reset();
		interfaces.gameUI->messageBox("Config", "Config cannot be loaded due to version mismatch");
	}
	in.close();

}

void Config::save(size_t id) const noexcept
{
    if (!std::filesystem::is_directory(path)) {
        std::filesystem::remove(path);
        std::filesystem::create_directory(path);
    }

    std::ofstream out{ path / configs[id] };

    if (!out.good())
        return;

    ArchiveX<std::ofstream>{ out } << version << aimbot << triggerbot << backtrack << glow << chams << esp << visuals << knifeChanger << misc;
    out.close();
}

void Config::add(const char* name) noexcept
{
    if (*name && std::find(std::cbegin(configs), std::cend(configs), name) == std::cend(configs))
        configs.emplace_back(name);
}

void Config::remove(size_t id) noexcept
{
    std::filesystem::remove(path / configs[id]);
    configs.erase(configs.cbegin() + id);
}

void Config::rename(size_t item, const char* newName) noexcept
{
    std::filesystem::rename(path / configs[item], path / newName);
    configs[item] = newName;
}

void Config::reset() noexcept
{
    aimbot = { };
    triggerbot = { };
    backtrack = { };
    glow = { };
    chams = { };
    esp = { };
    visuals = { };
    knifeChanger = { };
    misc = { };
}
