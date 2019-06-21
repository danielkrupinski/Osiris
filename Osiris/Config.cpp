#include <fstream>
#include <ShlObj.h>

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

    in >> json;
    in.close();

    for (size_t i = 0; i < aimbot.size(); i++) {
        const auto& aimbotJson = json["Aimbot"][i];
        auto& aimbotConfig = aimbot[i];

        aimbotConfig.enabled = aimbotJson["Enabled"].asBool();
        aimbotConfig.onKey = aimbotJson["On key"].asBool();
        aimbotConfig.key = aimbotJson["Key"].asInt();
        aimbotConfig.silent = aimbotJson["Silent"].asBool();
        aimbotConfig.friendlyFire = aimbotJson["Friendly fire"].asBool();
        aimbotConfig.visibleOnly = aimbotJson["Visible only"].asBool();
        aimbotConfig.scopedOnly = aimbotJson["Scoped only"].asBool();
        aimbotConfig.ignoreFlash = aimbotJson["Ignore flash"].asBool();
        aimbotConfig.ignoreSmoke = aimbotJson["Ignore smoke"].asBool();
        aimbotConfig.autoShot = aimbotJson["Auto shot"].asBool();
        aimbotConfig.recoilbasedFov = aimbotJson["Recoil-based fov"].asBool();
        aimbotConfig.fov = aimbotJson["Fov"].asFloat();
        aimbotConfig.maxAngleDelta = aimbotJson["Max angle delta"].asFloat();
        aimbotConfig.smooth = aimbotJson["Smooth"].asFloat();
        aimbotConfig.bone = aimbotJson["Bone"].asInt();
        aimbotConfig.recoilControlX = aimbotJson["Recoil control X"].asFloat();
        aimbotConfig.recoilControlY = aimbotJson["Recoil control Y"].asFloat();
    }

    for (size_t i = 0; i < triggerbot.size(); i++) {
        const auto& triggerbotJson = json["Triggerbot"][i];
        auto& triggerbotConfig = triggerbot[i];

        triggerbotConfig.enabled = triggerbotJson["Enabled"].asBool();
        triggerbotConfig.onKey = triggerbotJson["On key"].asBool();
        triggerbotConfig.key = triggerbotJson["Key"].asInt();
        triggerbotConfig.friendlyFire = triggerbotJson["Friendly fire"].asBool();
        triggerbotConfig.scopedOnly = triggerbotJson["Scoped only"].asBool();
        triggerbotConfig.ignoreFlash = triggerbotJson["Ignore flash"].asBool();
        triggerbotConfig.ignoreSmoke = triggerbotJson["Ignore smoke"].asBool();
        triggerbotConfig.hitgroup = triggerbotJson["Hitgroup"].asInt();
        triggerbotConfig.shotDelay = triggerbotJson["Shot delay"].asInt();
    }

    {
        const auto& backtrackJson = json["Backtrack"];
        backtrack.enabled = backtrackJson["Enabled"].asBool();
        backtrack.ignoreSmoke = backtrackJson["Ignore smoke"].asBool();
        backtrack.timeLimit = backtrackJson["Time limit"].asInt();
    }

    for (size_t i = 0; i < glow.size(); i++) {
        const auto& glowJson = json["glow"][i];
        auto& glowConfig = glow[i];

        glowConfig.enabled = glowJson["Enabled"].asBool();
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

        chamsConfig.enabled = chamsJson["Enabled"].asBool();
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
        
        espConfig.enabled = espJson["Enabled"].asBool();
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

    {
        const auto& visualsJson = json["visuals"];
        visuals.disablePostProcessing = visualsJson["disablePostProcessing"].asBool();
        visuals.inverseRagdollGravity = visualsJson["inverseRagdollGravity"].asBool();
        visuals.noFog = visualsJson["noFog"].asBool();
        visuals.no3dSky = visualsJson["no3dSky"].asBool();
        visuals.noVisualRecoil = visualsJson["noVisualRecoil"].asBool();
        visuals.noHands = visualsJson["noHands"].asBool();
        visuals.noSleeves = visualsJson["noSleeves"].asBool();
        visuals.noWeapons = visualsJson["noWeapons"].asBool();
        visuals.noSmoke = visualsJson["noSmoke"].asBool();
        visuals.noBlur = visualsJson["noBlur"].asBool();
        visuals.noScopeOverlay = visualsJson["noScopeOverlay"].asBool();
        visuals.noGrass = visualsJson["noGrass"].asBool();
        visuals.noShadows = visualsJson["noShadows"].asBool();
        visuals.wireframeSmoke = visualsJson["wireframeSmoke"].asBool();
        visuals.thirdperson = visualsJson["thirdperson"].asBool();
        visuals.thirdpersonKey = visualsJson["thirdpersonKey"].asInt();
        visuals.thirdpersonDistance = visualsJson["thirdpersonDistance"].asInt();
        visuals.viewmodelFov = visualsJson["viewmodelFov"].asInt();
        visuals.fov = visualsJson["Fov"].asInt();
        visuals.farZ = visualsJson["farZ"].asInt();
        visuals.flashReduction = visualsJson["flashReduction"].asInt();
        visuals.brightness = visualsJson["brightness"].asFloat();
        visuals.skybox = visualsJson["skybox"].asInt();
        visuals.worldColor[0] = visualsJson["worldColor"][0].asFloat();
        visuals.worldColor[1] = visualsJson["worldColor"][1].asFloat();
        visuals.worldColor[2] = visualsJson["worldColor"][2].asFloat();
    }

    for (size_t i = 0; i < skinChanger.size(); i++) {
        const auto& skinChangerJson = json["skinChanger"][i];
        auto& skinChangerConfig = skinChanger[i];

        skinChangerConfig.enabled = skinChangerJson["Enabled"].asBool();
        skinChangerConfig.definition_vector_index = skinChangerJson["definition_vector_index"].asInt();
        skinChangerConfig.definition_index = skinChangerJson["definition_index"].asInt();
        skinChangerConfig.entity_quality_vector_index = skinChangerJson["entity_quality_vector_index"].asInt();
        skinChangerConfig.entity_quality_index = skinChangerJson["entity_quality_index"].asInt();
        skinChangerConfig.paint_kit_vector_index = skinChangerJson["paint_kit_vector_index"].asInt();
        skinChangerConfig.paint_kit_index = skinChangerJson["paint_kit_index"].asInt();
        skinChangerConfig.definition_override_vector_index = skinChangerJson["definition_override_vector_index"].asInt();
        skinChangerConfig.definition_override_index = skinChangerJson["definition_override_index"].asInt();
        skinChangerConfig.seed = skinChangerJson["seed"].asInt();
        skinChangerConfig.stat_trak = skinChangerJson["stat_trak"].asInt();
        skinChangerConfig.wear = skinChangerJson["wear"].asFloat();
        strcpy_s(skinChangerConfig.custom_name, 32, skinChangerJson["custom_name"].asCString());

        for (size_t j = 0; j < skinChangerConfig.stickers.size(); j++) {
            const auto& stickerJson = skinChangerJson["stickers"][j];
            auto& stickerConfig = skinChangerConfig.stickers[j];

            stickerConfig.kit = stickerJson["kit"].asInt();
            stickerConfig.kit_vector_index = stickerJson["kit_vector_index"].asInt();
            stickerConfig.wear = stickerJson["wear"].asFloat();
            stickerConfig.scale = stickerJson["scale"].asFloat();
            stickerConfig.rotation = stickerJson["rotation"].asFloat();
        }
    }

    {
        const auto& miscJson = json["Misc"];

        misc.menuKey = miscJson["Menu key"].asInt();
        misc.autoStrafe = miscJson["Auto strafe"].asBool();
        misc.bunnyHop = miscJson["Bunny hop"].asBool();
        strcpy_s(misc.clanTag, 16, miscJson["Clan tag"].asCString());
        misc.animatedClanTag = miscJson["Animated clan tag"].asBool();
        misc.fastDuck = miscJson["Fast duck"].asBool();
        misc.sniperCrosshair = miscJson["Sniper crosshair"].asBool();
        misc.recoilCrosshair = miscJson["Recoil crosshair"].asBool();
        misc.autoPistol = miscJson["Auto pistol"].asBool();
        misc.autoReload = miscJson["Auto reload"].asBool();
        misc.autoAccept = miscJson["Auto accept"].asBool();
        misc.radarHack = miscJson["Radar hack"].asBool();
        misc.revealRanks = miscJson["Reveal ranks"].asBool();
        misc.spectatorList = miscJson["Spectator list"].asBool();
        misc.watermark = miscJson["Watermark"].asBool();
        misc.fixAnimationLOD = miscJson["Fix animation LOD"].asBool();
        misc.fixBoneMatrix = miscJson["Fix bone matrix"].asBool();
        misc.disableModelOcclusion = miscJson["Disable model occlusion"].asBool();
        misc.killMessage = miscJson["Kill message"].asBool();
        misc.fastPlant = miscJson["Fast plant"].asBool();
        misc.prepareRevolver = miscJson["Prepare revolver"].asBool();
        misc.prepareRevolverKey = miscJson["Prepare revolver key"].asInt();
        misc.hitSound = miscJson["Hit sound"].asInt();
        misc.chokedPackets = miscJson["Choked packets"].asInt();
        misc.headshotSoundVolume = miscJson["Headshot sound volume"].asInt();
    }
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
        auto& aimbotJson = json["Aimbot"][i];
        const auto& aimbotConfig = aimbot[i];

        aimbotJson["Enabled"] = aimbotConfig.enabled;
        aimbotJson["On key"] = aimbotConfig.onKey;
        aimbotJson["Key"] = aimbotConfig.key;
        aimbotJson["Silent"] = aimbotConfig.silent;
        aimbotJson["Friendly fire"] = aimbotConfig.friendlyFire;
        aimbotJson["Visible only"] = aimbotConfig.visibleOnly;
        aimbotJson["Scoped only"] = aimbotConfig.scopedOnly;
        aimbotJson["Ignore flash"] = aimbotConfig.ignoreFlash;;
        aimbotJson["Ignore smoke"] = aimbotConfig.ignoreSmoke;
        aimbotJson["Auto shot"] = aimbotConfig.autoShot;
        aimbotJson["Recoil-based fov"] = aimbotConfig.recoilbasedFov;
        aimbotJson["Fov"] = aimbotConfig.fov;
        aimbotJson["Max angle delta"] = aimbotConfig.maxAngleDelta;
        aimbotJson["Smooth"] = aimbotConfig.smooth;
        aimbotJson["Bone"] = aimbotConfig.bone;
        aimbotJson["Recoil control X"] = aimbotConfig.recoilControlX;
        aimbotJson["Recoil control Y"] = aimbotConfig.recoilControlY;
    }

    for (size_t i = 0; i < triggerbot.size(); i++) {
        auto& triggerbotJson = json["Triggerbot"][i];
        const auto& triggerbotConfig = triggerbot[i];

        triggerbotJson["Enabled"] = triggerbotConfig.enabled;
        triggerbotJson["On key"] = triggerbotConfig.onKey;
        triggerbotJson["Key"] = triggerbotConfig.key;
        triggerbotJson["Friendly fire"] = triggerbotConfig.friendlyFire;
        triggerbotJson["Scoped only"] = triggerbotConfig.scopedOnly;
        triggerbotJson["Ignore flash"] = triggerbotConfig.ignoreFlash;
        triggerbotJson["Ignore smoke"] = triggerbotConfig.ignoreSmoke;
        triggerbotJson["Hitgroup"] = triggerbotConfig.hitgroup;
        triggerbotJson["Shot delay"] = triggerbotConfig.shotDelay;
    }

    {
        auto& backtrackJson = json["Backtrack"];
        backtrackJson["Enabled"] = backtrack.enabled;
        backtrackJson["Ignore smoke"] = backtrack.ignoreSmoke;
        backtrackJson["Time limit"] = backtrack.timeLimit;
    }

    for (size_t i = 0; i < glow.size(); i++) {
        auto& glowJson = json["glow"][i];
        const auto& glowConfig = glow[i];

        glowJson["Enabled"] = glowConfig.enabled;
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

        chamsJson["Enabled"] = chamsConfig.enabled;
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

        espJson["Enabled"] = espConfig.enabled;
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

    {
        auto& visualsJson = json["visuals"];
        visualsJson["disablePostProcessing"] = visuals.disablePostProcessing;
        visualsJson["inverseRagdollGravity"] = visuals.inverseRagdollGravity;
        visualsJson["noFog"] = visuals.noFog;
        visualsJson["no3dSky"] = visuals.no3dSky;
        visualsJson["noVisualRecoil"] = visuals.noVisualRecoil;
        visualsJson["noHands"] = visuals.noHands;
        visualsJson["noSleeves"] = visuals.noSleeves;
        visualsJson["noWeapons"] = visuals.noWeapons;
        visualsJson["noSmoke"] = visuals.noSmoke;
        visualsJson["noBlur"] = visuals.noBlur;
        visualsJson["noScopeOverlay"] = visuals.noScopeOverlay;
        visualsJson["noGrass"] = visuals.noGrass;
        visualsJson["noShadows"] = visuals.noShadows;
        visualsJson["wireframeSmoke"] = visuals.wireframeSmoke;
        visualsJson["thirdperson"] = visuals.thirdperson;
        visualsJson["thirdpersonKey"] = visuals.thirdpersonKey;
        visualsJson["thirdpersonDistance"] = visuals.thirdpersonDistance;
        visualsJson["viewmodelFov"] = visuals.viewmodelFov;
        visualsJson["Fov"] = visuals.fov;
        visualsJson["farZ"] = visuals.farZ;
        visualsJson["flashReduction"] = visuals.flashReduction;
        visualsJson["brightness"] = visuals.brightness;
        visualsJson["skybox"] = visuals.skybox;
        visualsJson["worldColor"][0] = visuals.worldColor[0];
        visualsJson["worldColor"][1] = visuals.worldColor[1];
        visualsJson["worldColor"][2] = visuals.worldColor[2];
    }

    for (size_t i = 0; i < skinChanger.size(); i++) {
        auto& skinChangerJson = json["skinChanger"][i];
        const auto& skinChangerConfig = skinChanger[i];

        skinChangerJson["Enabled"] = skinChangerConfig.enabled;
        skinChangerJson["definition_vector_index"] = skinChangerConfig.definition_vector_index;
        skinChangerJson["definition_index"] = skinChangerConfig.definition_index;
        skinChangerJson["entity_quality_vector_index"] = skinChangerConfig.entity_quality_vector_index;
        skinChangerJson["entity_quality_index"] = skinChangerConfig.entity_quality_index;
        skinChangerJson["paint_kit_vector_index"] = skinChangerConfig.paint_kit_vector_index;
        skinChangerJson["paint_kit_index"] = skinChangerConfig.paint_kit_index;
        skinChangerJson["definition_override_vector_index"] = skinChangerConfig.definition_override_vector_index;
        skinChangerJson["definition_override_index"] = skinChangerConfig.definition_override_index;
        skinChangerJson["seed"] = skinChangerConfig.seed;
        skinChangerJson["stat_trak"] = skinChangerConfig.stat_trak;
        skinChangerJson["wear"] = skinChangerConfig.wear;
        skinChangerJson["custom_name"] = skinChangerConfig.custom_name;

        for (size_t j = 0; j < skinChangerConfig.stickers.size(); j++) {
            auto& stickerJson = skinChangerJson["stickers"][j];
            const auto& stickerConfig = skinChangerConfig.stickers[j];

            stickerJson["kit"] = stickerConfig.kit;
            stickerJson["kit_vector_index"] = stickerConfig.kit_vector_index;
            stickerJson["wear"] = stickerConfig.wear;
            stickerJson["scale"] = stickerConfig.scale;
            stickerJson["rotation"] = stickerConfig.rotation;
        }
    }

    {
        auto& miscJson = json["Misc"];
        
        miscJson["Menu key"] = misc.menuKey;
        miscJson["Auto strafe"] = misc.autoStrafe;
        miscJson["Bunny hop"] = misc.bunnyHop;
        miscJson["Clan tag"] = misc.clanTag;
        miscJson["Animated clan tag"] = misc.animatedClanTag;
        miscJson["Fast duck"] = misc.fastDuck;
        miscJson["Sniper crosshair"] = misc.sniperCrosshair;
        miscJson["Recoil crosshair"] = misc.recoilCrosshair;
        miscJson["Auto pistol"] = misc.autoPistol;
        miscJson["Auto reload"] = misc.autoReload;
        miscJson["Auto accept"] = misc.autoAccept;
        miscJson["Radar hack"] = misc.radarHack;
        miscJson["Reveal ranks"] = misc.revealRanks;
        miscJson["Spectator list"] = misc.spectatorList;
        miscJson["Watermark"] = misc.watermark;
        miscJson["Fix animation LOD"] = misc.fixAnimationLOD;
        miscJson["Fix bone matrix"] = misc.fixBoneMatrix;
        miscJson["Disable model occlusion"] = misc.disableModelOcclusion;
        miscJson["Kill message"] = misc.killMessage;
        miscJson["Fast plant"] = misc.fastPlant;
        miscJson["Prepare revolver"] = misc.prepareRevolver;
        miscJson["Prepare revolver key"] = misc.prepareRevolverKey;
        miscJson["Hit sound"] = misc.hitSound;
        miscJson["Choked packets"] = misc.chokedPackets;
        miscJson["Headshot sound volume"] = misc.headshotSoundVolume;
    }

    out << json;
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
