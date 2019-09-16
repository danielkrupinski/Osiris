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

        if (aimbotJson.isMember("Enabled")) aimbotConfig.enabled = aimbotJson["Enabled"].asBool();
        if (aimbotJson.isMember("On key")) aimbotConfig.onKey = aimbotJson["On key"].asBool();
        if (aimbotJson.isMember("Key")) aimbotConfig.key = aimbotJson["Key"].asInt();
        if (aimbotJson.isMember("Key mode")) aimbotConfig.keyMode = aimbotJson["Key mode"].asInt();
        if (aimbotJson.isMember("Aimlock")) aimbotConfig.aimlock = aimbotJson["Aimlock"].asBool();
        if (aimbotJson.isMember("Silent")) aimbotConfig.silent = aimbotJson["Silent"].asBool();
        if (aimbotJson.isMember("Friendly fire")) aimbotConfig.friendlyFire = aimbotJson["Friendly fire"].asBool();
        if (aimbotJson.isMember("Visible only")) aimbotConfig.visibleOnly = aimbotJson["Visible only"].asBool();
        if (aimbotJson.isMember("Scoped only")) aimbotConfig.scopedOnly = aimbotJson["Scoped only"].asBool();
        if (aimbotJson.isMember("Ignore flash")) aimbotConfig.ignoreFlash = aimbotJson["Ignore flash"].asBool();
        if (aimbotJson.isMember("Ignore smoke")) aimbotConfig.ignoreSmoke = aimbotJson["Ignore smoke"].asBool();
        if (aimbotJson.isMember("Auto shot")) aimbotConfig.autoShot = aimbotJson["Auto shot"].asBool();
        if (aimbotJson.isMember("Auto scope")) aimbotConfig.autoScope = aimbotJson["Auto scope"].asBool();
        if (aimbotJson.isMember("Recoil-based fov")) aimbotConfig.recoilbasedFov = aimbotJson["Recoil-based fov"].asBool();
        if (aimbotJson.isMember("Fov")) aimbotConfig.fov = aimbotJson["Fov"].asFloat();
        if (aimbotJson.isMember("Smooth")) aimbotConfig.smooth = aimbotJson["Smooth"].asFloat();
        if (aimbotJson.isMember("Bone")) aimbotConfig.bone = aimbotJson["Bone"].asInt();
        if (aimbotJson.isMember("Recoil control X")) aimbotConfig.recoilControlX = aimbotJson["Recoil control X"].asFloat();
        if (aimbotJson.isMember("Recoil control Y")) aimbotConfig.recoilControlY = aimbotJson["Recoil control Y"].asFloat();
        if (aimbotJson.isMember("Max aim inaccuracy")) aimbotConfig.maxAimInaccuracy = aimbotJson["Max aim inaccuracy"].asFloat();
        if (aimbotJson.isMember("Max shot inaccuracy")) aimbotConfig.maxShotInaccuracy = aimbotJson["Max shot inaccuracy"].asFloat();
        if (aimbotJson.isMember("Min damage")) aimbotConfig.minDamage = aimbotJson["Min damage"].asInt();
        if (aimbotJson.isMember("Killshot")) aimbotConfig.killshot = aimbotJson["Killshot"].asBool();
        if (aimbotJson.isMember("Between shots")) aimbotConfig.betweenShots = aimbotJson["Between shots"].asBool();
    }

    for (size_t i = 0; i < triggerbot.size(); i++) {
        const auto& triggerbotJson = json["Triggerbot"][i];
        auto& triggerbotConfig = triggerbot[i];

        if (triggerbotJson.isMember("Enabled")) triggerbotConfig.enabled = triggerbotJson["Enabled"].asBool();
        if (triggerbotJson.isMember("On key")) triggerbotConfig.onKey = triggerbotJson["On key"].asBool();
        if (triggerbotJson.isMember("Key")) triggerbotConfig.key = triggerbotJson["Key"].asInt();
        if (triggerbotJson.isMember("Friendly fire")) triggerbotConfig.friendlyFire = triggerbotJson["Friendly fire"].asBool();
        if (triggerbotJson.isMember("Scoped only")) triggerbotConfig.scopedOnly = triggerbotJson["Scoped only"].asBool();
        if (triggerbotJson.isMember("Ignore flash")) triggerbotConfig.ignoreFlash = triggerbotJson["Ignore flash"].asBool();
        if (triggerbotJson.isMember("Ignore smoke")) triggerbotConfig.ignoreSmoke = triggerbotJson["Ignore smoke"].asBool();
        if (triggerbotJson.isMember("Hitgroup")) triggerbotConfig.hitgroup = triggerbotJson["Hitgroup"].asInt();
        if (triggerbotJson.isMember("Shot delay")) triggerbotConfig.shotDelay = triggerbotJson["Shot delay"].asInt();
        if (triggerbotJson.isMember("Min damage")) triggerbotConfig.minDamage = triggerbotJson["Min damage"].asInt();
        if (triggerbotJson.isMember("Killshot")) triggerbotConfig.killshot = triggerbotJson["Killshot"].asBool();
    }

    {
        const auto& backtrackJson = json["Backtrack"];
        if (backtrackJson.isMember("Enabled")) backtrack.enabled = backtrackJson["Enabled"].asBool();
        if (backtrackJson.isMember("Ignore smoke")) backtrack.ignoreSmoke = backtrackJson["Ignore smoke"].asBool();
        if (backtrackJson.isMember("Recoil based fov")) backtrack.recoilBasedFov = backtrackJson["Recoil based fov"].asBool();
        if (backtrackJson.isMember("Time limit")) backtrack.timeLimit = backtrackJson["Time limit"].asInt();
    }

    {
        const auto& antiAimJson = json["Anti aim"];
        if (antiAimJson.isMember("Enabled")) antiAim.enabled = antiAimJson["Enabled"].asBool();
        if (antiAimJson.isMember("Pitch")) antiAim.pitch = antiAimJson["Pitch"].asBool();
        if (antiAimJson.isMember("Pitch angle")) antiAim.pitchAngle = antiAimJson["Pitch angle"].asFloat();
        if (antiAimJson.isMember("Yaw")) antiAim.yaw = antiAimJson["Yaw"].asBool();
    }

    for (size_t i = 0; i < glow.size(); i++) {
        const auto& glowJson = json["glow"][i];
        auto& glowConfig = glow[i];

        if (glowJson.isMember("Enabled")) glowConfig.enabled = glowJson["Enabled"].asBool();
        if (glowJson.isMember("healthBased")) glowConfig.healthBased = glowJson["healthBased"].asBool();
        if (glowJson.isMember("rainbow")) glowConfig.rainbow = glowJson["rainbow"].asBool();
        if (glowJson.isMember("thickness")) glowConfig.thickness = glowJson["thickness"].asFloat();
        if (glowJson.isMember("alpha")) glowConfig.alpha = glowJson["alpha"].asFloat();
        if (glowJson.isMember("style")) glowConfig.style = glowJson["style"].asInt();
        if (glowJson.isMember("color")) {
            glowConfig.color[0] = glowJson["color"][0].asFloat();
            glowConfig.color[1] = glowJson["color"][1].asFloat();
            glowConfig.color[2] = glowJson["color"][2].asFloat();
        }
    }

    for (size_t i = 0; i < chams.size(); i++) {
        const auto& chamsJson = json["chams"][i];
        auto& chamsConfig = chams[i];

        if (chamsJson.isMember("Enabled")) chamsConfig.enabled = chamsJson["Enabled"].asBool();
        if (chamsJson.isMember("healthBased")) chamsConfig.healthBased = chamsJson["healthBased"].asBool();
        if (chamsJson.isMember("rainbow")) chamsConfig.rainbow = chamsJson["rainbow"].asBool();
        if (chamsJson.isMember("blinking")) chamsConfig.blinking = chamsJson["blinking"].asBool();
        if (chamsJson.isMember("material")) chamsConfig.material = chamsJson["material"].asInt();
        if (chamsJson.isMember("wireframe")) chamsConfig.wireframe = chamsJson["wireframe"].asBool();
        if (chamsJson.isMember("color")) {
            chamsConfig.color[0] = chamsJson["color"][0].asFloat();
            chamsConfig.color[1] = chamsJson["color"][1].asFloat();
            chamsConfig.color[2] = chamsJson["color"][2].asFloat();
        }
        if (chamsJson.isMember("alpha")) chamsConfig.alpha = chamsJson["alpha"].asFloat();
    }

    for (size_t i = 0; i < esp.size(); i++) {
        const auto& espJson = json["esp"][i];
        auto& espConfig = esp[i];
        
        if (espJson.isMember("Enabled")) espConfig.enabled = espJson["Enabled"].asBool();
        if (espJson.isMember("Font")) espConfig.font = espJson["Font"].asInt();
        if (espJson.isMember("snaplines")) espConfig.snaplines = espJson["snaplines"].asBool();
        if (espJson.isMember("snaplinesColor")) {
            espConfig.snaplinesColor[0] = espJson["snaplinesColor"][0].asFloat();
            espConfig.snaplinesColor[1] = espJson["snaplinesColor"][1].asFloat();
            espConfig.snaplinesColor[2] = espJson["snaplinesColor"][2].asFloat();
        }
        if (espJson.isMember("Eye traces")) espConfig.eyeTraces = espJson["Eye traces"].asBool();
        if (espJson.isMember("Eye traces color")) {
            espConfig.eyeTracesColor[0] = espJson["Eye traces color"][0].asFloat();
            espConfig.eyeTracesColor[1] = espJson["Eye traces color"][1].asFloat();
            espConfig.eyeTracesColor[2] = espJson["Eye traces color"][2].asFloat();
        }
        if (espJson.isMember("box")) espConfig.box = espJson["box"].asBool();
        if (espJson.isMember("boxColor")) {
            espConfig.boxColor[0] = espJson["boxColor"][0].asFloat();
            espConfig.boxColor[1] = espJson["boxColor"][1].asFloat();
            espConfig.boxColor[2] = espJson["boxColor"][2].asFloat();
        }
        if (espJson.isMember("name")) espConfig.name = espJson["name"].asBool();
        if (espJson.isMember("nameColor")) {
            espConfig.nameColor[0] = espJson["nameColor"][0].asFloat();
            espConfig.nameColor[1] = espJson["nameColor"][1].asFloat();
            espConfig.nameColor[2] = espJson["nameColor"][2].asFloat();
        }
        if (espJson.isMember("health")) espConfig.health = espJson["health"].asBool();
        if (espJson.isMember("healthColor")) {
            espConfig.healthColor[0] = espJson["healthColor"][0].asFloat();
            espConfig.healthColor[1] = espJson["healthColor"][1].asFloat();
            espConfig.healthColor[2] = espJson["healthColor"][2].asFloat();
        }
        if (espJson.isMember("healthBar")) espConfig.healthBar = espJson["healthBar"].asBool();
        if (espJson.isMember("healthBarColor")) {
            espConfig.healthBarColor[0] = espJson["healthBarColor"][0].asFloat();
            espConfig.healthBarColor[1] = espJson["healthBarColor"][1].asFloat();
            espConfig.healthBarColor[2] = espJson["healthBarColor"][2].asFloat();
        }
        if (espJson.isMember("armor")) espConfig.armor = espJson["armor"].asBool();
        if (espJson.isMember("armorColor")) {
            espConfig.armorColor[0] = espJson["armorColor"][0].asFloat();
            espConfig.armorColor[1] = espJson["armorColor"][1].asFloat();
            espConfig.armorColor[2] = espJson["armorColor"][2].asFloat();
        }
        if (espJson.isMember("armorBar")) espConfig.armorBar = espJson["armorBar"].asBool();
        if (espJson.isMember("armorBarColor")) {
            espConfig.armorBarColor[0] = espJson["armorBarColor"][0].asFloat();
            espConfig.armorBarColor[1] = espJson["armorBarColor"][1].asFloat();
            espConfig.armorBarColor[2] = espJson["armorBarColor"][2].asFloat();
        }
        if (espJson.isMember("money")) espConfig.money = espJson["money"].asBool();
        if (espJson.isMember("moneyColor")) {
            espConfig.moneyColor[0] = espJson["moneyColor"][0].asFloat();
            espConfig.moneyColor[1] = espJson["moneyColor"][1].asFloat();
            espConfig.moneyColor[2] = espJson["moneyColor"][2].asFloat();
        }
        if (espJson.isMember("headDot")) espConfig.headDot = espJson["headDot"].asBool();
        if (espJson.isMember("headDotColor")) {
            espConfig.headDotColor[0] = espJson["headDotColor"][0].asFloat();
            espConfig.headDotColor[1] = espJson["headDotColor"][1].asFloat();
            espConfig.headDotColor[2] = espJson["headDotColor"][2].asFloat();
        }
    }

    {
        const auto& visualsJson = json["visuals"];
        if (visualsJson.isMember("disablePostProcessing")) visuals.disablePostProcessing = visualsJson["disablePostProcessing"].asBool();
        if (visualsJson.isMember("inverseRagdollGravity")) visuals.inverseRagdollGravity = visualsJson["inverseRagdollGravity"].asBool();
        if (visualsJson.isMember("noFog")) visuals.noFog = visualsJson["noFog"].asBool();
        if (visualsJson.isMember("no3dSky")) visuals.no3dSky = visualsJson["no3dSky"].asBool();
        if (visualsJson.isMember("No aim punch")) visuals.noAimPunch = visualsJson["No aim punch"].asBool();
        if (visualsJson.isMember("No view punch")) visuals.noViewPunch = visualsJson["No view punch"].asBool();
        if (visualsJson.isMember("noHands")) visuals.noHands = visualsJson["noHands"].asBool();
        if (visualsJson.isMember("noSleeves")) visuals.noSleeves = visualsJson["noSleeves"].asBool();
        if (visualsJson.isMember("noWeapons")) visuals.noWeapons = visualsJson["noWeapons"].asBool();
        if (visualsJson.isMember("noSmoke")) visuals.noSmoke = visualsJson["noSmoke"].asBool();
        if (visualsJson.isMember("noBlur")) visuals.noBlur = visualsJson["noBlur"].asBool();
        if (visualsJson.isMember("noScopeOverlay")) visuals.noScopeOverlay = visualsJson["noScopeOverlay"].asBool();
        if (visualsJson.isMember("noGrass")) visuals.noGrass = visualsJson["noGrass"].asBool();
        if (visualsJson.isMember("noShadows")) visuals.noShadows = visualsJson["noShadows"].asBool();
        if (visualsJson.isMember("wireframeSmoke")) visuals.wireframeSmoke = visualsJson["wireframeSmoke"].asBool();
        if (visualsJson.isMember("Zoom")) visuals.zoom = visualsJson["Zoom"].asBool();
        if (visualsJson.isMember("Zoom key")) visuals.zoomKey = visualsJson["Zoom key"].asInt();
        if (visualsJson.isMember("thirdperson")) visuals.thirdperson = visualsJson["thirdperson"].asBool();
        if (visualsJson.isMember("thirdpersonKey")) visuals.thirdpersonKey = visualsJson["thirdpersonKey"].asInt();
        if (visualsJson.isMember("thirdpersonDistance")) visuals.thirdpersonDistance = visualsJson["thirdpersonDistance"].asInt();
        if (visualsJson.isMember("viewmodelFov")) visuals.viewmodelFov = visualsJson["viewmodelFov"].asInt();
        if (visualsJson.isMember("Fov")) visuals.fov = visualsJson["Fov"].asInt();
        if (visualsJson.isMember("farZ")) visuals.farZ = visualsJson["farZ"].asInt();
        if (visualsJson.isMember("flashReduction")) visuals.flashReduction = visualsJson["flashReduction"].asInt();
        if (visualsJson.isMember("brightness")) visuals.brightness = visualsJson["brightness"].asFloat();
        if (visualsJson.isMember("skybox")) visuals.skybox = visualsJson["skybox"].asInt();
        if (visualsJson.isMember("worldColor")) {
            visuals.worldColor[0] = visualsJson["worldColor"][0].asFloat();
            visuals.worldColor[1] = visualsJson["worldColor"][1].asFloat();
            visuals.worldColor[2] = visualsJson["worldColor"][2].asFloat();
        }
        if (visualsJson.isMember("Deagle spinner")) visuals.deagleSpinner = visualsJson["Deagle spinner"].asBool();
        if (visualsJson.isMember("Screen effect")) visuals.screenEffect = visualsJson["Screen effect"].asInt();
        if (visualsJson.isMember("Hit marker")) visuals.hitMarker = visualsJson["Hit marker"].asInt();
        if (visualsJson.isMember("Hit marker time")) visuals.hitMarkerTime = visualsJson["Hit marker time"].asFloat();
    }

    for (size_t i = 0; i < skinChanger.size(); i++) {
        const auto& skinChangerJson = json["skinChanger"][i];
        auto& skinChangerConfig = skinChanger[i];

        if (skinChangerJson.isMember("Enabled")) skinChangerConfig.enabled = skinChangerJson["Enabled"].asBool();
        if (skinChangerJson.isMember("definition_vector_index")) skinChangerConfig.definition_vector_index = skinChangerJson["definition_vector_index"].asInt();
        if (skinChangerJson.isMember("definition_index")) skinChangerConfig.definition_index = skinChangerJson["definition_index"].asInt();
        if (skinChangerJson.isMember("entity_quality_vector_index")) skinChangerConfig.entity_quality_vector_index = skinChangerJson["entity_quality_vector_index"].asInt();
        if (skinChangerJson.isMember("entity_quality_index")) skinChangerConfig.entity_quality_index = skinChangerJson["entity_quality_index"].asInt();
        if (skinChangerJson.isMember("paint_kit_vector_index")) skinChangerConfig.paint_kit_vector_index = skinChangerJson["paint_kit_vector_index"].asInt();
        if (skinChangerJson.isMember("paint_kit_index")) skinChangerConfig.paint_kit_index = skinChangerJson["paint_kit_index"].asInt();
        if (skinChangerJson.isMember("definition_override_vector_index")) skinChangerConfig.definition_override_vector_index = skinChangerJson["definition_override_vector_index"].asInt();
        if (skinChangerJson.isMember("definition_override_index")) skinChangerConfig.definition_override_index = skinChangerJson["definition_override_index"].asInt();
        if (skinChangerJson.isMember("seed")) skinChangerConfig.seed = skinChangerJson["seed"].asInt();
        if (skinChangerJson.isMember("stat_trak")) skinChangerConfig.stat_trak = skinChangerJson["stat_trak"].asInt();
        if (skinChangerJson.isMember("wear")) skinChangerConfig.wear = skinChangerJson["wear"].asFloat();
        if (skinChangerJson.isMember("custom_name")) strcpy_s(skinChangerConfig.custom_name, 32, skinChangerJson["custom_name"].asCString());

        if (skinChangerJson.isMember("stickers")) {
            for (size_t j = 0; j < skinChangerConfig.stickers.size(); j++) {
                const auto& stickerJson = skinChangerJson["stickers"][j];
                auto& stickerConfig = skinChangerConfig.stickers[j];

                if (stickerJson.isMember("kit")) stickerConfig.kit = stickerJson["kit"].asInt();
                if (stickerJson.isMember("kit_vector_index")) stickerConfig.kit_vector_index = stickerJson["kit_vector_index"].asInt();
                if (stickerJson.isMember("wear")) stickerConfig.wear = stickerJson["wear"].asFloat();
                if (stickerJson.isMember("scale")) stickerConfig.scale = stickerJson["scale"].asFloat();
                if (stickerJson.isMember("rotation")) stickerConfig.rotation = stickerJson["rotation"].asFloat();
            }
        }
    }

    {
        const auto& soundJson = json["Sound"];

        if (soundJson.isMember("Chicken volume")) sound.chickenVolume = soundJson["Chicken volume"].asInt();

        if (soundJson.isMember("Players")) {
            for (size_t i = 0; i < sound.players.size(); i++) {
                const auto& playerJson = soundJson["Players"][i];
                auto& playerConfig = sound.players[i];

                if (playerJson.isMember("Master volume")) playerConfig.masterVolume = playerJson["Master volume"].asInt();
                if (playerJson.isMember("Headshot volume")) playerConfig.headshotVolume = playerJson["Headshot volume"].asInt();
                if (playerJson.isMember("Weapon volume")) playerConfig.weaponVolume = playerJson["Weapon volume"].asInt();
                if (playerJson.isMember("Footstep volume")) playerConfig.footstepVolume = playerJson["Footstep volume"].asInt();
            }
        }
    }


    {
        const auto& styleJson = json["Style"];

        if (styleJson.isMember("Menu style")) style.menuStyle = styleJson["Menu style"].asInt();
        if (styleJson.isMember("Menu colors")) style.menuColors = styleJson["Menu colors"].asInt();

        if (styleJson.isMember("Colors")) {
            const auto& colorsJson = styleJson["Colors"];

            ImGuiStyle& style = ImGui::GetStyle();

            for (int i = 0; i < ImGuiCol_COUNT; i++) {
                if (const char* name = ImGui::GetStyleColorName(i); colorsJson.isMember(name)) {
                    const auto& colorJson = styleJson["Colors"][name];
                    style.Colors[i].x = colorJson[0].asFloat();
                    style.Colors[i].y = colorJson[1].asFloat();
                    style.Colors[i].z = colorJson[2].asFloat();
                    style.Colors[i].w = colorJson[3].asFloat();
                }
            }
        }
    }

    {
        const auto& miscJson = json["Misc"];

        if (miscJson.isMember("Menu key")) misc.menuKey = miscJson["Menu key"].asInt();
        if (miscJson.isMember("Anti AFK kick")) misc.antiAfkKick = miscJson["Anti AFK kick"].asBool();
        if (miscJson.isMember("Auto strafe")) misc.autoStrafe = miscJson["Auto strafe"].asBool();
        if (miscJson.isMember("Bunny hop")) misc.bunnyHop = miscJson["Bunny hop"].asBool();
        if (miscJson.isMember("Custom clan tag")) misc.customClanTag = miscJson["Custom clan tag"].asBool();
        if (miscJson.isMember("Clan tag")) strcpy_s(misc.clanTag, 16, miscJson["Clan tag"].asCString());
        if (miscJson.isMember("Animated clan tag")) misc.animatedClanTag = miscJson["Animated clan tag"].asBool();
        if (miscJson.isMember("Fast duck")) misc.fastDuck = miscJson["Fast duck"].asBool();
        if (miscJson.isMember("Moonwalk")) misc.moonwalk = miscJson["Moonwalk"].asBool();
        if (miscJson.isMember("Sniper crosshair")) misc.sniperCrosshair = miscJson["Sniper crosshair"].asBool();
        if (miscJson.isMember("Recoil crosshair")) misc.recoilCrosshair = miscJson["Recoil crosshair"].asBool();
        if (miscJson.isMember("Auto pistol")) misc.autoPistol = miscJson["Auto pistol"].asBool();
        if (miscJson.isMember("Auto reload")) misc.autoReload = miscJson["Auto reload"].asBool();
        if (miscJson.isMember("Auto accept")) misc.autoAccept = miscJson["Auto accept"].asBool();
        if (miscJson.isMember("Radar hack")) misc.radarHack = miscJson["Radar hack"].asBool();
        if (miscJson.isMember("Reveal ranks")) misc.revealRanks = miscJson["Reveal ranks"].asBool();
        if (miscJson.isMember("Spectator list")) misc.spectatorList = miscJson["Spectator list"].asBool();
        if (miscJson.isMember("Watermark")) misc.watermark = miscJson["Watermark"].asBool();
        if (miscJson.isMember("Fix animation LOD")) misc.fixAnimationLOD = miscJson["Fix animation LOD"].asBool();
        if (miscJson.isMember("Fix bone matrix")) misc.fixBoneMatrix = miscJson["Fix bone matrix"].asBool();
        if (miscJson.isMember("Fix movement")) misc.fixMovement = miscJson["Fix movement"].asBool();
        if (miscJson.isMember("Disable model occlusion")) misc.disableModelOcclusion = miscJson["Disable model occlusion"].asBool();
        if (miscJson.isMember("Kill message")) misc.killMessage = miscJson["Kill message"].asBool();
        if (miscJson.isMember("Kill message string")) strcpy_s(misc.killMessageString, sizeof(misc.killMessageString), miscJson["Kill message string"].asCString());
        if (miscJson.isMember("Name stealer"))  misc.nameStealer = miscJson["Name stealer"].asBool();
        if (miscJson.isMember("Vote text")) strcpy_s(misc.voteText, sizeof(misc.voteText), miscJson["Vote text"].asCString());
        if (miscJson.isMember("Ban color")) misc.banColor = miscJson["Ban color"].asInt();
        if (miscJson.isMember("Ban text")) strcpy_s(misc.banText, sizeof(misc.banText), miscJson["Ban text"].asCString());
        if (miscJson.isMember("Fast plant")) misc.fastPlant = miscJson["Fast plant"].asBool();
        if (miscJson.isMember("Bomb timer")) misc.bombTimer = miscJson["Bomb timer"].asBool();
        if (miscJson.isMember("Quick reload")) misc.quickReload = miscJson["Quick reload"].asBool();
        if (miscJson.isMember("Prepare revolver")) misc.prepareRevolver = miscJson["Prepare revolver"].asBool();
        if (miscJson.isMember("Prepare revolver key")) misc.prepareRevolverKey = miscJson["Prepare revolver key"].asInt();
        if (miscJson.isMember("Hit sound")) misc.hitSound = miscJson["Hit sound"].asInt();
        if (miscJson.isMember("Choked packets")) misc.chokedPackets = miscJson["Choked packets"].asInt();
        if (miscJson.isMember("Choked packets key")) misc.chokedPacketsKey = miscJson["Choked packets key"].asInt();
        if (miscJson.isMember("Max angle delta")) misc.maxAngleDelta = miscJson["Max angle delta"].asFloat();
    }

    {
        const auto& reportbotJson = json["Reportbot"];

        if (reportbotJson.isMember("Enabled")) reportbot.enabled = reportbotJson["Enabled"].asBool();
        if (reportbotJson.isMember("Target")) reportbot.target = reportbotJson["Target"].asInt();
        if (reportbotJson.isMember("Delay")) reportbot.delay = reportbotJson["Delay"].asInt();
        if (reportbotJson.isMember("Aimbot")) reportbot.aimbot = reportbotJson["Aimbot"].asBool();
        if (reportbotJson.isMember("Wallhack")) reportbot.wallhack = reportbotJson["Wallhack"].asBool();
        if (reportbotJson.isMember("Other")) reportbot.other = reportbotJson["Other"].asBool();
        if (reportbotJson.isMember("Griefing")) reportbot.griefing = reportbotJson["Griefing"].asBool();
        if (reportbotJson.isMember("Voice abuse")) reportbot.voiceAbuse = reportbotJson["Voice abuse"].asBool();
        if (reportbotJson.isMember("Text abuse")) reportbot.textAbuse = reportbotJson["Text abuse"].asBool();
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
        aimbotJson["Key mode"] = aimbotConfig.keyMode;
        aimbotJson["Aimlock"] = aimbotConfig.aimlock;
        aimbotJson["Silent"] = aimbotConfig.silent;
        aimbotJson["Friendly fire"] = aimbotConfig.friendlyFire;
        aimbotJson["Visible only"] = aimbotConfig.visibleOnly;
        aimbotJson["Scoped only"] = aimbotConfig.scopedOnly;
        aimbotJson["Ignore flash"] = aimbotConfig.ignoreFlash;;
        aimbotJson["Ignore smoke"] = aimbotConfig.ignoreSmoke;
        aimbotJson["Auto shot"] = aimbotConfig.autoShot;
        aimbotJson["Auto scope"] = aimbotConfig.autoScope;
        aimbotJson["Recoil-based fov"] = aimbotConfig.recoilbasedFov;
        aimbotJson["Fov"] = aimbotConfig.fov;
        aimbotJson["Smooth"] = aimbotConfig.smooth;
        aimbotJson["Bone"] = aimbotConfig.bone;
        aimbotJson["Recoil control X"] = aimbotConfig.recoilControlX;
        aimbotJson["Recoil control Y"] = aimbotConfig.recoilControlY;
        aimbotJson["Max aim inaccuracy"] = aimbotConfig.maxAimInaccuracy;
        aimbotJson["Max shot inaccuracy"] = aimbotConfig.maxShotInaccuracy;
        aimbotJson["Min damage"] = aimbotConfig.minDamage;
        aimbotJson["Killshot"] = aimbotConfig.killshot;
        aimbotJson["Between shots"] = aimbotConfig.betweenShots;
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
        triggerbotJson["Min damage"] = triggerbotConfig.minDamage;
        triggerbotJson["Killshot"] = triggerbotConfig.killshot;
    }

    {
        auto& backtrackJson = json["Backtrack"];
        backtrackJson["Enabled"] = backtrack.enabled;
        backtrackJson["Ignore smoke"] = backtrack.ignoreSmoke;
        backtrackJson["Recoil based fov"] = backtrack.recoilBasedFov;
        backtrackJson["Time limit"] = backtrack.timeLimit;
    }

    {
        auto& antiAimJson = json["Anti aim"];
        antiAimJson["Enabled"] = antiAim.enabled;
        antiAimJson["Pitch"] = antiAim.pitch;
        antiAimJson["Pitch angle"] = antiAim.pitchAngle;
        antiAimJson["Yaw"] = antiAim.yaw;
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
        espJson["Font"] = espConfig.font;
        espJson["snaplines"] = espConfig.snaplines;
        espJson["snaplinesColor"][0] = espConfig.snaplinesColor[0];
        espJson["snaplinesColor"][1] = espConfig.snaplinesColor[1];
        espJson["snaplinesColor"][2] = espConfig.snaplinesColor[2];
        espJson["Eye traces"] = espConfig.eyeTraces;
        espJson["Eye traces color"][0] = espConfig.eyeTracesColor[0];
        espJson["Eye traces color"][1] = espConfig.eyeTracesColor[1];
        espJson["Eye traces color"][2] = espConfig.eyeTracesColor[2];
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
        visualsJson["No aim punch"] = visuals.noAimPunch;
        visualsJson["No view punch"] = visuals.noViewPunch;
        visualsJson["noHands"] = visuals.noHands;
        visualsJson["noSleeves"] = visuals.noSleeves;
        visualsJson["noWeapons"] = visuals.noWeapons;
        visualsJson["noSmoke"] = visuals.noSmoke;
        visualsJson["noBlur"] = visuals.noBlur;
        visualsJson["noScopeOverlay"] = visuals.noScopeOverlay;
        visualsJson["noGrass"] = visuals.noGrass;
        visualsJson["noShadows"] = visuals.noShadows;
        visualsJson["wireframeSmoke"] = visuals.wireframeSmoke;
        visualsJson["Zoom"] = visuals.zoom;
        visualsJson["Zoom key"] = visuals.zoomKey;
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
        visualsJson["Deagle spinner"] = visuals.deagleSpinner;
        visualsJson["Screen effect"] = visuals.screenEffect;
        visualsJson["Hit marker"] = visuals.hitMarker;
        visualsJson["Hit marker time"] = visuals.hitMarkerTime;
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
        auto& soundJson = json["Sound"];

        soundJson["Chicken volume"] = sound.chickenVolume;

        for (size_t i = 0; i < sound.players.size(); i++) {
            auto& playerJson = soundJson["Players"][i];
            const auto& playerConfig = sound.players[i];

            playerJson["Master volume"] = playerConfig.masterVolume;
            playerJson["Headshot volume"] = playerConfig.headshotVolume;
            playerJson["Weapon volume"] = playerConfig.weaponVolume;
            playerJson["Footstep volume"] = playerConfig.footstepVolume;
        }
    }

    {
        auto& styleJson = json["Style"];

        styleJson["Menu style"] = style.menuStyle;
        styleJson["Menu colors"] = style.menuColors;

        auto& colorsJson = styleJson["Colors"];

        const ImGuiStyle& style = ImGui::GetStyle();

        for (int i = 0; i < ImGuiCol_COUNT; i++) {
            auto& colorJson = styleJson["Colors"][ImGui::GetStyleColorName(i)];
            colorJson[0] = style.Colors[i].x;
            colorJson[1] = style.Colors[i].y;
            colorJson[2] = style.Colors[i].z;
            colorJson[3] = style.Colors[i].w;
        }
    }

    {
        auto& miscJson = json["Misc"];
        
        miscJson["Menu key"] = misc.menuKey;
        miscJson["Anti AFK kick"] = misc.antiAfkKick;
        miscJson["Auto strafe"] = misc.autoStrafe;
        miscJson["Bunny hop"] = misc.bunnyHop;
        miscJson["Custom clan tag"] = misc.customClanTag;
        miscJson["Clan tag"] = misc.clanTag;
        miscJson["Animated clan tag"] = misc.animatedClanTag;
        miscJson["Fast duck"] = misc.fastDuck;
        miscJson["Moonwalk"] = misc.moonwalk;
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
        miscJson["Fix movement"] = misc.fixMovement;
        miscJson["Disable model occlusion"] = misc.disableModelOcclusion;
        miscJson["Kill message"] = misc.killMessage;
        miscJson["Kill message string"] = misc.killMessageString;
        miscJson["Name stealer"] = misc.nameStealer;
        miscJson["Vote text"] = misc.voteText;
        miscJson["Ban color"] = misc.banColor;
        miscJson["Ban text"] = misc.banText;
        miscJson["Fast plant"] = misc.fastPlant;
        miscJson["Bomb timer"] = misc.bombTimer;
        miscJson["Quick reload"] = misc.quickReload;
        miscJson["Prepare revolver"] = misc.prepareRevolver;
        miscJson["Prepare revolver key"] = misc.prepareRevolverKey;
        miscJson["Hit sound"] = misc.hitSound;
        miscJson["Choked packets"] = misc.chokedPackets;
        miscJson["Choked packets key"] = misc.chokedPacketsKey;
        miscJson["Max angle delta"] = misc.maxAngleDelta;
    }

    {
        auto& reportbotJson = json["Reportbot"];

        reportbotJson["Enabled"] = reportbot.enabled;
        reportbotJson["Target"] = reportbot.target;
        reportbotJson["Delay"] = reportbot.delay;
        reportbotJson["Aimbot"] = reportbot.aimbot;
        reportbotJson["Wallhack"] = reportbot.wallhack;
        reportbotJson["Other"] = reportbot.other;
        reportbotJson["Griefing"] = reportbot.griefing;
        reportbotJson["Voice abuse"] = reportbot.voiceAbuse;
        reportbotJson["Text abuse"] = reportbot.textAbuse;
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
    sound = { };
    style = { };
    misc = { };
    reportbot = { };
}
