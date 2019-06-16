#include <fstream>
#include <ShlObj.h>

#include "ArchiveX.h"
#include "json/json.h"

#include "Config.h"

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

    Json::Value json;

    for (size_t i = 0; i < aimbot.size(); i++) {
        const auto& aimbotJson = json["aimbot"][i];
        auto& aimbotConfig = aimbot[i];

        aimbotConfig.enabled = aimbotJson["enabled"].asBool();
        aimbotConfig.onKey = aimbotJson["onKey"].asBool();
        aimbotConfig.key = aimbotJson["key"].asInt();
        aimbotConfig.silent = aimbotJson["silent"].asBool();
        aimbotConfig.friendlyFire = aimbotJson["friendlyFire"].asBool();
        aimbotConfig.visibleOnly = aimbotJson["visibleOnly"].asBool();
        aimbotConfig.scopedOnly = aimbotJson["scopedOnly"].asBool();
        aimbotConfig.ignoreFlash = aimbotJson["ignoreFlash"].asBool();
        aimbotConfig.ignoreSmoke = aimbotJson["ignoreSmoke"].asBool();
        aimbotConfig.autoShot = aimbotJson["autoShot"].asBool();
        aimbotConfig.recoilbasedFov = aimbotJson["recoilbasedFov"].asBool();
        aimbotConfig.fov = aimbotJson["fov"].asFloat();
        aimbotConfig.maxAngleDelta = aimbotJson["maxAngleDelta"].asFloat();
        aimbotConfig.smooth = aimbotJson["smooth"].asFloat();
        aimbotConfig.bone = aimbotJson["bone"].asInt();
        aimbotConfig.recoilControlX = aimbotJson["recoilControlX"].asFloat();
        aimbotConfig.recoilControlY = aimbotJson["recoilControlY"].asFloat();
    }

    for (size_t i = 0; i < triggerbot.size(); i++) {
        const auto& triggerbotJson = json["triggerbot"][i];
        auto& triggerbotConfig = triggerbot[i];

        triggerbotConfig.enabled = triggerbotJson["enabled"].asBool();
        triggerbotConfig.onKey = triggerbotJson["onKey"].asBool();
        triggerbotConfig.key = triggerbotJson["key"].asInt();
        triggerbotConfig.friendlyFire = triggerbotJson["friendlyFire"].asBool();
        triggerbotConfig.scopedOnly = triggerbotJson["scopedOnly"].asBool();
        triggerbotConfig.ignoreFlash = triggerbotJson["ignoreFlash"].asBool();
        triggerbotConfig.ignoreSmoke = triggerbotJson["ignoreSmoke"].asBool();
        triggerbotConfig.hitgroup = triggerbotJson["hitgroup"].asInt();
        triggerbotConfig.shotDelay = triggerbotJson["shotDelay"].asInt();
    }

    {
        const auto& backtrackJson = json["backtrack"];
        backtrack.enabled = backtrackJson["enabled"].asBool();
        backtrack.ignoreSmoke = backtrackJson["ignoreSmoke"].asBool();
        backtrack.timeLimit = backtrackJson["timeLimit"].asInt();
    }

    for (size_t i = 0; i < glow.size(); i++) {
        const auto& glowJson = json["glow"][i];
        auto& glowConfig = glow[i];

        glowConfig.enabled = glowJson["enabled"].asBool();
        glowConfig.healthBased = glowJson["healthBased"].asBool();
        glowConfig.rainbow = glowJson["rainbow"].asBool();
        glowConfig.thickness = glowJson["thickness"].asFloat();
        glowConfig.alpha = glowJson["alpha"].asFloat();
        glowConfig.style = glowJson["style"].asInt();
        glowConfig.color[0] = glowJson["color"][0].asFloat();
        glowConfig.color[1] = glowJson["color"][1].asFloat();
        glowConfig.color[2] = glowJson["color"][2].asFloat();
    }

    ArchiveX<std::ifstream>{ in } >> aimbot >> triggerbot >> backtrack >> glow >> chams >> esp >> visuals >> skinChanger >> misc;
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

    Json::Value json;

    for (size_t i = 0; i < aimbot.size(); i++) {
        auto& aimbotJson = json["aimbot"][i];
        const auto& aimbotConfig = aimbot[i];

        aimbotJson["enabled"] = aimbotConfig.enabled;
        aimbotJson["onKey"] = aimbotConfig.onKey;
        aimbotJson["key"] = aimbotConfig.key;
        aimbotJson["silent"] = aimbotConfig.silent;
        aimbotJson["friendlyFire"] = aimbotConfig.friendlyFire;
        aimbotJson["visibleOnly"] = aimbotConfig.visibleOnly;
        aimbotJson["scopedOnly"] = aimbotConfig.scopedOnly;
        aimbotJson["ignoreFlash"] = aimbotConfig.ignoreFlash;;
        aimbotJson["ignoreSmoke"] = aimbotConfig.ignoreSmoke;
        aimbotJson["autoShot"] = aimbotConfig.autoShot;
        aimbotJson["recoilbasedFov"] = aimbotConfig.recoilbasedFov;
        aimbotJson["fov"] = aimbotConfig.fov;
        aimbotJson["maxAngleDelta"] = aimbotConfig.maxAngleDelta;
        aimbotJson["smooth"] = aimbotConfig.smooth;
        aimbotJson["bone"] = aimbotConfig.bone;
        aimbotJson["recoilControlX"] = aimbotConfig.recoilControlX;
        aimbotJson["recoilControlY"] = aimbotConfig.recoilControlY;
    }

    for (size_t i = 0; i < triggerbot.size(); i++) {
        auto& triggerbotJson = json["triggerbot"][i];
        const auto& triggerbotConfig = triggerbot[i];

        triggerbotJson["enabled"] = triggerbotConfig.enabled;
        triggerbotJson["onKey"] = triggerbotConfig.onKey;
        triggerbotJson["key"] = triggerbotConfig.key;
        triggerbotJson["friendlyFire"] = triggerbotConfig.friendlyFire;
        triggerbotJson["scopedOnly"] = triggerbotConfig.scopedOnly;
        triggerbotJson["ignoreFlash"] = triggerbotConfig.ignoreFlash;
        triggerbotJson["ignoreSmoke"] = triggerbotConfig.ignoreSmoke;
        triggerbotJson["hitgroup"] = triggerbotConfig.hitgroup;
        triggerbotJson["shotDelay"] = triggerbotConfig.shotDelay;
    }

    {
        auto& backtrackJson = json["backtrack"];
        backtrackJson["enabled"] = backtrack.enabled;
        backtrackJson["ignoreSmoke"] = backtrack.ignoreSmoke;
        backtrackJson["timeLimit"] = backtrack.timeLimit;
    }

    for (size_t i = 0; i < glow.size(); i++) {
        auto& glowJson = json["glow"][i];
        const auto& glowConfig = glow[i];

        glowJson["enabled"] = glowConfig.enabled;
        glowJson["healthBased"] = glowConfig.healthBased;
        glowJson["rainbow"] = glowConfig.rainbow;
        glowJson["thickness"] = glowConfig.thickness;
        glowJson["alpha"] = glowConfig.alpha;
        glowJson["style"] = glowConfig.style;
        glowJson["color"][0] = glowConfig.color[0];
        glowJson["color"][1] = glowConfig.color[1];
        glowJson["color"][2] = glowConfig.color[2];
    }

    ArchiveX<std::ofstream>{ out } << aimbot << triggerbot << backtrack << glow << chams << esp << visuals << skinChanger << misc;
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
    skinChanger = { };
    misc = { };
}
