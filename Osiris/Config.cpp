#include <fstream>
#include <ShlObj.h>

#include "json/json.h"

#include "Config.h"

Config::Config(const char* name) noexcept
{
    if (PWSTR pathToDocuments; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &pathToDocuments))) {
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
                   [](const auto& entry) { return std::string{ (const char*)entry.path().filename().u8string().c_str() }; });
}

void Config::load(size_t id) noexcept
{
    Json::Value json;

    if (std::ifstream in{ path / (const char8_t*)configs[id].c_str() }; in.good())
        in >> json;
    else
        return;

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
        if (glowJson.isMember("thickness")) glowConfig.thickness = glowJson["thickness"].asFloat();
        if (glowJson.isMember("alpha")) glowConfig.alpha = glowJson["alpha"].asFloat();
        if (glowJson.isMember("style")) glowConfig.style = glowJson["style"].asInt();
        if (glowJson.isMember("Color")) {
            const auto& colorJson = glowJson["Color"];
            auto& colorConfig = glowConfig.color;

            if (colorJson.isMember("Color")) {
                colorConfig.color[0] = colorJson["Color"][0].asFloat();
                colorConfig.color[1] = colorJson["Color"][1].asFloat();
                colorConfig.color[2] = colorJson["Color"][2].asFloat();
            }

            if (colorJson.isMember("Rainbow")) colorConfig.rainbow = colorJson["Rainbow"].asBool();
            if (colorJson.isMember("Rainbow speed")) colorConfig.rainbowSpeed = colorJson["Rainbow speed"].asFloat();
        }
    }

    for (size_t i = 0; i < chams.size(); i++) {
        const auto& chamsJson = json["Chams"][i];
        auto& chamsConfig = chams[i];

        for (size_t j = 0; j < chams[0].materials.size(); j++) {
            const auto& materialsJson = chamsJson[j];
            auto& materialsConfig = chams[i].materials[j];

            if (materialsJson.isMember("Enabled")) materialsConfig.enabled = materialsJson["Enabled"].asBool();
            if (materialsJson.isMember("Health based")) materialsConfig.healthBased = materialsJson["Health based"].asBool();
            if (materialsJson.isMember("Blinking")) materialsConfig.blinking = materialsJson["Blinking"].asBool();
            if (materialsJson.isMember("Material")) materialsConfig.material = materialsJson["Material"].asInt();
            if (materialsJson.isMember("Wireframe")) materialsConfig.wireframe = materialsJson["Wireframe"].asBool();
            if (materialsJson.isMember("Color")) {
                const auto& colorJson = materialsJson["Color"];
                auto& colorConfig = materialsConfig.color;

                if (colorJson.isMember("Color")) {
                    colorConfig.color[0] = colorJson["Color"][0].asFloat();
                    colorConfig.color[1] = colorJson["Color"][1].asFloat();
                    colorConfig.color[2] = colorJson["Color"][2].asFloat();
                }

                if (colorJson.isMember("Rainbow")) colorConfig.rainbow = colorJson["Rainbow"].asBool();
                if (colorJson.isMember("Rainbow speed")) colorConfig.rainbowSpeed = colorJson["Rainbow speed"].asFloat();
            }
            if (materialsJson.isMember("Alpha")) materialsConfig.alpha = materialsJson["Alpha"].asFloat();
        }
    }

    for (size_t i = 0; i < esp.players.size(); i++) {
        const auto& espJson = json["Esp"]["Players"][i];
        auto& espConfig = esp.players[i];
        
        if (espJson.isMember("Enabled")) espConfig.enabled = espJson["Enabled"].asBool();
        if (espJson.isMember("Font")) espConfig.font = espJson["Font"].asInt();

        if (espJson.isMember("Snaplines")) {
            const auto& snaplinesJson = espJson["Snaplines"];
            auto& snaplinesConfig = espConfig.snaplines;

            if (snaplinesJson.isMember("Enabled")) snaplinesConfig.enabled = snaplinesJson["Enabled"].asBool();

            if (snaplinesJson.isMember("Color")) {
                snaplinesConfig.color[0] = snaplinesJson["Color"][0].asFloat();
                snaplinesConfig.color[1] = snaplinesJson["Color"][1].asFloat();
                snaplinesConfig.color[2] = snaplinesJson["Color"][2].asFloat();
            }

            if (snaplinesJson.isMember("Rainbow")) snaplinesConfig.rainbow = snaplinesJson["Rainbow"].asBool();
            if (snaplinesJson.isMember("Rainbow speed")) snaplinesConfig.rainbowSpeed = snaplinesJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Eye traces")) {
            const auto& eyeTracesJson = espJson["Eye traces"];
            auto& eyeTracesConfig = espConfig.eyeTraces;

            if (eyeTracesJson.isMember("Enabled")) eyeTracesConfig.enabled = eyeTracesJson["Enabled"].asBool();

            if (eyeTracesJson.isMember("Color")) {
                eyeTracesConfig.color[0] = eyeTracesJson["Color"][0].asFloat();
                eyeTracesConfig.color[1] = eyeTracesJson["Color"][1].asFloat();
                eyeTracesConfig.color[2] = eyeTracesJson["Color"][2].asFloat();
            }

            if (eyeTracesJson.isMember("Rainbow")) eyeTracesConfig.rainbow = eyeTracesJson["Rainbow"].asBool();
            if (eyeTracesJson.isMember("Rainbow speed")) eyeTracesConfig.rainbowSpeed = eyeTracesJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Box")) {
            const auto& boxJson = espJson["Box"];
            auto& boxConfig = espConfig.box;

            if (boxJson.isMember("Enabled")) boxConfig.enabled = boxJson["Enabled"].asBool();

            if (boxJson.isMember("Color")) {
                boxConfig.color[0] = boxJson["Color"][0].asFloat();
                boxConfig.color[1] = boxJson["Color"][1].asFloat();
                boxConfig.color[2] = boxJson["Color"][2].asFloat();
            }

            if (boxJson.isMember("Rainbow")) boxConfig.rainbow = boxJson["Rainbow"].asBool();
            if (boxJson.isMember("Rainbow speed")) boxConfig.rainbowSpeed = boxJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Box type")) espConfig.boxType = espJson["Box type"].asInt();

        if (espJson.isMember("Name")) {
            const auto& nameJson = espJson["Name"];
            auto& nameConfig = espConfig.name;

            if (nameJson.isMember("Enabled")) nameConfig.enabled = nameJson["Enabled"].asBool();

            if (nameJson.isMember("Color")) {
                nameConfig.color[0] = nameJson["Color"][0].asFloat();
                nameConfig.color[1] = nameJson["Color"][1].asFloat();
                nameConfig.color[2] = nameJson["Color"][2].asFloat();
            }

            if (nameJson.isMember("Rainbow")) nameConfig.rainbow = nameJson["Rainbow"].asBool();
            if (nameJson.isMember("Rainbow speed")) nameConfig.rainbowSpeed = nameJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Health")) {
            const auto& healthJson = espJson["Health"];
            auto& healthConfig = espConfig.health;

            if (healthJson.isMember("Enabled")) healthConfig.enabled = healthJson["Enabled"].asBool();

            if (healthJson.isMember("Color")) {
                healthConfig.color[0] = healthJson["Color"][0].asFloat();
                healthConfig.color[1] = healthJson["Color"][1].asFloat();
                healthConfig.color[2] = healthJson["Color"][2].asFloat();
            }

            if (healthJson.isMember("Rainbow")) healthConfig.rainbow = healthJson["Rainbow"].asBool();
            if (healthJson.isMember("Rainbow speed")) healthConfig.rainbowSpeed = healthJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Health bar")) {
            const auto& healthBarJson = espJson["Health bar"];
            auto& healthBarConfig = espConfig.healthBar;

            if (healthBarJson.isMember("Enabled")) healthBarConfig.enabled = healthBarJson["Enabled"].asBool();

            if (healthBarJson.isMember("Color")) {
                healthBarConfig.color[0] = healthBarJson["Color"][0].asFloat();
                healthBarConfig.color[1] = healthBarJson["Color"][1].asFloat();
                healthBarConfig.color[2] = healthBarJson["Color"][2].asFloat();
            }

            if (healthBarJson.isMember("Rainbow")) healthBarConfig.rainbow = healthBarJson["Rainbow"].asBool();
            if (healthBarJson.isMember("Rainbow speed")) healthBarConfig.rainbowSpeed = healthBarJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Armor")) {
            const auto& armorJson = espJson["Armor"];
            auto& armorConfig = espConfig.armor;

            if (armorJson.isMember("Enabled")) armorConfig.enabled = armorJson["Enabled"].asBool();

            if (armorJson.isMember("Color")) {
                armorConfig.color[0] = armorJson["Color"][0].asFloat();
                armorConfig.color[1] = armorJson["Color"][1].asFloat();
                armorConfig.color[2] = armorJson["Color"][2].asFloat();
            }

            if (armorJson.isMember("Rainbow")) armorConfig.rainbow = armorJson["Rainbow"].asBool();
            if (armorJson.isMember("Rainbow speed")) armorConfig.rainbowSpeed = armorJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Armor bar")) {
            const auto& armorBarJson = espJson["Armor bar"];
            auto& armorBarConfig = espConfig.armorBar;

            if (armorBarJson.isMember("Enabled")) armorBarConfig.enabled = armorBarJson["Enabled"].asBool();

            if (armorBarJson.isMember("Color")) {
                armorBarConfig.color[0] = armorBarJson["Color"][0].asFloat();
                armorBarConfig.color[1] = armorBarJson["Color"][1].asFloat();
                armorBarConfig.color[2] = armorBarJson["Color"][2].asFloat();
            }

            if (armorBarJson.isMember("Rainbow")) armorBarConfig.rainbow = armorBarJson["Rainbow"].asBool();
            if (armorBarJson.isMember("Rainbow speed")) armorBarConfig.rainbowSpeed = armorBarJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Money")) {
            const auto& moneyJson = espJson["Money"];
            auto& moneyConfig = espConfig.money;

            if (moneyJson.isMember("Enabled")) moneyConfig.enabled = moneyJson["Enabled"].asBool();

            if (moneyJson.isMember("Color")) {
                moneyConfig.color[0] = moneyJson["Color"][0].asFloat();
                moneyConfig.color[1] = moneyJson["Color"][1].asFloat();
                moneyConfig.color[2] = moneyJson["Color"][2].asFloat();
            }

            if (moneyJson.isMember("Rainbow")) moneyConfig.rainbow = moneyJson["Rainbow"].asBool();
            if (moneyJson.isMember("Rainbow speed")) moneyConfig.rainbowSpeed = moneyJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Head dot")) {
            const auto& headDotJson = espJson["Head dot"];
            auto& headDotConfig = espConfig.headDot;

            if (headDotJson.isMember("Enabled")) headDotConfig.enabled = headDotJson["Enabled"].asBool();

            if (headDotJson.isMember("Color")) {
                headDotConfig.color[0] = headDotJson["Color"][0].asFloat();
                headDotConfig.color[1] = headDotJson["Color"][1].asFloat();
                headDotConfig.color[2] = headDotJson["Color"][2].asFloat();
            }

            if (headDotJson.isMember("Rainbow")) headDotConfig.rainbow = headDotJson["Rainbow"].asBool();
            if (headDotJson.isMember("Rainbow speed")) headDotConfig.rainbowSpeed = headDotJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Active weapon")) {
            const auto& activeWeaponJson = espJson["Active weapon"];
            auto& activeWeaponConfig = espConfig.activeWeapon;

            if (activeWeaponJson.isMember("Enabled")) activeWeaponConfig.enabled = activeWeaponJson["Enabled"].asBool();

            if (activeWeaponJson.isMember("Color")) {
                activeWeaponConfig.color[0] = activeWeaponJson["Color"][0].asFloat();
                activeWeaponConfig.color[1] = activeWeaponJson["Color"][1].asFloat();
                activeWeaponConfig.color[2] = activeWeaponJson["Color"][2].asFloat();
            }

            if (activeWeaponJson.isMember("Rainbow")) activeWeaponConfig.rainbow = activeWeaponJson["Rainbow"].asBool();
            if (activeWeaponJson.isMember("Rainbow speed")) activeWeaponConfig.rainbowSpeed = activeWeaponJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Outline")) {
            const auto& outlineJson = espJson["Outline"];
            auto& outlineConfig = espConfig.outline;

            if (outlineJson.isMember("Enabled")) outlineConfig.enabled = outlineJson["Enabled"].asBool();

            if (outlineJson.isMember("Color")) {
                outlineConfig.color[0] = outlineJson["Color"][0].asFloat();
                outlineConfig.color[1] = outlineJson["Color"][1].asFloat();
                outlineConfig.color[2] = outlineJson["Color"][2].asFloat();
            }

            if (outlineJson.isMember("Rainbow")) outlineConfig.rainbow = outlineJson["Rainbow"].asBool();
            if (outlineJson.isMember("Rainbow speed")) outlineConfig.rainbowSpeed = outlineJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Distance")) {
            const auto& distanceJson = espJson["Distance"];
            auto& distanceConfig = espConfig.distance;

            if (distanceJson.isMember("Enabled")) distanceConfig.enabled = distanceJson["Enabled"].asBool();

            if (distanceJson.isMember("Color")) {
                distanceConfig.color[0] = distanceJson["Color"][0].asFloat();
                distanceConfig.color[1] = distanceJson["Color"][1].asFloat();
                distanceConfig.color[2] = distanceJson["Color"][2].asFloat();
            }

            if (distanceJson.isMember("Rainbow")) distanceConfig.rainbow = distanceJson["Rainbow"].asBool();
            if (distanceJson.isMember("Rainbow speed")) distanceConfig.rainbowSpeed = distanceJson["Rainbow speed"].asFloat();
        }
        
        if (espJson.isMember("Dead ESP")) espConfig.deadesp = espJson["Dead ESP"].asBool();
        if (espJson.isMember("Max distance")) espConfig.maxDistance = espJson["Max distance"].asFloat();
    }

    {
        const auto& espJson = json["Esp"]["Weapons"];
        auto& espConfig = esp.weapon;

        if (espJson.isMember("Enabled")) espConfig.enabled = espJson["Enabled"].asBool();
        if (espJson.isMember("Font")) espConfig.font = espJson["Font"].asInt();
        if (espJson.isMember("Snaplines")) {
            const auto& snaplinesJson = espJson["Snaplines"];
            auto& snaplinesConfig = espConfig.snaplines;

            if (snaplinesJson.isMember("Enabled")) snaplinesConfig.enabled = snaplinesJson["Enabled"].asBool();

            if (snaplinesJson.isMember("Color")) {
                snaplinesConfig.color[0] = snaplinesJson["Color"][0].asFloat();
                snaplinesConfig.color[1] = snaplinesJson["Color"][1].asFloat();
                snaplinesConfig.color[2] = snaplinesJson["Color"][2].asFloat();
            }

            if (snaplinesJson.isMember("Rainbow")) snaplinesConfig.rainbow = snaplinesJson["Rainbow"].asBool();
            if (snaplinesJson.isMember("Rainbow speed")) snaplinesConfig.rainbowSpeed = snaplinesJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Box")) {
            const auto& boxJson = espJson["Box"];
            auto& boxConfig = espConfig.box;

            if (boxJson.isMember("Enabled")) boxConfig.enabled = boxJson["Enabled"].asBool();

            if (boxJson.isMember("Color")) {
                boxConfig.color[0] = boxJson["Color"][0].asFloat();
                boxConfig.color[1] = boxJson["Color"][1].asFloat();
                boxConfig.color[2] = boxJson["Color"][2].asFloat();
            }

            if (boxJson.isMember("Rainbow")) boxConfig.rainbow = boxJson["Rainbow"].asBool();
            if (boxJson.isMember("Rainbow speed")) boxConfig.rainbowSpeed = boxJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Box type")) espConfig.boxType = espJson["Box type"].asInt();

        if (espJson.isMember("Outline")) {
            const auto& outlineJson = espJson["Outline"];
            auto& outlineConfig = espConfig.outline;

            if (outlineJson.isMember("Enabled")) outlineConfig.enabled = outlineJson["Enabled"].asBool();

            if (outlineJson.isMember("Color")) {
                outlineConfig.color[0] = outlineJson["Color"][0].asFloat();
                outlineConfig.color[1] = outlineJson["Color"][1].asFloat();
                outlineConfig.color[2] = outlineJson["Color"][2].asFloat();
            }

            if (outlineJson.isMember("Rainbow")) outlineConfig.rainbow = outlineJson["Rainbow"].asBool();
            if (outlineJson.isMember("Rainbow speed")) outlineConfig.rainbowSpeed = outlineJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Name")) {
            const auto& nameJson = espJson["Name"];
            auto& nameConfig = espConfig.name;

            if (nameJson.isMember("Enabled")) nameConfig.enabled = nameJson["Enabled"].asBool();

            if (nameJson.isMember("Color")) {
                nameConfig.color[0] = nameJson["Color"][0].asFloat();
                nameConfig.color[1] = nameJson["Color"][1].asFloat();
                nameConfig.color[2] = nameJson["Color"][2].asFloat();
            }

            if (nameJson.isMember("Rainbow")) nameConfig.rainbow = nameJson["Rainbow"].asBool();
            if (nameJson.isMember("Rainbow speed")) nameConfig.rainbowSpeed = nameJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Distance")) {
            const auto& distanceJson = espJson["Distance"];
            auto& distanceConfig = espConfig.distance;

            if (distanceJson.isMember("Enabled")) distanceConfig.enabled = distanceJson["Enabled"].asBool();

            if (distanceJson.isMember("Color")) {
                distanceConfig.color[0] = distanceJson["Color"][0].asFloat();
                distanceConfig.color[1] = distanceJson["Color"][1].asFloat();
                distanceConfig.color[2] = distanceJson["Color"][2].asFloat();
            }

            if (distanceJson.isMember("Rainbow")) distanceConfig.rainbow = distanceJson["Rainbow"].asBool();
            if (distanceJson.isMember("Rainbow speed")) distanceConfig.rainbowSpeed = distanceJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Max distance")) espConfig.maxDistance = espJson["Max distance"].asFloat();
    }

    for (size_t i = 0; i < esp.dangerZone.size(); i++) {
        const auto& espJson = json["Esp"]["Danger Zone"][i];
        auto& espConfig = esp.dangerZone[i];

        if (espJson.isMember("Enabled")) espConfig.enabled = espJson["Enabled"].asBool();
        if (espJson.isMember("Font")) espConfig.font = espJson["Font"].asInt();
        if (espJson.isMember("Snaplines")) {
            const auto& snaplinesJson = espJson["Snaplines"];
            auto& snaplinesConfig = espConfig.snaplines;

            if (snaplinesJson.isMember("Enabled")) snaplinesConfig.enabled = snaplinesJson["Enabled"].asBool();

            if (snaplinesJson.isMember("Color")) {
                snaplinesConfig.color[0] = snaplinesJson["Color"][0].asFloat();
                snaplinesConfig.color[1] = snaplinesJson["Color"][1].asFloat();
                snaplinesConfig.color[2] = snaplinesJson["Color"][2].asFloat();
            }

            if (snaplinesJson.isMember("Rainbow")) snaplinesConfig.rainbow = snaplinesJson["Rainbow"].asBool();
            if (snaplinesJson.isMember("Rainbow speed")) snaplinesConfig.rainbowSpeed = snaplinesJson["Rainbow speed"].asFloat();
        }
        
        if (espJson.isMember("Box")) {
            const auto& boxJson = espJson["Box"];
            auto& boxConfig = espConfig.box;

            if (boxJson.isMember("Enabled")) boxConfig.enabled = boxJson["Enabled"].asBool();

            if (boxJson.isMember("Color")) {
                boxConfig.color[0] = boxJson["Color"][0].asFloat();
                boxConfig.color[1] = boxJson["Color"][1].asFloat();
                boxConfig.color[2] = boxJson["Color"][2].asFloat();
            }

            if (boxJson.isMember("Rainbow")) boxConfig.rainbow = boxJson["Rainbow"].asBool();
            if (boxJson.isMember("Rainbow speed")) boxConfig.rainbowSpeed = boxJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Box type")) espConfig.boxType = espJson["Box type"].asInt();

        if (espJson.isMember("Outline")) {
            const auto& outlineJson = espJson["Outline"];
            auto& outlineConfig = espConfig.outline;

            if (outlineJson.isMember("Enabled")) outlineConfig.enabled = outlineJson["Enabled"].asBool();

            if (outlineJson.isMember("Color")) {
                outlineConfig.color[0] = outlineJson["Color"][0].asFloat();
                outlineConfig.color[1] = outlineJson["Color"][1].asFloat();
                outlineConfig.color[2] = outlineJson["Color"][2].asFloat();
            }

            if (outlineJson.isMember("Rainbow")) outlineConfig.rainbow = outlineJson["Rainbow"].asBool();
            if (outlineJson.isMember("Rainbow speed")) outlineConfig.rainbowSpeed = outlineJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Name")) {
            const auto& nameJson = espJson["Name"];
            auto& nameConfig = espConfig.name;

            if (nameJson.isMember("Enabled")) nameConfig.enabled = nameJson["Enabled"].asBool();

            if (nameJson.isMember("Color")) {
                nameConfig.color[0] = nameJson["Color"][0].asFloat();
                nameConfig.color[1] = nameJson["Color"][1].asFloat();
                nameConfig.color[2] = nameJson["Color"][2].asFloat();
            }

            if (nameJson.isMember("Rainbow")) nameConfig.rainbow = nameJson["Rainbow"].asBool();
            if (nameJson.isMember("Rainbow speed")) nameConfig.rainbowSpeed = nameJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Distance")) {
            const auto& distanceJson = espJson["Distance"];
            auto& distanceConfig = espConfig.distance;

            if (distanceJson.isMember("Enabled")) distanceConfig.enabled = distanceJson["Enabled"].asBool();

            if (distanceJson.isMember("Color")) {
                distanceConfig.color[0] = distanceJson["Color"][0].asFloat();
                distanceConfig.color[1] = distanceJson["Color"][1].asFloat();
                distanceConfig.color[2] = distanceJson["Color"][2].asFloat();
            }

            if (distanceJson.isMember("Rainbow")) distanceConfig.rainbow = distanceJson["Rainbow"].asBool();
            if (distanceJson.isMember("Rainbow speed")) distanceConfig.rainbowSpeed = distanceJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Max distance")) espConfig.maxDistance = espJson["Max distance"].asFloat();
    }

    for (size_t i = 0; i < esp.projectiles.size(); i++) {
        const auto& espJson = json["Esp"]["Projectiles"][i];
        auto& espConfig = esp.projectiles[i];

        if (espJson.isMember("Enabled")) espConfig.enabled = espJson["Enabled"].asBool();
        if (espJson.isMember("Font")) espConfig.font = espJson["Font"].asInt();
        if (espJson.isMember("Snaplines")) {
            const auto& snaplinesJson = espJson["Snaplines"];
            auto& snaplinesConfig = espConfig.snaplines;

            if (snaplinesJson.isMember("Enabled")) snaplinesConfig.enabled = snaplinesJson["Enabled"].asBool();

            if (snaplinesJson.isMember("Color")) {
                snaplinesConfig.color[0] = snaplinesJson["Color"][0].asFloat();
                snaplinesConfig.color[1] = snaplinesJson["Color"][1].asFloat();
                snaplinesConfig.color[2] = snaplinesJson["Color"][2].asFloat();
            }

            if (snaplinesJson.isMember("Rainbow")) snaplinesConfig.rainbow = snaplinesJson["Rainbow"].asBool();
            if (snaplinesJson.isMember("Rainbow speed")) snaplinesConfig.rainbowSpeed = snaplinesJson["Rainbow speed"].asFloat();
        }
        if (espJson.isMember("Box")) {
            const auto& boxJson = espJson["Box"];
            auto& boxConfig = espConfig.box;

            if (boxJson.isMember("Enabled")) boxConfig.enabled = boxJson["Enabled"].asBool();

            if (boxJson.isMember("Color")) {
                boxConfig.color[0] = boxJson["Color"][0].asFloat();
                boxConfig.color[1] = boxJson["Color"][1].asFloat();
                boxConfig.color[2] = boxJson["Color"][2].asFloat();
            }

            if (boxJson.isMember("Rainbow")) boxConfig.rainbow = boxJson["Rainbow"].asBool();
            if (boxJson.isMember("Rainbow speed")) boxConfig.rainbowSpeed = boxJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Box type")) espConfig.boxType = espJson["Box type"].asInt();

        if (espJson.isMember("Outline")) {
            const auto& outlineJson = espJson["Outline"];
            auto& outlineConfig = espConfig.outline;

            if (outlineJson.isMember("Enabled")) outlineConfig.enabled = outlineJson["Enabled"].asBool();

            if (outlineJson.isMember("Color")) {
                outlineConfig.color[0] = outlineJson["Color"][0].asFloat();
                outlineConfig.color[1] = outlineJson["Color"][1].asFloat();
                outlineConfig.color[2] = outlineJson["Color"][2].asFloat();
            }

            if (outlineJson.isMember("Rainbow")) outlineConfig.rainbow = outlineJson["Rainbow"].asBool();
            if (outlineJson.isMember("Rainbow speed")) outlineConfig.rainbowSpeed = outlineJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Name")) {
            const auto& nameJson = espJson["Name"];
            auto& nameConfig = espConfig.name;

            if (nameJson.isMember("Enabled")) nameConfig.enabled = nameJson["Enabled"].asBool();

            if (nameJson.isMember("Color")) {
                nameConfig.color[0] = nameJson["Color"][0].asFloat();
                nameConfig.color[1] = nameJson["Color"][1].asFloat();
                nameConfig.color[2] = nameJson["Color"][2].asFloat();
            }

            if (nameJson.isMember("Rainbow")) nameConfig.rainbow = nameJson["Rainbow"].asBool();
            if (nameJson.isMember("Rainbow speed")) nameConfig.rainbowSpeed = nameJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Distance")) {
            const auto& distanceJson = espJson["Distance"];
            auto& distanceConfig = espConfig.distance;

            if (distanceJson.isMember("Enabled")) distanceConfig.enabled = distanceJson["Enabled"].asBool();

            if (distanceJson.isMember("Color")) {
                distanceConfig.color[0] = distanceJson["Color"][0].asFloat();
                distanceConfig.color[1] = distanceJson["Color"][1].asFloat();
                distanceConfig.color[2] = distanceJson["Color"][2].asFloat();
            }

            if (distanceJson.isMember("Rainbow")) distanceConfig.rainbow = distanceJson["Rainbow"].asBool();
            if (distanceJson.isMember("Rainbow speed")) distanceConfig.rainbowSpeed = distanceJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Max distance")) espConfig.maxDistance = espJson["Max distance"].asFloat();
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
        if (visualsJson.isMember("World")) {
            const auto& worldJson = visualsJson["World"];

            if (worldJson.isMember("Enabled")) visuals.world.enabled = worldJson["Enabled"].asBool();

            if (worldJson.isMember("Color")) {
                visuals.world.color[0] = worldJson["Color"][0].asFloat();
                visuals.world.color[1] = worldJson["Color"][1].asFloat();
                visuals.world.color[2] = worldJson["Color"][2].asFloat();
            }
            if (worldJson.isMember("Rainbow")) visuals.world.rainbow = worldJson["Rainbow"].asBool();
            if (worldJson.isMember("Rainbow speed")) visuals.world.rainbowSpeed = worldJson["Rainbow speed"].asFloat();
        }
        if (visualsJson.isMember("Sky")) {
            const auto& skyJson = visualsJson["Sky"];

            if (skyJson.isMember("Enabled")) visuals.sky.enabled = skyJson["Enabled"].asBool();

            if (skyJson.isMember("Color")) {
                visuals.sky.color[0] = skyJson["Color"][0].asFloat();
                visuals.sky.color[1] = skyJson["Color"][1].asFloat();
                visuals.sky.color[2] = skyJson["Color"][2].asFloat();
            }
            if (skyJson.isMember("Rainbow")) visuals.sky.rainbow = skyJson["Rainbow"].asBool();
            if (skyJson.isMember("Rainbow speed")) visuals.sky.rainbowSpeed = skyJson["Rainbow speed"].asFloat();
        }
        if (visualsJson.isMember("Deagle spinner")) visuals.deagleSpinner = visualsJson["Deagle spinner"].asBool();
        if (visualsJson.isMember("Screen effect")) visuals.screenEffect = visualsJson["Screen effect"].asInt();
        if (visualsJson.isMember("Hit effect")) visuals.hitEffect = visualsJson["Hit effect"].asInt();
        if (visualsJson.isMember("Hit effect time")) visuals.hitEffectTime = visualsJson["Hit effect time"].asFloat();
        if (visualsJson.isMember("Hit marker")) visuals.hitMarker = visualsJson["Hit marker"].asInt();
        if (visualsJson.isMember("Hit marker time")) visuals.hitMarkerTime = visualsJson["Hit marker time"].asFloat();
        if (visualsJson.isMember("Playermodel T")) visuals.playerModelT = visualsJson["Playermodel T"].asInt();
        if (visualsJson.isMember("Playermodel CT")) visuals.playerModelCT = visualsJson["Playermodel CT"].asInt();

        if (visualsJson.isMember("Color correction")) {
            const auto& cc = visualsJson["Color correction"];

            if (cc.isMember("Enabled")) visuals.colorCorrection.enabled = cc["Enabled"].asBool();
            if (cc.isMember("Blue")) visuals.colorCorrection.blue = cc["Blue"].asFloat();
            if (cc.isMember("Red")) visuals.colorCorrection.red = cc["Red"].asFloat();
            if (cc.isMember("Mono")) visuals.colorCorrection.mono = cc["Mono"].asFloat();
            if (cc.isMember("Saturation")) visuals.colorCorrection.saturation = cc["Saturation"].asFloat();
            if (cc.isMember("Ghost")) visuals.colorCorrection.ghost = cc["Ghost"].asFloat();
            if (cc.isMember("Green")) visuals.colorCorrection.green = cc["Green"].asFloat();
            if (cc.isMember("Yellow")) visuals.colorCorrection.yellow = cc["Yellow"].asFloat();
        }
    }

    for (size_t i = 0; i < skinChanger.size(); i++) {
        const auto& skinChangerJson = json["skinChanger"][i];
        auto& skinChangerConfig = skinChanger[i];

        if (skinChangerJson.isMember("Enabled")) skinChangerConfig.enabled = skinChangerJson["Enabled"].asBool();
        if (skinChangerJson.isMember("definition_vector_index")) skinChangerConfig.itemIdIndex = skinChangerJson["definition_vector_index"].asInt();
        if (skinChangerJson.isMember("definition_index")) skinChangerConfig.itemId = skinChangerJson["definition_index"].asInt();
        if (skinChangerJson.isMember("entity_quality_vector_index")) skinChangerConfig.entity_quality_vector_index = skinChangerJson["entity_quality_vector_index"].asInt();
        if (skinChangerJson.isMember("entity_quality_index")) skinChangerConfig.quality = skinChangerJson["entity_quality_index"].asInt();
        if (skinChangerJson.isMember("paint_kit_vector_index")) skinChangerConfig.paint_kit_vector_index = skinChangerJson["paint_kit_vector_index"].asInt();
        if (skinChangerJson.isMember("paint_kit_index")) skinChangerConfig.paintKit = skinChangerJson["paint_kit_index"].asInt();
        if (skinChangerJson.isMember("definition_override_vector_index")) skinChangerConfig.definition_override_vector_index = skinChangerJson["definition_override_vector_index"].asInt();
        if (skinChangerJson.isMember("definition_override_index")) skinChangerConfig.definition_override_index = skinChangerJson["definition_override_index"].asInt();
        if (skinChangerJson.isMember("seed")) skinChangerConfig.seed = skinChangerJson["seed"].asInt();
        if (skinChangerJson.isMember("stat_trak")) skinChangerConfig.stat_trak = skinChangerJson["stat_trak"].asInt();
        if (skinChangerJson.isMember("wear")) skinChangerConfig.wear = skinChangerJson["wear"].asFloat();
        if (skinChangerJson.isMember("custom_name")) strcpy_s(skinChangerConfig.custom_name, sizeof(skinChangerConfig.custom_name), skinChangerJson["custom_name"].asCString());

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
        if (miscJson.isMember("Clock tag")) misc.clocktag = miscJson["Clock tag"].asBool();
        if (miscJson.isMember("Clan tag")) misc.clanTag = miscJson["Clan tag"].asString();
        if (miscJson.isMember("Animated clan tag")) misc.animatedClanTag = miscJson["Animated clan tag"].asBool();
        if (miscJson.isMember("Fast duck")) misc.fastDuck = miscJson["Fast duck"].asBool();
        if (miscJson.isMember("Moonwalk")) misc.moonwalk = miscJson["Moonwalk"].asBool();
        if (miscJson.isMember("Edge Jump")) misc.edgejump = miscJson["Edge Jump"].asBool();
        if (miscJson.isMember("Edge Jump Key")) misc.edgejumpkey = miscJson["Edge Jump Key"].asInt();
        if (miscJson.isMember("Slowwalk")) misc.slowwalk = miscJson["Slowwalk"].asBool();
        if (miscJson.isMember("Slowwalk key")) misc.slowwalkKey = miscJson["Slowwalk key"].asInt();
        if (miscJson.isMember("Sniper crosshair")) misc.sniperCrosshair = miscJson["Sniper crosshair"].asBool();
        if (miscJson.isMember("Recoil crosshair")) misc.recoilCrosshair = miscJson["Recoil crosshair"].asBool();
        if (miscJson.isMember("Auto pistol")) misc.autoPistol = miscJson["Auto pistol"].asBool();
        if (miscJson.isMember("Auto reload")) misc.autoReload = miscJson["Auto reload"].asBool();
        if (miscJson.isMember("Auto accept")) misc.autoAccept = miscJson["Auto accept"].asBool();
        if (miscJson.isMember("Radar hack")) misc.radarHack = miscJson["Radar hack"].asBool();
        if (miscJson.isMember("Reveal ranks")) misc.revealRanks = miscJson["Reveal ranks"].asBool();
        if (miscJson.isMember("Reveal money")) misc.revealMoney = miscJson["Reveal money"].asBool();
        if (miscJson.isMember("Reveal suspect")) misc.revealSuspect = miscJson["Reveal suspect"].asBool();

        if (const auto& spectatorList{ miscJson["Spectator list"] }; spectatorList.isObject()) {
            if (const auto& enabled{ spectatorList["Enabled"] }; enabled.isBool())
                misc.spectatorList.enabled = enabled.asBool();

            if (const auto& color{ spectatorList["Color"] }; color.isArray()) {
                misc.spectatorList.color[0] = color[0].asFloat();
                misc.spectatorList.color[1] = color[1].asFloat();
                misc.spectatorList.color[2] = color[2].asFloat();
            }
            if (const auto& rainbow{ spectatorList["Rainbow"] }; rainbow.isBool())
                misc.spectatorList.rainbow = rainbow.asBool();

            if (const auto& rainbowSpeed{ spectatorList["Rainbow speed"] }; rainbowSpeed.isDouble())
                misc.spectatorList.rainbowSpeed = rainbowSpeed.asFloat();
        }

        if (const auto& watermark{ miscJson["Watermark"] }; watermark.isObject()) {
            if (const auto& enabled{ watermark["Enabled"] }; enabled.isBool())
                misc.watermark.enabled = enabled.asBool();

            if (const auto& color{ watermark["Color"] }; color.isArray()) {
                misc.watermark.color[0] = color[0].asFloat();
                misc.watermark.color[1] = color[1].asFloat();
                misc.watermark.color[2] = color[2].asFloat();
            }
            if (const auto& rainbow{ watermark["Rainbow"] }; rainbow.isBool())
                misc.watermark.rainbow = rainbow.asBool();

            if (const auto& rainbowSpeed{ watermark["Rainbow speed"] }; rainbowSpeed.isDouble())
                misc.watermark.rainbowSpeed = rainbowSpeed.asFloat();
        }

        if (miscJson.isMember("Fix animation LOD")) misc.fixAnimationLOD = miscJson["Fix animation LOD"].asBool();
        if (miscJson.isMember("Fix bone matrix")) misc.fixBoneMatrix = miscJson["Fix bone matrix"].asBool();
        if (miscJson.isMember("Fix movement")) misc.fixMovement = miscJson["Fix movement"].asBool();
        if (miscJson.isMember("Disable model occlusion")) misc.disableModelOcclusion = miscJson["Disable model occlusion"].asBool();
        if (miscJson.isMember("Aspect Ratio")) misc.aspectratio = miscJson["Aspect Ratio"].asFloat();
        if (miscJson.isMember("Kill message")) misc.killMessage = miscJson["Kill message"].asBool();
        if (miscJson.isMember("Kill message string")) misc.killMessageString = miscJson["Kill message string"].asString();
        if (miscJson.isMember("Name stealer"))  misc.nameStealer = miscJson["Name stealer"].asBool();
        if (miscJson.isMember("Disable HUD blur"))  misc.disablePanoramablur = miscJson["Disable HUD blur"].asBool();
        if (miscJson.isMember("Ban color")) misc.banColor = miscJson["Ban color"].asInt();
        if (miscJson.isMember("Ban text")) misc.banText = miscJson["Ban text"].asString();
        if (miscJson.isMember("Fast plant")) misc.fastPlant = miscJson["Fast plant"].asBool();

        if (const auto& bombTimer{ miscJson["Bomb timer"] }; bombTimer.isObject()) {
            if (const auto& enabled{ bombTimer["Enabled"] }; enabled.isBool())
                misc.bombTimer.enabled = enabled.asBool();

            if (const auto& color{ bombTimer["Color"] }; color.isArray()) {
                misc.bombTimer.color[0] = color[0].asFloat();
                misc.bombTimer.color[1] = color[1].asFloat();
                misc.bombTimer.color[2] = color[2].asFloat();
            }
            if (const auto& rainbow{ bombTimer["Rainbow"] }; rainbow.isBool())
                misc.bombTimer.rainbow = rainbow.asBool();

            if (const auto& rainbowSpeed{ bombTimer["Rainbow speed"] }; rainbowSpeed.isDouble())
                misc.bombTimer.rainbowSpeed = rainbowSpeed.asFloat();
        }

        if (miscJson.isMember("Quick reload")) misc.quickReload = miscJson["Quick reload"].asBool();
        if (miscJson.isMember("Prepare revolver")) misc.prepareRevolver = miscJson["Prepare revolver"].asBool();
        if (miscJson.isMember("Prepare revolver key")) misc.prepareRevolverKey = miscJson["Prepare revolver key"].asInt();
        if (miscJson.isMember("Hit sound")) misc.hitSound = miscJson["Hit sound"].asInt();
        if (miscJson.isMember("Choked packets")) misc.chokedPackets = miscJson["Choked packets"].asInt();
        if (miscJson.isMember("Choked packets key")) misc.chokedPacketsKey = miscJson["Choked packets key"].asInt();
        if (miscJson.isMember("Quick healthshot key")) misc.quickHealthshotKey = miscJson["Quick healthshot key"].asInt();
        if (miscJson.isMember("Grenade predict")) misc.nadePredict = miscJson["Grenade predict"].asBool();
        if (miscJson.isMember("Fix tablet signal")) misc.fixTabletSignal = miscJson["Fix tablet signal"].asBool();
        if (miscJson.isMember("Max angle delta")) misc.maxAngleDelta = miscJson["Max angle delta"].asFloat();
        if (miscJson.isMember("Fake prime")) misc.fakePrime = miscJson["Fake prime"].asBool();
    }

    {
        const auto& reportbotJson = json["Reportbot"];

        if (reportbotJson.isMember("Enabled")) reportbot.enabled = reportbotJson["Enabled"].asBool();
        if (reportbotJson.isMember("Target")) reportbot.target = reportbotJson["Target"].asInt();
        if (reportbotJson.isMember("Delay")) reportbot.delay = reportbotJson["Delay"].asInt();
        if (reportbotJson.isMember("Rounds")) reportbot.rounds = reportbotJson["Rounds"].asInt();
        if (reportbotJson.isMember("Abusive Communications")) reportbot.textAbuse = reportbotJson["Abusive Communications"].asBool();
        if (reportbotJson.isMember("Griefing")) reportbot.griefing = reportbotJson["Griefing"].asBool();
        if (reportbotJson.isMember("Wall Hacking")) reportbot.wallhack = reportbotJson["Wall Hacking"].asBool();
        if (reportbotJson.isMember("Aim Hacking")) reportbot.aimbot = reportbotJson["Aim Hacking"].asBool();
        if (reportbotJson.isMember("Other Hacking")) reportbot.other = reportbotJson["Other Hacking"].asBool();
    }
}

void Config::save(size_t id) const noexcept
{
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
        glowJson["thickness"] = glowConfig.thickness;
        glowJson["alpha"] = glowConfig.alpha;
        glowJson["style"] = glowConfig.style;

        {
            auto& colorJson = glowJson["Color"];
            const auto& colorConfig = glowConfig.color;

            colorJson["Color"][0] = colorConfig.color[0];
            colorJson["Color"][1] = colorConfig.color[1];
            colorJson["Color"][2] = colorConfig.color[2];

            colorJson["Rainbow"] = colorConfig.rainbow;
            colorJson["Rainbow speed"] = colorConfig.rainbowSpeed;
        }
    }

    for (size_t i = 0; i < chams.size(); i++) {
        auto& chamsJson = json["Chams"][i];
        const auto& chamsConfig = chams[i];

        for (size_t j = 0; j < chams[0].materials.size(); j++) {
            auto& materialsJson = chamsJson[j];
            const auto& materialsConfig = chams[i].materials[j];

            materialsJson["Enabled"] = materialsConfig.enabled;
            materialsJson["Health based"] = materialsConfig.healthBased;
            materialsJson["Blinking"] = materialsConfig.blinking;
            materialsJson["Material"] = materialsConfig.material;
            materialsJson["Wireframe"] = materialsConfig.wireframe;

            {
                auto& colorJson = materialsJson["Color"];
                const auto& colorConfig = materialsConfig.color;

                colorJson["Color"][0] = colorConfig.color[0];
                colorJson["Color"][1] = colorConfig.color[1];
                colorJson["Color"][2] = colorConfig.color[2];

                colorJson["Rainbow"] = colorConfig.rainbow;
                colorJson["Rainbow speed"] = colorConfig.rainbowSpeed;
            }

            materialsJson["Alpha"] = materialsConfig.alpha;
        }
    }

    for (size_t i = 0; i < esp.players.size(); i++) {
        auto& espJson = json["Esp"]["Players"][i];
        const auto& espConfig = esp.players[i];

        espJson["Enabled"] = espConfig.enabled;
        espJson["Font"] = espConfig.font;

        {
            auto& snaplinesJson = espJson["Snaplines"];
            const auto& snaplinesConfig = espConfig.snaplines;

            snaplinesJson["Enabled"] = snaplinesConfig.enabled;
            snaplinesJson["Color"][0] = snaplinesConfig.color[0];
            snaplinesJson["Color"][1] = snaplinesConfig.color[1];
            snaplinesJson["Color"][2] = snaplinesConfig.color[2];
            snaplinesJson["Rainbow"] = snaplinesConfig.rainbow;
            snaplinesJson["Rainbow speed"] = snaplinesConfig.rainbowSpeed;
        }

        {
            auto& eyeTracesJson = espJson["Eye traces"];
            const auto& eyeTracesConfig = espConfig.eyeTraces;

            eyeTracesJson["Enabled"] = eyeTracesConfig.enabled;
            eyeTracesJson["Color"][0] = eyeTracesConfig.color[0];
            eyeTracesJson["Color"][1] = eyeTracesConfig.color[1];
            eyeTracesJson["Color"][2] = eyeTracesConfig.color[2];
            eyeTracesJson["Rainbow"] = eyeTracesConfig.rainbow;
            eyeTracesJson["Rainbow speed"] = eyeTracesConfig.rainbowSpeed;
        }

        {
            auto& boxJson = espJson["Box"];
            const auto& boxConfig = espConfig.box;

            boxJson["Enabled"] = boxConfig.enabled;
            boxJson["Color"][0] = boxConfig.color[0];
            boxJson["Color"][1] = boxConfig.color[1];
            boxJson["Color"][2] = boxConfig.color[2];
            boxJson["Rainbow"] = boxConfig.rainbow;
            boxJson["Rainbow speed"] = boxConfig.rainbowSpeed;
        }

        espJson["Box type"] = espConfig.boxType;

        {
            auto& nameJson = espJson["Name"];
            const auto& nameConfig = espConfig.name;

            nameJson["Enabled"] = nameConfig.enabled;
            nameJson["Color"][0] = nameConfig.color[0];
            nameJson["Color"][1] = nameConfig.color[1];
            nameJson["Color"][2] = nameConfig.color[2];
            nameJson["Rainbow"] = nameConfig.rainbow;
            nameJson["Rainbow speed"] = nameConfig.rainbowSpeed;
        }

        {
            auto& healthJson = espJson["Health"];
            const auto& healthConfig = espConfig.health;

            healthJson["Enabled"] = healthConfig.enabled;
            healthJson["Color"][0] = healthConfig.color[0];
            healthJson["Color"][1] = healthConfig.color[1];
            healthJson["Color"][2] = healthConfig.color[2];
            healthJson["Rainbow"] = healthConfig.rainbow;
            healthJson["Rainbow speed"] = healthConfig.rainbowSpeed;
        }

        {
            auto& healthBarJson = espJson["Health bar"];
            const auto& healthBarConfig = espConfig.healthBar;

            healthBarJson["Enabled"] = healthBarConfig.enabled;
            healthBarJson["Color"][0] = healthBarConfig.color[0];
            healthBarJson["Color"][1] = healthBarConfig.color[1];
            healthBarJson["Color"][2] = healthBarConfig.color[2];
            healthBarJson["Rainbow"] = healthBarConfig.rainbow;
            healthBarJson["Rainbow speed"] = healthBarConfig.rainbowSpeed;
        }

        {
            auto& armorJson = espJson["Armor"];
            const auto& armorConfig = espConfig.armor;

            armorJson["Enabled"] = armorConfig.enabled;
            armorJson["Color"][0] = armorConfig.color[0];
            armorJson["Color"][1] = armorConfig.color[1];
            armorJson["Color"][2] = armorConfig.color[2];
            armorJson["Rainbow"] = armorConfig.rainbow;
            armorJson["Rainbow speed"] = armorConfig.rainbowSpeed;
        }

        {
            auto& armorBarJson = espJson["Armor bar"];
            const auto& armorBarConfig = espConfig.armorBar;

            armorBarJson["Enabled"] = armorBarConfig.enabled;
            armorBarJson["Color"][0] = armorBarConfig.color[0];
            armorBarJson["Color"][1] = armorBarConfig.color[1];
            armorBarJson["Color"][2] = armorBarConfig.color[2];
            armorBarJson["Rainbow"] = armorBarConfig.rainbow;
            armorBarJson["Rainbow speed"] = armorBarConfig.rainbowSpeed;
        }

        {
            auto& moneyJson = espJson["Money"];
            const auto& moneyConfig = espConfig.money;

            moneyJson["Enabled"] = moneyConfig.enabled;
            moneyJson["Color"][0] = moneyConfig.color[0];
            moneyJson["Color"][1] = moneyConfig.color[1];
            moneyJson["Color"][2] = moneyConfig.color[2];
            moneyJson["Rainbow"] = moneyConfig.rainbow;
            moneyJson["Rainbow speed"] = moneyConfig.rainbowSpeed;
        }

        {
            auto& headDotJson = espJson["Head dot"];
            const auto& headDotConfig = espConfig.headDot;

            headDotJson["Enabled"] = headDotConfig.enabled;
            headDotJson["Color"][0] = headDotConfig.color[0];
            headDotJson["Color"][1] = headDotConfig.color[1];
            headDotJson["Color"][2] = headDotConfig.color[2];
            headDotJson["Rainbow"] = headDotConfig.rainbow;
            headDotJson["Rainbow speed"] = headDotConfig.rainbowSpeed;
        }

        {
            auto& activeWeaponJson = espJson["Active weapon"];
            const auto& activeWeaponConfig = espConfig.activeWeapon;

            activeWeaponJson["Enabled"] = activeWeaponConfig.enabled;
            activeWeaponJson["Color"][0] = activeWeaponConfig.color[0];
            activeWeaponJson["Color"][1] = activeWeaponConfig.color[1];
            activeWeaponJson["Color"][2] = activeWeaponConfig.color[2];
            activeWeaponJson["Rainbow"] = activeWeaponConfig.rainbow;
            activeWeaponJson["Rainbow speed"] = activeWeaponConfig.rainbowSpeed;
        }

        {
            auto& outlineJson = espJson["Outline"];
            const auto& outlineConfig = espConfig.outline;

            outlineJson["Enabled"] = outlineConfig.enabled;
            outlineJson["Color"][0] = outlineConfig.color[0];
            outlineJson["Color"][1] = outlineConfig.color[1];
            outlineJson["Color"][2] = outlineConfig.color[2];
            outlineJson["Rainbow"] = outlineConfig.rainbow;
            outlineJson["Rainbow speed"] = outlineConfig.rainbowSpeed;
        }

        {
            auto& distanceJson = espJson["Distance"];
            const auto& distanceConfig = espConfig.distance;

            distanceJson["Enabled"] = distanceConfig.enabled;
            distanceJson["Color"][0] = distanceConfig.color[0];
            distanceJson["Color"][1] = distanceConfig.color[1];
            distanceJson["Color"][2] = distanceConfig.color[2];
            distanceJson["Rainbow"] = distanceConfig.rainbow;
            distanceJson["Rainbow speed"] = distanceConfig.rainbowSpeed;
        }

        espJson["Dead ESP"] = espConfig.deadesp;
        espJson["Max distance"] = espConfig.maxDistance;
    }

    {
        auto& espJson = json["Esp"]["Weapons"];
        const auto& espConfig = esp.weapon;

        espJson["Enabled"] = espConfig.enabled;
        espJson["Font"] = espConfig.font;
        {
            auto& snaplinesJson = espJson["Snaplines"];
            const auto& snaplinesConfig = espConfig.snaplines;

            snaplinesJson["Enabled"] = snaplinesConfig.enabled;
            snaplinesJson["Color"][0] = snaplinesConfig.color[0];
            snaplinesJson["Color"][1] = snaplinesConfig.color[1];
            snaplinesJson["Color"][2] = snaplinesConfig.color[2];
            snaplinesJson["Rainbow"] = snaplinesConfig.rainbow;
            snaplinesJson["Rainbow speed"] = snaplinesConfig.rainbowSpeed;
        }

        {
            auto& boxJson = espJson["Box"];
            const auto& boxConfig = espConfig.box;

            boxJson["Enabled"] = boxConfig.enabled;
            boxJson["Color"][0] = boxConfig.color[0];
            boxJson["Color"][1] = boxConfig.color[1];
            boxJson["Color"][2] = boxConfig.color[2];
            boxJson["Rainbow"] = boxConfig.rainbow;
            boxJson["Rainbow speed"] = boxConfig.rainbowSpeed;
        }

        espJson["Box type"] = espConfig.boxType;

        {
            auto& outlineJson = espJson["Outline"];
            const auto& outlineConfig = espConfig.outline;

            outlineJson["Enabled"] = outlineConfig.enabled;
            outlineJson["Color"][0] = outlineConfig.color[0];
            outlineJson["Color"][1] = outlineConfig.color[1];
            outlineJson["Color"][2] = outlineConfig.color[2];
            outlineJson["Rainbow"] = outlineConfig.rainbow;
            outlineJson["Rainbow speed"] = outlineConfig.rainbowSpeed;
        }

        {
            auto& nameJson = espJson["Name"];
            const auto& nameConfig = espConfig.name;

            nameJson["Enabled"] = nameConfig.enabled;
            nameJson["Color"][0] = nameConfig.color[0];
            nameJson["Color"][1] = nameConfig.color[1];
            nameJson["Color"][2] = nameConfig.color[2];
            nameJson["Rainbow"] = nameConfig.rainbow;
            nameJson["Rainbow speed"] = nameConfig.rainbowSpeed;
        }

        {
            auto& distanceJson = espJson["Distance"];
            const auto& distanceConfig = espConfig.distance;

            distanceJson["Enabled"] = distanceConfig.enabled;
            distanceJson["Color"][0] = distanceConfig.color[0];
            distanceJson["Color"][1] = distanceConfig.color[1];
            distanceJson["Color"][2] = distanceConfig.color[2];
            distanceJson["Rainbow"] = distanceConfig.rainbow;
            distanceJson["Rainbow speed"] = distanceConfig.rainbowSpeed;
        }

        espJson["Max distance"] = espConfig.maxDistance;
    }

    for (size_t i = 0; i < esp.dangerZone.size(); i++) {
        auto& espJson = json["Esp"]["Danger Zone"][i];
        const auto& espConfig = esp.dangerZone[i];

        espJson["Enabled"] = espConfig.enabled;
        espJson["Font"] = espConfig.font;
        {
            auto& snaplinesJson = espJson["Snaplines"];
            const auto& snaplinesConfig = espConfig.snaplines;

            snaplinesJson["Enabled"] = snaplinesConfig.enabled;
            snaplinesJson["Color"][0] = snaplinesConfig.color[0];
            snaplinesJson["Color"][1] = snaplinesConfig.color[1];
            snaplinesJson["Color"][2] = snaplinesConfig.color[2];
            snaplinesJson["Rainbow"] = snaplinesConfig.rainbow;
            snaplinesJson["Rainbow speed"] = snaplinesConfig.rainbowSpeed;
        }

        {
            auto& boxJson = espJson["Box"];
            const auto& boxConfig = espConfig.box;

            boxJson["Enabled"] = boxConfig.enabled;
            boxJson["Color"][0] = boxConfig.color[0];
            boxJson["Color"][1] = boxConfig.color[1];
            boxJson["Color"][2] = boxConfig.color[2];
            boxJson["Rainbow"] = boxConfig.rainbow;
            boxJson["Rainbow speed"] = boxConfig.rainbowSpeed;
        }

        espJson["Box type"] = espConfig.boxType;
        
        {
            auto& outlineJson = espJson["Outline"];
            const auto& outlineConfig = espConfig.outline;

            outlineJson["Enabled"] = outlineConfig.enabled;
            outlineJson["Color"][0] = outlineConfig.color[0];
            outlineJson["Color"][1] = outlineConfig.color[1];
            outlineJson["Color"][2] = outlineConfig.color[2];
            outlineJson["Rainbow"] = outlineConfig.rainbow;
            outlineJson["Rainbow speed"] = outlineConfig.rainbowSpeed;
        }

        {
            auto& nameJson = espJson["Name"];
            const auto& nameConfig = espConfig.name;

            nameJson["Enabled"] = nameConfig.enabled;
            nameJson["Color"][0] = nameConfig.color[0];
            nameJson["Color"][1] = nameConfig.color[1];
            nameJson["Color"][2] = nameConfig.color[2];
            nameJson["Rainbow"] = nameConfig.rainbow;
            nameJson["Rainbow speed"] = nameConfig.rainbowSpeed;
        }

        {
            auto& distanceJson = espJson["Distance"];
            const auto& distanceConfig = espConfig.distance;

            distanceJson["Enabled"] = distanceConfig.enabled;
            distanceJson["Color"][0] = distanceConfig.color[0];
            distanceJson["Color"][1] = distanceConfig.color[1];
            distanceJson["Color"][2] = distanceConfig.color[2];
            distanceJson["Rainbow"] = distanceConfig.rainbow;
            distanceJson["Rainbow speed"] = distanceConfig.rainbowSpeed;
        }

        espJson["Max distance"] = espConfig.maxDistance;
    }

    for (size_t i = 0; i < esp.projectiles.size(); i++) {
        auto& espJson = json["Esp"]["Projectiles"][i];
        const auto& espConfig = esp.projectiles[i];

        espJson["Enabled"] = espConfig.enabled;
        espJson["Font"] = espConfig.font;
        {
            auto& snaplinesJson = espJson["Snaplines"];
            const auto& snaplinesConfig = espConfig.snaplines;

            snaplinesJson["Enabled"] = snaplinesConfig.enabled;
            snaplinesJson["Color"][0] = snaplinesConfig.color[0];
            snaplinesJson["Color"][1] = snaplinesConfig.color[1];
            snaplinesJson["Color"][2] = snaplinesConfig.color[2];
            snaplinesJson["Rainbow"] = snaplinesConfig.rainbow;
            snaplinesJson["Rainbow speed"] = snaplinesConfig.rainbowSpeed;
        }

        {
            auto& boxJson = espJson["Box"];
            const auto& boxConfig = espConfig.box;

            boxJson["Enabled"] = boxConfig.enabled;
            boxJson["Color"][0] = boxConfig.color[0];
            boxJson["Color"][1] = boxConfig.color[1];
            boxJson["Color"][2] = boxConfig.color[2];
            boxJson["Rainbow"] = boxConfig.rainbow;
            boxJson["Rainbow speed"] = boxConfig.rainbowSpeed;
        }

        espJson["Box type"] = espConfig.boxType;

        {
            auto& outlineJson = espJson["Outline"];
            const auto& outlineConfig = espConfig.outline;

            outlineJson["Enabled"] = outlineConfig.enabled;
            outlineJson["Color"][0] = outlineConfig.color[0];
            outlineJson["Color"][1] = outlineConfig.color[1];
            outlineJson["Color"][2] = outlineConfig.color[2];
            outlineJson["Rainbow"] = outlineConfig.rainbow;
            outlineJson["Rainbow speed"] = outlineConfig.rainbowSpeed;
        }

        {
            auto& nameJson = espJson["Name"];
            const auto& nameConfig = espConfig.name;

            nameJson["Enabled"] = nameConfig.enabled;
            nameJson["Color"][0] = nameConfig.color[0];
            nameJson["Color"][1] = nameConfig.color[1];
            nameJson["Color"][2] = nameConfig.color[2];
            nameJson["Rainbow"] = nameConfig.rainbow;
            nameJson["Rainbow speed"] = nameConfig.rainbowSpeed;
        }

        {
            auto& distanceJson = espJson["Distance"];
            const auto& distanceConfig = espConfig.distance;

            distanceJson["Enabled"] = distanceConfig.enabled;
            distanceJson["Color"][0] = distanceConfig.color[0];
            distanceJson["Color"][1] = distanceConfig.color[1];
            distanceJson["Color"][2] = distanceConfig.color[2];
            distanceJson["Rainbow"] = distanceConfig.rainbow;
            distanceJson["Rainbow speed"] = distanceConfig.rainbowSpeed;
        }

        espJson["Max distance"] = espConfig.maxDistance;
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

        {
            auto& worldJson = visualsJson["World"];
            worldJson["Enabled"] = visuals.world.enabled;
            worldJson["Color"][0] = visuals.world.color[0];
            worldJson["Color"][1] = visuals.world.color[1];
            worldJson["Color"][2] = visuals.world.color[2];
            worldJson["Rainbow"] = visuals.world.rainbow;
            worldJson["Rainbow speed"] = visuals.world.rainbowSpeed;
        }

        {
            auto& skyJson = visualsJson["Sky"];
            skyJson["Enabled"] = visuals.sky.enabled;
            skyJson["Color"][0] = visuals.sky.color[0];
            skyJson["Color"][1] = visuals.sky.color[1];
            skyJson["Color"][2] = visuals.sky.color[2];
            skyJson["Rainbow"] = visuals.sky.rainbow;
            skyJson["Rainbow speed"] = visuals.sky.rainbowSpeed;
        }

        visualsJson["Deagle spinner"] = visuals.deagleSpinner;
        visualsJson["Screen effect"] = visuals.screenEffect;
        visualsJson["Hit effect"] = visuals.hitEffect;
        visualsJson["Hit effect time"] = visuals.hitEffectTime;
        visualsJson["Hit marker"] = visuals.hitMarker;
        visualsJson["Hit marker time"] = visuals.hitMarkerTime;
        visualsJson["Playermodel T"] = visuals.playerModelT;
        visualsJson["Playermodel CT"] = visuals.playerModelCT;

        {
            auto& cc = visualsJson["Color correction"];
            cc["Enabled"] = visuals.colorCorrection.enabled;
            cc["Blue"] = visuals.colorCorrection.blue;
            cc["Red"] = visuals.colorCorrection.red;
            cc["Mono"] = visuals.colorCorrection.mono;
            cc["Saturation"] = visuals.colorCorrection.saturation;
            cc["Ghost"] = visuals.colorCorrection.ghost;
            cc["Green"] = visuals.colorCorrection.green;
            cc["Yellow"] = visuals.colorCorrection.yellow;
        }
    }

    for (size_t i = 0; i < skinChanger.size(); i++) {
        auto& skinChangerJson = json["skinChanger"][i];
        const auto& skinChangerConfig = skinChanger[i];

        skinChangerJson["Enabled"] = skinChangerConfig.enabled;
        skinChangerJson["definition_vector_index"] = skinChangerConfig.itemIdIndex;
        skinChangerJson["definition_index"] = skinChangerConfig.itemId;
        skinChangerJson["entity_quality_vector_index"] = skinChangerConfig.entity_quality_vector_index;
        skinChangerJson["entity_quality_index"] = skinChangerConfig.quality;
        skinChangerJson["paint_kit_vector_index"] = skinChangerConfig.paint_kit_vector_index;
        skinChangerJson["paint_kit_index"] = skinChangerConfig.paintKit;
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
        miscJson["Clock tag"] = misc.clocktag;
        miscJson["Clan tag"] = misc.clanTag;
        miscJson["Animated clan tag"] = misc.animatedClanTag;
        miscJson["Fast duck"] = misc.fastDuck;
        miscJson["Moonwalk"] = misc.moonwalk;
        miscJson["Edge Jump"] = misc.edgejump;
        miscJson["Edge Jump Key"] = misc.edgejumpkey;
        miscJson["Slowwalk"] = misc.slowwalk;
        miscJson["Slowwalk key"] = misc.slowwalkKey;
        miscJson["Sniper crosshair"] = misc.sniperCrosshair;
        miscJson["Recoil crosshair"] = misc.recoilCrosshair;
        miscJson["Auto pistol"] = misc.autoPistol;
        miscJson["Auto reload"] = misc.autoReload;
        miscJson["Auto accept"] = misc.autoAccept;
        miscJson["Radar hack"] = misc.radarHack;
        miscJson["Reveal ranks"] = misc.revealRanks;
        miscJson["Reveal money"] = misc.revealMoney;
        miscJson["Reveal suspect"] = misc.revealSuspect;

        {
            auto& spectatorListJson = miscJson["Spectator list"];
            spectatorListJson["Enabled"] = misc.spectatorList.enabled;
            spectatorListJson["Color"][0] = misc.spectatorList.color[0];
            spectatorListJson["Color"][1] = misc.spectatorList.color[1];
            spectatorListJson["Color"][2] = misc.spectatorList.color[2];
            spectatorListJson["Rainbow"] = misc.spectatorList.rainbow;
            spectatorListJson["Rainbow speed"] = misc.spectatorList.rainbowSpeed;
        }

        {
            auto& watermarkJson = miscJson["Watermark"];
            watermarkJson["Enabled"] = misc.watermark.enabled;
            watermarkJson["Color"][0] = misc.watermark.color[0];
            watermarkJson["Color"][1] = misc.watermark.color[1];
            watermarkJson["Color"][2] = misc.watermark.color[2];
            watermarkJson["Rainbow"] = misc.watermark.rainbow;
            watermarkJson["Rainbow speed"] = misc.watermark.rainbowSpeed;
        }

        miscJson["Fix animation LOD"] = misc.fixAnimationLOD;
        miscJson["Fix bone matrix"] = misc.fixBoneMatrix;
        miscJson["Fix movement"] = misc.fixMovement;
        miscJson["Disable model occlusion"] = misc.disableModelOcclusion;
        miscJson["Aspect Ratio"] = misc.aspectratio;
        miscJson["Kill message"] = misc.killMessage;
        miscJson["Kill message string"] = misc.killMessageString;
        miscJson["Name stealer"] = misc.nameStealer;
        miscJson["Disable HUD blur"] = misc.disablePanoramablur;
        miscJson["Ban color"] = misc.banColor;
        miscJson["Ban text"] = misc.banText;
        miscJson["Fast plant"] = misc.fastPlant;

        {
            auto& bombTimerJson = miscJson["Bomb timer"];
            bombTimerJson["Enabled"] = misc.bombTimer.enabled;
            bombTimerJson["Color"][0] = misc.bombTimer.color[0];
            bombTimerJson["Color"][1] = misc.bombTimer.color[1];
            bombTimerJson["Color"][2] = misc.bombTimer.color[2];
            bombTimerJson["Rainbow"] = misc.bombTimer.rainbow;
            bombTimerJson["Rainbow speed"] = misc.bombTimer.rainbowSpeed;
        }

        miscJson["Quick reload"] = misc.quickReload;
        miscJson["Prepare revolver"] = misc.prepareRevolver;
        miscJson["Prepare revolver key"] = misc.prepareRevolverKey;
        miscJson["Hit sound"] = misc.hitSound;
        miscJson["Choked packets"] = misc.chokedPackets;
        miscJson["Choked packets key"] = misc.chokedPacketsKey;
        miscJson["Quick healthshot key"] = misc.quickHealthshotKey;
        miscJson["Grenade predict"] = misc.nadePredict;
        miscJson["Fix tablet signal"] = misc.fixTabletSignal;
        miscJson["Max angle delta"] = misc.maxAngleDelta;
        miscJson["Fake prime"] = misc.fakePrime;
    }

    {
        auto& reportbotJson = json["Reportbot"];

        reportbotJson["Enabled"] = reportbot.enabled;
        reportbotJson["Target"] = reportbot.target;
        reportbotJson["Delay"] = reportbot.delay;
        reportbotJson["Rounds"] = reportbot.rounds;
        reportbotJson["Abusive Communications"] = reportbot.textAbuse;
        reportbotJson["Griefing"] = reportbot.griefing;
        reportbotJson["Wall Hacking"] = reportbot.wallhack;
        reportbotJson["Aim Hacking"] = reportbot.aimbot;
        reportbotJson["Other Hacking"] = reportbot.other;
    }

    if (!std::filesystem::is_directory(path)) {
        std::filesystem::remove(path);
        std::filesystem::create_directory(path);
    }

    if (std::ofstream out{ path / (const char8_t*)configs[id].c_str() }; out.good())
        out << json;
}

void Config::add(const char* name) noexcept
{
    if (*name && std::find(std::cbegin(configs), std::cend(configs), name) == std::cend(configs))
        configs.emplace_back(name);
}

void Config::remove(size_t id) noexcept
{
    std::filesystem::remove(path / (const char8_t*)configs[id].c_str());
    configs.erase(configs.cbegin() + id);
}

void Config::rename(size_t item, const char* newName) noexcept
{
    std::filesystem::rename(path / (const char8_t*)configs[item].c_str(), path / (const char8_t*)newName);
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
