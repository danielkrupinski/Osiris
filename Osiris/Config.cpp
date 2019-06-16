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

    for (size_t i = 0; i < chams.size(); i++) {
        const auto& chamsJson = json["chams"][i];
        auto& chamsConfig = chams[i];

        chamsConfig.enabled = chamsJson["enabled"].asBool();
        chamsConfig.healthBased = chamsJson["healthBased"].asBool();
        chamsConfig.rainbow = chamsJson["rainbow"].asBool();
        chamsConfig.blinking = chamsJson["blinking"].asBool();
        chamsConfig.material = chamsJson["material"].asInt();
        chamsConfig.wireframe = chamsJson["wireframe"].asBool();
        chamsConfig.color[0] = chamsJson["color"][0].asFloat();
        chamsConfig.color[1] = chamsJson["color"][1].asFloat();
        chamsConfig.color[2] = chamsJson["color"][2].asFloat();
        chamsConfig.alpha = chamsJson["alpha"].asFloat();
    }

    for (size_t i = 0; i < esp.size(); i++) {
        const auto& espJson = json["esp"][i];
        auto& espConfig = esp[i];
        
        espConfig.enabled = espJson["enabled"].asBool();
        espConfig.snaplines = espJson["snaplines"].asBool();
        espConfig.snaplinesColor[0] = espJson["snaplinesColor"][0].asFloat();
        espConfig.snaplinesColor[1] = espJson["snaplinesColor"][1].asFloat();
        espConfig.snaplinesColor[2] = espJson["snaplinesColor"][2].asFloat();
        espConfig.box = espJson["box"].asBool();
        espConfig.boxColor[0] = espJson["boxColor"][0].asFloat();
        espConfig.boxColor[1] = espJson["boxColor"][1].asFloat();
        espConfig.boxColor[2] = espJson["boxColor"][2].asFloat();
        espConfig.name = espJson["name"].asBool();
        espConfig.nameColor[0] = espJson["nameColor"][0].asFloat();
        espConfig.nameColor[1] = espJson["nameColor"][1].asFloat();
        espConfig.nameColor[2] = espJson["nameColor"][2].asFloat();
        espConfig.health = espJson["health"].asBool();
        espConfig.healthColor[0] = espJson["healthColor"][0].asFloat();
        espConfig.healthColor[1] = espJson["healthColor"][1].asFloat();
        espConfig.healthColor[2] = espJson["healthColor"][2].asFloat();
        espConfig.healthBar = espJson["healthBar"].asBool();
        espConfig.healthBarColor[0] = espJson["healthBarColor"][0].asFloat();
        espConfig.healthBarColor[1] = espJson["healthBarColor"][1].asFloat();
        espConfig.healthBarColor[2] = espJson["healthBarColor"][2].asFloat();
        espConfig.armor = espJson["armor"].asBool();
        espConfig.armorColor[0] = espJson["armorColor"][0].asFloat();
        espConfig.armorColor[1] = espJson["armorColor"][1].asFloat();
        espConfig.armorColor[2] = espJson["armorColor"][2].asFloat();
        espConfig.armorBar = espJson["armorBar"].asBool();
        espConfig.armorBarColor[0] = espJson["armorBarColor"][0].asFloat();
        espConfig.armorBarColor[1] = espJson["armorBarColor"][1].asFloat();
        espConfig.armorBarColor[2] = espJson["armorBarColor"][2].asFloat();
        espConfig.money = espJson["money"].asBool();
        espConfig.moneyColor[0] = espJson["moneyColor"][0].asFloat();
        espConfig.moneyColor[1] = espJson["moneyColor"][1].asFloat();
        espConfig.moneyColor[2] = espJson["moneyColor"][2].asFloat();
        espConfig.headDot = espJson["headDot"].asBool();
        espConfig.headDotColor[0] = espJson["headDotColor"][0].asFloat();
        espConfig.headDotColor[1] = espJson["headDotColor"][1].asFloat();
        espConfig.headDotColor[2] = espJson["headDotColor"][2].asFloat();
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

    for (size_t i = 0; i < chams.size(); i++) {
        auto& chamsJson = json["chams"][i];
        const auto& chamsConfig = chams[i];

        chamsJson["enabled"] = chamsConfig.enabled;
        chamsJson["healthBased"] = chamsConfig.healthBased;
        chamsJson["rainbow"] = chamsConfig.rainbow;
        chamsJson["blinking"] = chamsConfig.blinking;
        chamsJson["material"] = chamsConfig.material;
        chamsJson["wireframe"] = chamsConfig.wireframe;
        chamsJson["color"][0] = chamsConfig.color[0];
        chamsJson["color"][1] = chamsConfig.color[1];
        chamsJson["color"][2] = chamsConfig.color[2];
        chamsJson["alpha"] = chamsConfig.alpha;
    }

    for (size_t i = 0; i < esp.size(); i++) {
        auto& espJson = json["esp"][i];
        const auto& espConfig = esp[i];

        espJson["enabled"] = espConfig.enabled;
        espJson["snaplines"] = espConfig.snaplines;
        espJson["snaplinesColor"][0] = espConfig.snaplinesColor[0];
        espJson["snaplinesColor"][1] = espConfig.snaplinesColor[1];
        espJson["snaplinesColor"][2] = espConfig.snaplinesColor[2];
        espJson["box"] = espConfig.box;
        espJson["boxColor"][0] = espConfig.boxColor[0];
        espJson["boxColor"][1] = espConfig.boxColor[1];
        espJson["boxColor"][2] = espConfig.boxColor[2];
        espJson["name"] = espConfig.name;
        espJson["nameColor"][0] = espConfig.nameColor[0];
        espJson["nameColor"][1] = espConfig.nameColor[1];
        espJson["nameColor"][2] = espConfig.nameColor[2];
        espJson["health"] = espConfig.health;
        espJson["healthColor"][0] = espConfig.healthColor[0];
        espJson["healthColor"][1] = espConfig.healthColor[1];
        espJson["healthColor"][2] = espConfig.healthColor[2];
        espJson["healthBar"] = espConfig.healthBar;
        espJson["healthBarColor"][0] = espConfig.healthBarColor[0];
        espJson["healthBarColor"][1] = espConfig.healthBarColor[1];
        espJson["healthBarColor"][2] = espConfig.healthBarColor[2];
        espJson["armor"] = espConfig.armor;
        espJson["armorColor"][0] = espConfig.armorColor[0];
        espJson["armorColor"][1] = espConfig.armorColor[1];
        espJson["armorColor"][2] = espConfig.armorColor[2];
        espJson["armorBar"] = espConfig.armorBar;
        espJson["armorBarColor"][0] = espConfig.armorBarColor[0];
        espJson["armorBarColor"][1] = espConfig.armorBarColor[1];
        espJson["armorBarColor"][2] = espConfig.armorBarColor[2];
        espJson["money"] = espConfig.money;
        espJson["moneyColor"][0] = espConfig.moneyColor[0];
        espJson["moneyColor"][1] = espConfig.moneyColor[1];
        espJson["moneyColor"][2] = espConfig.moneyColor[2];
        espJson["headDot"] = espConfig.headDot;
        espJson["headDotColor"][0] = espConfig.headDotColor[0];
        espJson["headDotColor"][1] = espConfig.headDotColor[1];
        espJson["headDotColor"][2] = espConfig.headDotColor[2];
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
