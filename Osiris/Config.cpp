#include <fstream>

#ifdef _WIN32
#include <ShlObj.h>
#endif

#include "nlohmann/json.hpp"

#include "Config.h"
#include "Helpers.h"

#ifdef _WIN32
int CALLBACK fontCallback(const LOGFONTA* lpelfe, const TEXTMETRICA*, DWORD, LPARAM lParam)
{
    const auto fontName = (const char*)reinterpret_cast<const ENUMLOGFONTEXA*>(lpelfe)->elfFullName;

    if (fontName[0] == '@')
        return TRUE;

    if (HFONT font = CreateFontA(0, 0, 0, 0,
        FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH, fontName)) {

        DWORD fontData = GDI_ERROR;

        if (HDC hdc = CreateCompatibleDC(nullptr)) {
            SelectObject(hdc, font);
            // Do not use TTC fonts as we only support TTF fonts
            fontData = GetFontData(hdc, 'fctt', 0, NULL, 0);
            DeleteDC(hdc);
        }
        DeleteObject(font);

        if (fontData == GDI_ERROR)
            reinterpret_cast<std::vector<std::string>*>(lParam)->emplace_back(fontName);
    }
    return TRUE;
}
#endif

Config::Config(const char* name) noexcept
{
#ifdef _WIN32
    if (PWSTR pathToDocuments; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &pathToDocuments))) {
        path = pathToDocuments;
        path /= name;
        CoTaskMemFree(pathToDocuments);
    }
#endif

    listConfigs();
    misc.clanTag[0] = '\0';

#ifdef _WIN32
    LOGFONTA logfont;
    logfont.lfCharSet = ANSI_CHARSET;
    logfont.lfPitchAndFamily = DEFAULT_PITCH;
    logfont.lfFaceName[0] = '\0';

    EnumFontFamiliesExA(GetDC(nullptr), &logfont, fontCallback, (LPARAM)&systemFonts, 0);
#endif

    std::sort(std::next(systemFonts.begin()), systemFonts.end());
}

using json = nlohmann::basic_json<std::map, std::vector, std::string, bool, std::int64_t, std::uint64_t, float>;
using value_t = json::value_t;

template <value_t Type, typename T>
static typename std::enable_if_t<!std::is_same_v<T, bool>> read(const json& j, const char* key, T& o) noexcept
{
    if (!j.contains(key))
        return;

    if (const auto& val = j[key]; val.type() == Type)
        val.get_to(o);
}

static void read(const json& j, const char* key, bool& o) noexcept
{
    if (!j.contains(key))
        return;

    if (const auto& val = j[key]; val.type() == value_t::boolean)
        val.get_to(o);
}

static void read(const json& j, const char* key, float& o) noexcept
{
    if (!j.contains(key))
        return;

    if (const auto& val = j[key]; val.type() == value_t::number_float)
        val.get_to(o);
}

static void read(const json& j, const char* key, int& o) noexcept
{
    if (!j.contains(key))
        return;

    if (const auto& val = j[key]; val.is_number_integer())
        val.get_to(o);
}

template <typename T, size_t Size>
static void read_array_opt(const json& j, const char* key, std::array<T, Size>& o) noexcept
{
    if (j.contains(key) && j[key].type() == value_t::array) {
        std::size_t i = 0;
        for (const auto& e : j[key]) {
            if (i >= o.size())
                break;

            if (e.is_null())
                continue;

            e.get_to(o[i]);
            ++i;
        }
    }
}

template <typename T, size_t Size>
static void read(const json& j, const char* key, std::array<T, Size>& o) noexcept
{
    if (!j.contains(key))
        return;

    if (const auto& val = j[key]; val.type() == value_t::array && val.size() == o.size()) {
        for (std::size_t i = 0; i < val.size(); ++i) {
            if (!val[i].empty())
                val[i].get_to(o[i]);
        }
    }
}

template <typename T>
static void read(const json& j, const char* key, std::unordered_map<std::string, T>& o) noexcept
{
    if (j.contains(key) && j[key].is_object()) {
        for (auto& element : j[key].items())
            element.value().get_to(o[element.key()]);
    }
}

static void from_json(const json& j, ColorA& c)
{
    read(j, "Color", c.color);
    read(j, "Rainbow", c.rainbow);
    read(j, "Rainbow Speed", c.rainbowSpeed);
}

static void from_json(const json& j, ColorToggle& ct)
{
    from_json(j, static_cast<ColorA&>(ct));
    read(j, "Enabled", ct.enabled);
}

static void from_json(const json& j, Config::Color& c)
{
    read(j, "Color", c.color);
    read(j, "Rainbow", c.rainbow);
    read(j, "Rainbow Speed", c.rainbowSpeed);
}

static void from_json(const json& j, Config::ColorToggle& ct)
{
    from_json(j, static_cast<Config::Color&>(ct));
    read(j, "Enabled", ct.enabled);
}

static void from_json(const json& j, ColorToggleRounding& ctr)
{
    from_json(j, static_cast<ColorToggle&>(ctr));

    read(j, "Rounding", ctr.rounding);
}

static void from_json(const json& j, ColorToggleThickness& ctt)
{
    from_json(j, static_cast<ColorToggle&>(ctt));

    read(j, "Thickness", ctt.thickness);
}

static void from_json(const json& j, ColorToggleThicknessRounding& cttr)
{
    from_json(j, static_cast<ColorToggleRounding&>(cttr));

    read(j, "Thickness", cttr.thickness);
}

static void from_json(const json& j, Font& f)
{
    read<value_t::string>(j, "Name", f.name);

    if (!f.name.empty())
        config->scheduleFontLoad(f.name);

    if (const auto it = std::find_if(std::cbegin(config->systemFonts), std::cend(config->systemFonts), [&f](const auto& e) { return e == f.name; }); it != std::cend(config->systemFonts))
        f.index = std::distance(std::cbegin(config->systemFonts), it);
    else
        f.index = 0;
}

static void from_json(const json& j, Snapline& s)
{
    from_json(j, static_cast<ColorToggleThickness&>(s));

    read(j, "Type", s.type);
}

static void from_json(const json& j, Box& b)
{
    from_json(j, static_cast<ColorToggleRounding&>(b));

    read(j, "Type", b.type);
    read(j, "Scale", b.scale);
    read<value_t::object>(j, "Fill", b.fill);
}

static void from_json(const json& j, Shared& s)
{
    read(j, "Enabled", s.enabled);
    read<value_t::object>(j, "Font", s.font);
    read<value_t::object>(j, "Snapline", s.snapline);
    read<value_t::object>(j, "Box", s.box);
    read<value_t::object>(j, "Name", s.name);
    read(j, "Text Cull Distance", s.textCullDistance);
}

static void from_json(const json& j, Weapon& w)
{
    from_json(j, static_cast<Shared&>(w));

    read<value_t::object>(j, "Ammo", w.ammo);
}

static void from_json(const json& j, Trail& t)
{
    from_json(j, static_cast<ColorToggleThickness&>(t));

    read(j, "Type", t.type);
    read(j, "Time", t.time);
}

static void from_json(const json& j, Trails& t)
{
    read(j, "Enabled", t.enabled);
    read<value_t::object>(j, "Local Player", t.localPlayer);
    read<value_t::object>(j, "Allies", t.allies);
    read<value_t::object>(j, "Enemies", t.enemies);
}

static void from_json(const json& j, Projectile& p)
{
    from_json(j, static_cast<Shared&>(p));

    read<value_t::object>(j, "Trails", p.trails);
}

static void from_json(const json& j, Player& p)
{
    from_json(j, static_cast<Shared&>(p));

    read<value_t::object>(j, "Weapon", p.weapon);
    read<value_t::object>(j, "Flash Duration", p.flashDuration);
    read(j, "Audible Only", p.audibleOnly);
    read(j, "Spotted Only", p.spottedOnly);
    read(j, "Health Bar", p.healthBar);
    read<value_t::object>(j, "Skeleton", p.skeleton);
    read<value_t::object>(j, "Head Box", p.headBox);
}

static void from_json(const json& j, ImVec2& v)
{
    read(j, "X", v.x);
    read(j, "Y", v.y);
}

static void from_json(const json& j, Config::Aimbot& a)
{
    read(j, "Enabled", a.enabled);
    read(j, "On key", a.onKey);
    read(j, "Key", a.key);
    read(j, "Key mode", a.keyMode);
    read(j, "Aimlock", a.aimlock);
    read(j, "Silent", a.silent);
    read(j, "Friendly fire", a.friendlyFire);
    read(j, "Visible only", a.visibleOnly);
    read(j, "Scoped only", a.scopedOnly);
    read(j, "Ignore flash", a.ignoreFlash);
    read(j, "Ignore smoke", a.ignoreSmoke);
    read(j, "Auto shot", a.autoShot);
    read(j, "Auto scope", a.autoScope);
    read(j, "Fov", a.fov);
    read(j, "Smooth", a.smooth);
    read(j, "Bone", a.bone);
    read(j, "Max aim inaccuracy", a.maxAimInaccuracy);
    read(j, "Max shot inaccuracy", a.maxShotInaccuracy);
    read(j, "Min damage", a.minDamage);
    read(j, "Killshot", a.killshot);
    read(j, "Jump Check", a.jumpCheck);
    read(j, "Between shots", a.betweenShots);
    read(j, "Standalone RCS", a.standaloneRCS);
    read(j, "Standalone RCS Ignore Shots", a.shotsFired);
    read(j, "Recoil control X", a.recoilControlX);
    read(j, "Recoil control Y", a.recoilControlY);
    read(j, "Standalone RCS Random factor", a.randomRCS);
}

static void from_json(const json& j, Config::Triggerbot& t)
{
    read(j, "Enabled", t.enabled);
    read(j, "On key", t.onKey);
    read(j, "Key", t.key);
    read(j, "Friendly fire", t.friendlyFire);
    read(j, "Scoped only", t.scopedOnly);
    read(j, "Ignore flash", t.ignoreFlash);
    read(j, "Ignore smoke", t.ignoreSmoke);
    read(j, "Hitgroup", t.hitgroup);
    read(j, "Shot delay", t.shotDelay);
    read(j, "Min damage", t.minDamage);
    read(j, "Killshot", t.killshot);
    read(j, "Burst Time", t.burstTime);
    read(j, "Max aim inaccuracy", t.maxAimInaccuracy);
    read(j, "Max shot inaccuracy", t.maxShotInaccuracy);
}

static void from_json(const json& j, Config::Backtrack& b)
{
    read(j, "Enabled", b.enabled);
    read(j, "Ignore smoke", b.ignoreSmoke);
    read(j, "Recoil based fov", b.recoilBasedFov);
    read(j, "Draw all ticks", b.drawAllTicks);
    read(j, "Ping based", b.pingBased);
    read(j, "Time limit", b.timeLimit);
    read(j, "Fake Latency", b.fakeLatency);
    read(j, "Fake Latency Ammount", b.fakeLatencyAmmount);
}

static void from_json(const json& j, Config::AntiAim& a)
{
    read(j, "Enabled", a.enabled);
    read(j, "Pitch", a.pitch);
    read(j, "Yaw", a.yaw);
    read(j, "Pitch angle", a.pitchAngle);
}

static void from_json(const json& j, Config::Glow& g)
{
    from_json(j, static_cast<ColorA&>(g));

    read(j, "Enabled", g.enabled);
    read(j, "Health based", g.healthBased);
    read(j, "Style", g.style);
}

static void from_json(const json& j, Config::Chams::Material& m)
{
    from_json(j, static_cast<ColorA&>(m));

    read(j, "Enabled", m.enabled);
    read(j, "Health based", m.healthBased);
    read(j, "Blinking", m.blinking);
    read(j, "Wireframe", m.wireframe);
    read(j, "Cover", m.cover);
    read(j, "Ignore-Z", m.ignorez);
    read(j, "Material", m.material);
}

static void from_json(const json& j, Config::Chams& c)
{
    read_array_opt(j, "Materials", c.materials);
}

static void from_json(const json& j, Config::StreamProofESP& e)
{
    read(j, "Allies", e.allies);
    read(j, "Enemies", e.enemies);
    read(j, "Weapons", e.weapons);
    read(j, "Projectiles", e.projectiles);
    read(j, "Loot Crates", e.lootCrates);
    read(j, "Other Entities", e.otherEntities);
}

static void from_json(const json& j, Config::Visuals::ColorCorrection& c)
{
    read(j, "Enabled", c.enabled);
    read(j, "Blue", c.blue);
    read(j, "Red", c.red);
    read(j, "Mono", c.mono);
    read(j, "Saturation", c.saturation);
    read(j, "Ghost", c.ghost);
    read(j, "Green", c.green);
    read(j, "Yellow", c.yellow);
    read(j, "Yellow", c.yellow);
}

static void from_json(const json& j, Config::Visuals& v)
{
    read(j, "Disable post-processing", v.disablePostProcessing);
    read(j, "Inverse ragdoll gravity", v.inverseRagdollGravity);
    read(j, "Inverse ragdoll gravity Custom", v.inverseRagdollGravityCustomize);
    read(j, "Inverse ragdoll gravity Value", v.inverseRagdollGravityValue);
    read(j, "Physics Timescale", v.ragdollTimescale);
    read(j, "Physics Timescale Value", v.ragdollTimescaleValue);
    read(j, "Night Mode", v.nightMode);
    read(j, "No fog", v.noFog);
    read(j, "No 3d sky", v.no3dSky);
    read(j, "No aim punch", v.noAimPunch);
    read(j, "No view punch", v.noViewPunch);
    read(j, "No hands", v.noHands);
    read(j, "No sleeves", v.noSleeves);
    read(j, "No weapons", v.noWeapons);
    read(j, "No smoke", v.noSmoke);
    read(j, "No blur", v.noBlur);
    read(j, "No bloom", v.noBloom);
    read(j, "No scope overlay", v.noScopeOverlay);
    read(j, "No grass", v.noGrass);
    read(j, "No shadows", v.noShadows);
    read(j, "Wireframe smoke", v.wireframeSmoke);
    read<value_t::object>(j, "Show velocity", v.showvelocity);
    read(j, "Show velocity Custom", v.showvelocityM);
    read(j, "Show velocity Pos X", v.showvelocityPosX);
    read(j, "Show velocity Pos Y", v.showvelocityPosY);
    read(j, "Zoom", v.noScopeOverlay);
    read(j, "Zoom key", v.zoomKey);
    read(j, "No zoom", v.noZoom);
    read(j, "Thirdperson", v.thirdperson);
    read(j, "Dead thirdperson", v.deadThirdperson);
    read(j, "Thirdperson key", v.thirdpersonKey);
    read(j, "Thirdperson distance", v.thirdpersonDistance);
    read(j, "Viewmodel FOV", v.viewmodelFov);
    read(j, "FOV", v.fov);
    read(j, "Far Z", v.farZ);
    read(j, "Flash reduction", v.flashReduction);
    read(j, "Brightness", v.brightness);
    read(j, "Full Bright", v.fullBright);
    read(j, "Skybox", v.skybox);
    read<value_t::object>(j, "customSkybox", v.customSkybox);
    read<value_t::object>(j, "Rainbow Bar", v.rainbowBar);
    read(j, "Rainbow Up", v.rainbowUp);
    read(j, "Rainbow Bottom", v.rainbowBottom);
    read(j, "Rainbow Left", v.rainbowLeft);
    read(j, "Rainbow Right", v.rainbowRight);
    read(j, "Rainbow Scale", v.rainbowScale);
    read(j, "Rainbow Pulse", v.rainbowPulse);
    read(j, "Rainbow Pulse Speed", v.rainbowPulseSpeed);
    read<value_t::object>(j, "World", v.world);
    read<value_t::object>(j, "Sky", v.sky);
    read(j, "Deagle spinner", v.deagleSpinner);
    read(j, "Screen effect", v.screenEffect);
    read(j, "Hit effect", v.hitEffect);
    read(j, "Hit effect time", v.hitEffectTime);
    read(j, "Hit marker", v.hitMarker);
    read(j, "Hit marker time", v.hitMarkerTime);
    read<value_t::object>(j, "Hit marker damage indicator", v.hitMarkerDamageIndicator);
    read(j, "Hit marker damage indicator Customize", v.hitMarkerDamageIndicatorCustomize);
    read(j, "Hit marker damage indicator Font", v.hitMarkerDamageIndicatorFont);
    read(j, "Hit marker damage indicator Alpha", v.hitMarkerDamageIndicatorAlpha);
    read(j, "Hit marker damage indicator Dist", v.hitMarkerDamageIndicatorDist);
    read(j, "Hit marker damage indicator Text X", v.hitMarkerDamageIndicatorTextX);
    read(j, "Hit marker damage indicator Text Y", v.hitMarkerDamageIndicatorTextY);
    read(j, "Hit marker damage indicator Ratio", v.hitMarkerDamageIndicatorRatio);
    read(j, "Playermodel T", v.playerModelT);
    read(j, "Playermodel CT", v.playerModelCT);
    read<value_t::object>(j, "Bullet Tracer", v.bulletTracers);
    read<value_t::object>(j, "Color correction", v.colorCorrection);
}

static void from_json(const json& j, sticker_setting& s)
{
    read(j, "Kit", s.kit);
    read(j, "Kit vector index", s.kit_vector_index);
    read(j, "Wear", s.wear);
    read(j, "Scale", s.scale);
    read(j, "Rotation", s.rotation);
}

static void from_json(const json& j, item_setting& i)
{
    read(j, "Enabled", i.enabled);
    read(j, "Definition index", i.itemId);
    read(j, "Definition vector index", i.itemIdIndex);
    read(j, "Quality", i.quality);
    read(j, "Quality vector index", i.entity_quality_vector_index);

    read(j, "Paint Kit", i.paintKit);
    read(j, "Paint Kit vector index", i.paint_kit_vector_index);

    read(j, "Definition override", i.definition_override_index);
    read(j, "Definition override vector index", i.definition_override_vector_index);

    read(j, "Seed", i.seed);
    read(j, "StatTrak", i.stat_trak);
    read(j, "Wear", i.wear);

    if (j.contains("Custom name"))
        strncpy_s(i.custom_name, j["Custom name"].get<std::string>().c_str(), _TRUNCATE);

    read(j, "Stickers", i.stickers);
}

static void from_json(const json& j, Config::Sound::Player& p)
{
    read(j, "Master volume", p.masterVolume);
    read(j, "Headshot volume", p.headshotVolume);
    read(j, "Weapon volume", p.weaponVolume);
    read(j, "Footstep volume", p.footstepVolume);
}

static void from_json(const json& j, Config::Sound& s)
{
    read(j, "Chicken volume", s.chickenVolume);
    read(j, "Players", s.players);
}

static void from_json(const json& j, Config::Style& s)
{
    read(j, "Menu style", s.menuStyle);
    read(j, "Menu colors", s.menuColors);

    if (j.contains("Colors") && j["Colors"].is_object()) {
        const auto& colors = j["Colors"];

        ImGuiStyle& style = ImGui::GetStyle();

        for (int i = 0; i < ImGuiCol_COUNT; i++) {
            if (const char* name = ImGui::GetStyleColorName(i); colors.contains(name)) {
                std::array<float, 4> temp;
                read(colors, name, temp);
                style.Colors[i].x = temp[0];
                style.Colors[i].y = temp[1];
                style.Colors[i].z = temp[2];
                style.Colors[i].w = temp[3];
            }
        }
    }

    read<value_t::object>(j, "Custom Easy", s.customEasy);
}

static void from_json(const json& j, PurchaseList& pl)
{
    read(j, "Enabled", pl.enabled);
    read(j, "Only During Freeze Time", pl.onlyDuringFreezeTime);
    read(j, "Show Prices", pl.showPrices);
    read(j, "No Title Bar", pl.noTitleBar);
    read(j, "Mode", pl.mode);
}

static void from_json(const json& j, PreserveKillfeed& o)
{
    read(j, "Enabled", o.enabled);
    read(j, "Only Headshots", o.onlyHeadshots);
}

static void from_json(const json& j, Config::Misc& m)
{
    read(j, "Menu key", m.menuKey);
    read(j, "Anti AFK kick", m.antiAfkKick);
    read(j, "Auto strafe", m.autoStrafe);
    read(j, "Auto strafe Key", m.autoStrafeKey);
    read(j, "Auto strafe Style", m.autoStrafeStyle);
    read(j, "Bunny hop", m.bunnyHop);
    read(j, "Bunny hop hitchance Enabled", m.bhopHitchanceEnable);
    read(j, "Bunny hop hitchance", m.bhopHitchance);
    read(j, "Bunny hop Min hits", m.bhopMinHits);
    read(j, "Bunny hop Max hits", m.bhopMaxHits);
    if (j.contains("Clan tag"))
        strncpy_s(m.clanTag, j["Clan tag"].get<std::string>().c_str(), _TRUNCATE);
    read(j, "Clan tag Style", m.clanTagStyle);
    read(j, "Animated clan tag", m.animatedClanTag);
    read<value_t::object>(j, "Custom Multi ClanTag", m.customMultiClanTag);
    read(j, "Custom ClanTag Speed", m.customClanTagSpeed);
    read(j, "OsirisBETA Clan tag Speed", m.OsirisBETAClanTagSpeed);
    read(j, "Custom Multi ClanTag Speed", m.customMultiClanTagSpeed);
    read(j, "Chat spam", m.chatSpam);
    read(j, "Chat spam random", m.chatSpamRandom);
    read(j, "Chat spam delay", m.chatSpamDelay);
    read<value_t::object>(j, "Chat spam phrases", m.chatSpamPhrases);
    read(j, "Fast duck", m.fastDuck);
    read(j, "Moonwalk", m.moonwalk);
    read(j, "Edge Jump", m.edgejump);
    read(j, "Edge Jump Key", m.edgejumpkey);
    read(j, "Jump Bug", m.jumpbug);
    read(j, "Jump Bug Key", m.jumpbugkey);
    read(j, "Jump Bug Hold", m.jumpbughold);
    read(j, "Slowwalk", m.slowwalk);
    read(j, "Slowwalk key", m.slowwalkKey);
    read(j, "Door spam", m.doorSpam);
    read(j, "Buy Bot", m.buyBot);
    read(j, "Buy Bot Primary", m.buyBotPrimary);
    read(j, "Buy Bot Secondary", m.buyBotSecondary);
    read(j, "Buy Bot Vest", m.buyBotVest);
    read(j, "Buy Bot VestHelm", m.buyBotVestHelm);
    read(j, "Buy Bot Taser", m.buyBotTaser);
    read(j, "Buy Bot Defuser", m.buyBotDefuser);
    read(j, "Buy Bot Molotov", m.buyBotMolotov);
    read(j, "Buy Bot Decoy", m.buyBotDecoy);
    read(j, "Buy Bot Flashbang", m.buyBotFlashbang);
    read(j, "Buy Bot Flashbang x2", m.buyBotFlashbangX2);
    read(j, "Buy Bot HE", m.buyBotHE);
    read(j, "Buy Bot Smoke", m.buyBotSmoke);
    read<value_t::object>(j, "Noscope crosshair", m.noscopeCrosshair);
    read<value_t::object>(j, "Recoil crosshair", m.recoilCrosshair);
    read(j, "Auto pistol", m.autoPistol);
    read(j, "Auto reload", m.autoReload);
    read(j, "Auto accept", m.autoAccept);
    read(j, "Radar hack", m.radarHack);
    read(j, "Reveal ranks", m.revealRanks);
    read(j, "Reveal money", m.revealMoney);
    read(j, "Reveal suspect", m.revealSuspect);
    read<value_t::object>(j, "Spectator list", m.spectatorList);
    read<value_t::object>(j, "Watermark", m.watermark);
    read(j, "Watermark Nickname", m.watermarkNickname);
    read(j, "Watermark Username", m.watermarkUsername);
    read(j, "Watermark FPS", m.watermarkFPS);
    read(j, "Watermark Ping", m.watermarkPing);
    read(j, "Watermark Tickrate", m.watermarkTickrate);
    read(j, "Watermark Time", m.watermarkTime);
    read(j, "Watermark Pos X", m.watermarkPosX);
    read(j, "Watermark Pos Y", m.watermarkPosY);
    read(j, "Watermark Scale", m.watermarkScale);
    read(j, "Fix animation LOD", m.fixAnimationLOD);
    read(j, "Fix bone matrix", m.fixBoneMatrix);
    read(j, "Fix movement", m.fixMovement);
    read(j, "Disable model occlusion", m.disableModelOcclusion);
    read(j, "Aspect Ratio", m.aspectratio);
    read(j, "Kill message", m.killMessage);
    read<value_t::object>(j, "Kill message string", m.killMessageString);
    read(j, "Name stealer", m.nameStealer);
    read(j, "Disable HUD blur", m.disablePanoramablur);
    read(j, "Ban color", m.banColor);
    read<value_t::object>(j, "Ban text", m.banText);
    read<value_t::object>(j, "Custom Name", m.customName);
    read(j, "Fake Item", m.fakeItem);
    read(j, "Fake Item flag StatTrack", m.fakeItemFlagsST);
    read(j, "Fake Item flag Star", m.fakeItemFlagsStar);
    read(j, "Fake Item Team", m.fakeItemTeam);
    read(j, "Fake Item Message Type", m.fakeItemMessageType);
    read(j, "Fake Item Type", m.fakeItemType);
    read(j, "Fake Item Rarity", m.fakeItemRarity);
    read(j, "Fake Item Player Color", m.fakeItemPlayerColor);
    read<value_t::object>(j, "Fake Item Player Name", m.fakeItemPlayerName);
    read<value_t::object>(j, "Fake Item Skin Name", m.fakeItemName);
    read(j, "Fast plant", m.fastPlant);
    read<value_t::object>(j, "Bomb timer", m.bombTimer);
    read(j, "Bomb Damage Indicator", m.bombDamage);
    read(j, "Quick reload", m.quickReload);
    read(j, "Prepare revolver", m.prepareRevolver);
    read(j, "Prepare revolver key", m.prepareRevolverKey);
    read(j, "Hit sound", m.hitSound);
    read(j, "Choked packets", m.chokedPackets);
    read(j, "Choked packets On Shooting", m.chokedPacketsShooting);
    read(j, "Choked packets On Standing", m.chokedPacketsStanding);
    read(j, "Choked packets On Moving", m.chokedPacketsMoving);
    read(j, "Choked packets In Air", m.chokedPacketsAir);
    read(j, "Choked packets Ticks", m.chokedPacketsTicks);
    read(j, "Choked packets key", m.chokedPacketsKey);
    read(j, "Choked packets Ping Based", m.pingBasedChoked);
    read(j, "Quick healthshot key", m.quickHealthshotKey);
    read(j, "Grenade predict", m.nadePredict);
    read(j, "Fix tablet signal", m.fixTabletSignal);
    read(j, "Max angle delta", m.maxAngleDelta);
    read(j, "Fake prime", m.fakePrime);
    read(j, "Zeusbot", m.autoZeus);
    read(j, "Zeus Max Wall Penetration Distance", m.autoZeusMaxPenDist);
    read(j, "Bypass sv_pure", m.svpurebypass);
    read(j, "Bypass sv_pure (OLD)", m.svpurebypassOLD);
    read(j, "Draw aimbot FOV", m.drawAimbotFov);
    read<value_t::object>(j, "Draw Inaccuracy", m.drawInaccuracy);
    read(j, "Team Damage Counter", m.teamDamageCounter);
    read(j, "Player Blocker", m.playerBlocker);
    read(j, "Player Blocker Key", m.playerBlockerKey);
    read(j, "Fix tablet signal", m.fixTabletSignal);
    read<value_t::object>(j, "Custom Hit Sound", m.customHitSound);
    read(j, "Kill sound", m.killSound);
    read<value_t::object>(j, "Custom Kill Sound", m.customKillSound);
    read(j, "Fake Duck", m.fakeDuck);
    read(j, "Fake Duck Bind", m.fakeDuckKey);
    read<value_t::object>(j, "Purchase List", m.purchaseList);
    read<value_t::object>(j, "Reportbot", m.reportbot);
    read(j, "Custom Viewmodel", m.customViewmodelToggle);
    read(j, "Custom Viewmodel X", m.viewmodel_x);
    read(j, "Custom Viewmodel Y", m.viewmodel_y);
    read(j, "Custom Viewmodel Z", m.viewmodel_z);
    read(j, "Custom Viewmodel Knife", m.customViewmodelKnifeToggle);
    read(j, "Custom Viewmodel Switch", m.customViewmodelSwitchHand);
    read(j, "Custom Viewmodel Switch Bind", m.customViewmodelSwitchHandBind);
    read(j, "Custom Viewmodel Knife X", m.viewmodel_x_knife);
    read(j, "Custom Viewmodel Knife Y", m.viewmodel_y_knife);
    read(j, "Custom Viewmodel Knife Z", m.viewmodel_z_knife);
    read(j, "Custom Viewmodel HeadBob", m.view_bob);
    read(j, "Opposite Hand Knife", m.oppositeHandKnife);
    read(j, "Opposite Hand Knife Bind", m.oppositeHandKnifeBind);
    read<value_t::object>(j, "Preserve Killfeed", m.preserveKillfeed);
}

static void from_json(const json& j, Config::Misc::Reportbot& r)
{
    read(j, "Enabled", r.enabled);
    read(j, "Target", r.target);
    read(j, "Delay", r.delay);
    read(j, "Rounds", r.rounds);
    read(j, "Abusive Communications", r.textAbuse);
    read(j, "Griefing", r.griefing);
    read(j, "Wall Hacking", r.wallhack);
    read(j, "Aim Hacking", r.aimbot);
    read(j, "Other Hacking", r.other);
}

static void from_json(const json& j, Config::Style::StyleCustomEasy& s)
{
    read<value_t::object>(j, "Background Color", s.BackGroundColor);
    read<value_t::object>(j, "Text Color", s.TextColor);
    read<value_t::object>(j, "Main Color", s.MainColor);
    read<value_t::object>(j, "Main Accent Color", s.MainAccentColor);
    read<value_t::object>(j, "Highlight Color", s.HighlightColor);
}

void Config::load(size_t id, bool incremental) noexcept
{
    json j;

    if (std::ifstream in{ path / (const char8_t*)configs[id].c_str() }; in.good())
        in >> j;
    else
        return;

    if (!incremental)
        reset();

    read(j, "Aimbot", aimbot);
    read(j, "Triggerbot", triggerbot);
    read<value_t::object>(j, "Backtrack", backtrack);
    read<value_t::object>(j, "Anti aim", antiAim);
    read(j, "Glow", glow);
    read(j, "Chams", chams);
    read<value_t::object>(j, "ESP", streamProofESP);
    read<value_t::object>(j, "Visuals", visuals);
    read(j, "Skin changer", skinChanger);
    read<value_t::object>(j, "Sound", sound);
    read<value_t::object>(j, "Style", style);
    read<value_t::object>(j, "Misc", misc);
}

// WRITE macro requires:
// - json object named 'j'
// - object holding default values named 'dummy'
// - object to write to json named 'o'
#define WRITE(name, valueName) to_json(j[name], o.valueName, dummy.valueName)

template <typename T>
static void to_json(json& j, const T& o, const T& dummy)
{
    if (o != dummy)
        j = o;
}

static void to_json(json& j, const ColorA& o, const ColorA& dummy = {})
{
    WRITE("Color", color);
    WRITE("Rainbow", rainbow);
    WRITE("Rainbow Speed", rainbowSpeed);
}

static void to_json(json& j, const ColorToggle& o, const ColorToggle& dummy = {})
{
    to_json(j, static_cast<const ColorA&>(o), dummy);
    WRITE("Enabled", enabled);
}

static void to_json(json& j, const Config::Color& o, const Config::Color& dummy = {})
{
    WRITE("Color", color);
    WRITE("Rainbow", rainbow);
    WRITE("Rainbow Speed", rainbowSpeed);
}

static void to_json(json& j, const Config::ColorToggle& o, const Config::ColorToggle& dummy = {})
{
    to_json(j, static_cast<const Config::Color&>(o), dummy);
    WRITE("Enabled", enabled);
}

static void to_json(json& j, const ColorToggleRounding& o, const ColorToggleRounding& dummy = {})
{
    to_json(j, static_cast<const ColorToggle&>(o), dummy);
    WRITE("Rounding", rounding);
}

static void to_json(json& j, const ColorToggleThickness& o, const ColorToggleThickness& dummy = {})
{
    to_json(j, static_cast<const ColorToggle&>(o), dummy);
    WRITE("Thickness", thickness);
}

static void to_json(json& j, const ColorToggleThicknessRounding& o, const ColorToggleThicknessRounding& dummy = {})
{
    to_json(j, static_cast<const ColorToggleRounding&>(o), dummy);
    WRITE("Thickness", thickness);
}

static void to_json(json& j, const Font& o, const Font& dummy = {})
{
    WRITE("Name", name);
}

static void to_json(json& j, const Snapline& o, const Snapline& dummy = {})
{
    to_json(j, static_cast<const ColorToggleThickness&>(o), dummy);
    WRITE("Type", type);
}

static void to_json(json& j, const Box& o, const Box& dummy = {})
{
    to_json(j, static_cast<const ColorToggleRounding&>(o), dummy);
    WRITE("Type", type);
    WRITE("Scale", scale);
    WRITE("Fill", fill);
}

static void to_json(json& j, const Shared& o, const Shared& dummy = {})
{
    WRITE("Enabled", enabled);
    WRITE("Font", font);
    WRITE("Snapline", snapline);
    WRITE("Box", box);
    WRITE("Name", name);
    WRITE("Text Cull Distance", textCullDistance);
}

static void to_json(json& j, const Player& o, const Player& dummy = {})
{
    to_json(j, static_cast<const Shared&>(o), dummy);
    WRITE("Weapon", weapon);
    WRITE("Flash Duration", flashDuration);
    WRITE("Audible Only", audibleOnly);
    WRITE("Spotted Only", spottedOnly);
    WRITE("Health Bar", healthBar);
    WRITE("Skeleton", skeleton);
    WRITE("Head Box", headBox);
}

static void to_json(json& j, const Weapon& o, const Weapon& dummy = {})
{
    to_json(j, static_cast<const Shared&>(o), dummy);
    WRITE("Ammo", ammo);
}

static void to_json(json& j, const Trail& o, const Trail& dummy = {})
{
    to_json(j, static_cast<const ColorToggleThickness&>(o), dummy);
    WRITE("Type", type);
    WRITE("Time", time);
}

static void to_json(json& j, const Trails& o, const Trails& dummy = {})
{
    WRITE("Enabled", enabled);
    WRITE("Local Player", localPlayer);
    WRITE("Allies", allies);
    WRITE("Enemies", enemies);
}

static void to_json(json& j, const Projectile& o, const Projectile& dummy = {})
{
    j = static_cast<const Shared&>(o);

    WRITE("Trails", trails);
}

static void to_json(json& j, const ImVec2& o, const ImVec2& dummy = {})
{
    WRITE("X", x);
    WRITE("Y", y);
}

static void to_json(json& j, const Config::Aimbot& o, const Config::Aimbot& dummy = {})
{
    WRITE("Enabled", enabled);
    WRITE("On key", onKey);
    WRITE("Key", key);
    WRITE("Key mode", keyMode);
    WRITE("Aimlock", aimlock);
    WRITE("Silent", silent);
    WRITE("Friendly fire", friendlyFire);
    WRITE("Visible only", visibleOnly);
    WRITE("Scoped only", scopedOnly);
    WRITE("Ignore flash", ignoreFlash);
    WRITE("Ignore smoke", ignoreSmoke);
    WRITE("Auto shot", autoShot);
    WRITE("Auto scope", autoScope);
    WRITE("Fov", fov);
    WRITE("Smooth", smooth);
    WRITE("Bone", bone);
    WRITE("Max aim inaccuracy", maxAimInaccuracy);
    WRITE("Max shot inaccuracy", maxShotInaccuracy);
    WRITE("Min damage", minDamage);
    WRITE("Killshot", killshot);
    WRITE("Jump Check", jumpCheck);
    WRITE("Between shots", betweenShots);
    WRITE("Recoil control X", recoilControlX);
    WRITE("Recoil control Y", recoilControlY);
    WRITE("Standalone RCS", standaloneRCS);
    WRITE("Standalone RCS Ignore Shots", shotsFired);
    WRITE("Standalone RCS Random factor", randomRCS);
}

static void to_json(json& j, const Config::Triggerbot& o, const Config::Triggerbot& dummy = {})
{
    WRITE("Enabled", enabled);
    WRITE("On key", onKey);
    WRITE("Key", key);
    WRITE("Friendly fire", friendlyFire);
    WRITE("Scoped only", scopedOnly);
    WRITE("Ignore flash", ignoreFlash);
    WRITE("Ignore smoke", ignoreSmoke);
    WRITE("Hitgroup", hitgroup);
    WRITE("Shot delay", shotDelay);
    WRITE("Min damage", minDamage);
    WRITE("Killshot", killshot);
    WRITE("Burst Time", burstTime);
    WRITE("Max aim inaccuracy", maxAimInaccuracy);
    WRITE("Max shot inaccuracy", maxShotInaccuracy);
}

static void to_json(json& j, const Config::Backtrack& o, const Config::Backtrack& dummy = {})
{
    WRITE("Enabled", enabled);
    WRITE("Ignore smoke", ignoreSmoke);
    WRITE("Recoil based fov", recoilBasedFov);
    WRITE("Draw all ticks", drawAllTicks);
    WRITE("Ping based", pingBased);
    WRITE("Time limit", timeLimit);
    WRITE("Fake Latency", fakeLatency);
    WRITE("Fake Latency Ammount", fakeLatencyAmmount);
}

static void to_json(json& j, const Config::AntiAim& o, const Config::AntiAim& dummy = {})
{
    WRITE("Enabled", enabled);
    WRITE("Pitch", pitch);
    WRITE("Pitch angle", pitchAngle);
    WRITE("Yaw", yaw);
}

static void to_json(json& j, const Config::Glow& o, const Config::Glow& dummy = {})
{
    to_json(j, static_cast<const ColorA&>(o), dummy);
    WRITE("Enabled", enabled);
    WRITE("Health based", healthBased);
    WRITE("Style", style);
}

static void to_json(json& j, const Config::Chams::Material& o)
{
    const Config::Chams::Material dummy;

    to_json(j, static_cast<const ColorA&>(o), dummy);
    WRITE("Enabled", enabled);
    WRITE("Health based", healthBased);
    WRITE("Blinking", blinking);
    WRITE("Wireframe", wireframe);
    WRITE("Cover", cover);
    WRITE("Ignore-Z", ignorez);
    WRITE("Material", material);
}

static void to_json(json& j, const Config::Chams& o)
{
    j["Materials"] = o.materials;
}

static void to_json(json& j, const Config::StreamProofESP& o)
{
    j["Allies"] = o.allies;
    j["Enemies"] = o.enemies;
    j["Weapons"] = o.weapons;
    j["Projectiles"] = o.projectiles;
    j["Loot Crates"] = o.lootCrates;
    j["Other Entities"] = o.otherEntities;
}

static void to_json(json& j, const Config::Misc::Reportbot& o, const Config::Misc::Reportbot& dummy = {})
{
    WRITE("Enabled", enabled);
    WRITE("Target", target);
    WRITE("Delay", delay);
    WRITE("Rounds", rounds);
    WRITE("Abusive Communications", textAbuse);
    WRITE("Griefing", griefing);
    WRITE("Wall Hacking", wallhack);
    WRITE("Aim Hacking", aimbot);
    WRITE("Other Hacking", other);
}
static void to_json(json& j, const Config::Style::StyleCustomEasy& o, const Config::Style::StyleCustomEasy& dummy = {})
{
    WRITE("Background Color", BackGroundColor);
    WRITE("Text Color", TextColor);
    WRITE("Main Color", MainColor);
    WRITE("Main Accent Color", MainAccentColor);
    WRITE("Highlight Color", HighlightColor);
}

static void to_json(json& j, const Config::Sound::Player& o)
{
    const Config::Sound::Player dummy;

    WRITE("Master volume", masterVolume);
    WRITE("Headshot volume", headshotVolume);
    WRITE("Weapon volume", weaponVolume);
    WRITE("Footstep volume", footstepVolume);
}

static void to_json(json& j, const Config::Sound& o)
{
    const Config::Sound dummy;

    WRITE("Chicken volume", chickenVolume);
    j["Players"] = o.players;
}

static void to_json(json& j, const PurchaseList& o, const PurchaseList& dummy = {})
{
    WRITE("Enabled", enabled);
    WRITE("Only During Freeze Time", onlyDuringFreezeTime);
    WRITE("Show Prices", showPrices);
    WRITE("No Title Bar", noTitleBar);
    WRITE("Mode", mode);
}

static void to_json(json& j, const PreserveKillfeed& o, const PreserveKillfeed& dummy = {})
{
    WRITE("Enabled", enabled);
    WRITE("Only Headshots", onlyHeadshots);
}

static void to_json(json& j, const Config::Misc& o)
{
    const Config::Misc dummy;

    WRITE("Menu key", menuKey);
    WRITE("Anti AFK kick", antiAfkKick);
    WRITE("Auto strafe", autoStrafe);
    WRITE("Auto strafe Key", autoStrafeKey);
    WRITE("Auto strafe Style", autoStrafeStyle);
    WRITE("Bunny hop", bunnyHop);
    WRITE("Bunny hop hitchance Enabled", bhopHitchanceEnable);
    WRITE("Bunny hop hitchance", bhopHitchance);
    WRITE("Bunny hop Min hits", bhopMinHits);
    WRITE("Bunny hop Max hits", bhopMaxHits);

    if (o.clanTag[0])
        j["Clan tag"] = o.clanTag;

    WRITE("Clan tag Style", clanTagStyle);
    WRITE("Animated clan tag", animatedClanTag);
    WRITE("Custom Multi ClanTag", customMultiClanTag);
    WRITE("Custom ClanTag Speed", customClanTagSpeed);
    WRITE("OsirisBETA Clan tag Speed", OsirisBETAClanTagSpeed);
    WRITE("Custom Multi ClanTag Speed", customMultiClanTagSpeed);
    WRITE("Chat spam", chatSpam);
    WRITE("Chat spam random", chatSpamRandom);
    WRITE("Chat spam delay", chatSpamDelay);
    WRITE("Chat spam phrases", chatSpamPhrases);
    WRITE("Fast duck", fastDuck);
    WRITE("Moonwalk", moonwalk);
    WRITE("Edge Jump", edgejump);
    WRITE("Edge Jump Key", edgejumpkey);
    WRITE("Jump Bug", jumpbug);
    WRITE("Jump Bug Key", jumpbugkey);
    WRITE("Jump Bug Hold", jumpbughold);
    WRITE("Slowwalk", slowwalk);
    WRITE("Slowwalk key", slowwalkKey);
    WRITE("Door spam", doorSpam);
    WRITE("Buy Bot", buyBot);
    WRITE("Buy Bot Primary", buyBotPrimary);
    WRITE("Buy Bot Secondary", buyBotSecondary);
    WRITE("Buy Bot Vest", buyBotVest);
    WRITE("Buy Bot VestHelm", buyBotVestHelm);
    WRITE("Buy Bot Taser", buyBotTaser);
    WRITE("Buy Bot Defuser", buyBotDefuser);
    WRITE("Buy Bot Molotov", buyBotMolotov);
    WRITE("Buy Bot Decoy", buyBotDecoy);
    WRITE("Buy Bot Flashbang", buyBotFlashbang);
    WRITE("Buy Bot Flashbang x2", buyBotFlashbangX2);
    WRITE("Buy Bot HE", buyBotHE);
    WRITE("Buy Bot Smoke", buyBotSmoke);
    WRITE("Noscope crosshair", noscopeCrosshair);
    WRITE("Recoil crosshair", recoilCrosshair);
    WRITE("Auto pistol", autoPistol);
    WRITE("Auto reload", autoReload);
    WRITE("Auto accept", autoAccept);
    WRITE("Radar hack", radarHack);
    WRITE("Reveal ranks", revealRanks);
    WRITE("Reveal money", revealMoney);
    WRITE("Reveal suspect", revealSuspect);
    WRITE("Spectator list", spectatorList);
    WRITE("Watermark", watermark);
    WRITE("Watermark Nickname", watermarkNickname);
    WRITE("Watermark Username", watermarkUsername);
    WRITE("Watermark FPS", watermarkFPS);
    WRITE("Watermark Ping", watermarkPing);
    WRITE("Watermark Tickrate", watermarkTickrate);
    WRITE("Watermark Time", watermarkTime);
    WRITE("Watermark Pos X", watermarkPosX);
    WRITE("Watermark Pos Y", watermarkPosY);
    WRITE("Watermark Scale", watermarkScale);
    WRITE("Fix animation LOD", fixAnimationLOD);
    WRITE("Fix bone matrix", fixBoneMatrix);
    WRITE("Fix movement", fixMovement);
    WRITE("Disable model occlusion", disableModelOcclusion);
    WRITE("Aspect Ratio", aspectratio);
    WRITE("Kill message", killMessage);
    WRITE("Kill message string", killMessageString);
    WRITE("Name stealer", nameStealer);
    WRITE("Disable HUD blur", disablePanoramablur);
    WRITE("Ban color", banColor);
    WRITE("Ban text", banText);
    WRITE("Custom Name", customName);
    WRITE("Fake Item", fakeItem);
    WRITE("Fake Item flag StatTrack", fakeItemFlagsST);
    WRITE("Fake Item flag Star", fakeItemFlagsStar);
    WRITE("Fake Item Team", fakeItemTeam);
    WRITE("Fake Item Message Type", fakeItemMessageType);
    WRITE("Fake Item Type", fakeItemType);
    WRITE("Fake Item Rarity", fakeItemRarity);
    WRITE("Fake Item Player Color", fakeItemPlayerColor);
    WRITE("Fake Item Player Name", fakeItemPlayerName);
    WRITE("Fake Item Skin Name", fakeItemName);
    WRITE("Fast plant", fastPlant);
    WRITE("Bomb timer", bombTimer);
    WRITE("Bomb Damage Indicator", bombDamage);
    WRITE("Quick reload", quickReload);
    WRITE("Prepare revolver", prepareRevolver);
    WRITE("Prepare revolver key", prepareRevolverKey);
    WRITE("Hit sound", hitSound);
    WRITE("Choked packets", chokedPackets);
    WRITE("Choked packets On Shooting", chokedPacketsShooting);
    WRITE("Choked packets On Standing", chokedPacketsStanding);
    WRITE("Choked packets On Moving", chokedPacketsMoving);
    WRITE("Choked packets In Air", chokedPacketsAir);
    WRITE("Choked packets Ticks", chokedPacketsTicks);
    WRITE("Choked packets key", chokedPacketsKey);
    WRITE("Choked packets Ping Based", pingBasedChoked);
    WRITE("Quick healthshot key", quickHealthshotKey);
    WRITE("Grenade predict", nadePredict);
    WRITE("Fix tablet signal", fixTabletSignal);
    WRITE("Max angle delta", maxAngleDelta);
    WRITE("Fake prime", fakePrime);
    WRITE("Zeusbot", autoZeus);
    WRITE("Zeus Max Wall Penetration Distance", autoZeusMaxPenDist);
    WRITE("Bypass sv_pure", svpurebypass);
    WRITE("Bypass sv_pure (OLD)", svpurebypassOLD);
    WRITE("Draw aimbot FOV", drawAimbotFov);
    WRITE("Draw Inaccuracy", drawInaccuracy);
    WRITE("Team Damage Counter", teamDamageCounter);
    WRITE("Player Blocker", playerBlocker);
    WRITE("Player Blocker Key", playerBlockerKey);
    WRITE("Fix tablet signal", fixTabletSignal);
    WRITE("Custom Hit Sound", customHitSound);
    WRITE("Kill sound", killSound);
    WRITE("Custom Kill Sound", customKillSound);
    WRITE("Fake Duck", fakeDuck);
    WRITE("Fake Duck Bind", fakeDuckKey);
    WRITE("Purchase List", purchaseList);
    WRITE("Reportbot", reportbot);
    WRITE("Custom Viewmodel", customViewmodelToggle);
    WRITE("Custom Viewmodel X", viewmodel_x);
    WRITE("Custom Viewmodel Y", viewmodel_y);
    WRITE("Custom Viewmodel Z", viewmodel_z);
    WRITE("Custom Viewmodel Knife", customViewmodelKnifeToggle);
    WRITE("Custom Viewmodel Switch", customViewmodelSwitchHand);
    WRITE("Custom Viewmodel Switch Bind", customViewmodelSwitchHandBind);
    WRITE("Custom Viewmodel Knife X", viewmodel_x_knife);
    WRITE("Custom Viewmodel Knife Y", viewmodel_y_knife);
    WRITE("Custom Viewmodel Knife Z", viewmodel_z_knife);
    WRITE("Custom Viewmodel HeadBob", view_bob);
    WRITE("Opposite Hand Knife", oppositeHandKnife);
    WRITE("Opposite Hand Knife Bind", oppositeHandKnifeBind);
    WRITE("Preserve Killfeed", preserveKillfeed);
}

static void to_json(json& j, const Config::Visuals::ColorCorrection& o, const Config::Visuals::ColorCorrection& dummy)
{
    WRITE("Enabled", enabled);
    WRITE("Blue", blue);
    WRITE("Red", red);
    WRITE("Mono", mono);
    WRITE("Saturation", saturation);
    WRITE("Ghost", ghost);
    WRITE("Green", green);
    WRITE("Yellow", yellow);
}

static void to_json(json& j, const Config::Visuals& o)
{
    const Config::Visuals dummy;

    WRITE("Disable post-processing", disablePostProcessing);
    WRITE("Inverse ragdoll gravity", inverseRagdollGravity);
    WRITE("Inverse ragdoll gravity Custom", inverseRagdollGravityCustomize);
    WRITE("Inverse ragdoll gravity Value", inverseRagdollGravityValue);
    WRITE("Physics Timescale", ragdollTimescale);
    WRITE("Physics Timescale Value", ragdollTimescaleValue);
    WRITE("Night Mode", nightMode);
    WRITE("No fog", noFog);
    WRITE("No 3d sky", no3dSky);
    WRITE("No aim punch", noAimPunch);
    WRITE("No view punch", noViewPunch);
    WRITE("No hands", noHands);
    WRITE("No sleeves", noSleeves);
    WRITE("No weapons", noWeapons);
    WRITE("No smoke", noSmoke);
    WRITE("No blur", noBlur);
    WRITE("No bloom", noBloom);
    WRITE("No scope overlay", noScopeOverlay);
    WRITE("No grass", noGrass);
    WRITE("No shadows", noShadows);
    WRITE("Wireframe smoke", wireframeSmoke);
    WRITE("Show velocity", showvelocity);
    WRITE("Show velocity Custom", showvelocityM);
    WRITE("Show velocity Pos X", showvelocityPosX);
    WRITE("Show velocity Pos Y", showvelocityPosY);
    WRITE("Zoom", noScopeOverlay);
    WRITE("Zoom key", zoomKey);
    WRITE("No zoom", noZoom);
    WRITE("Thirdperson", thirdperson);
    WRITE("Dead thirdperson", deadThirdperson);
    WRITE("Thirdperson key", thirdpersonKey);
    WRITE("Thirdperson distance", thirdpersonDistance);
    WRITE("Viewmodel FOV", viewmodelFov);
    WRITE("FOV", fov);
    WRITE("Far Z", farZ);
    WRITE("Flash reduction", flashReduction);
    WRITE("Brightness", brightness);
    WRITE("Full Bright", fullBright);
    WRITE("Skybox", skybox);
    WRITE("Rainbow Bar", rainbowBar);
    WRITE("Rainbow Up", rainbowUp);
    WRITE("Rainbow Bottom", rainbowBottom);
    WRITE("Rainbow Left", rainbowLeft);
    WRITE("Rainbow Right", rainbowRight);
    WRITE("Rainbow Scale", rainbowScale);
    WRITE("Rainbow Pulse", rainbowPulse);
    WRITE("Rainbow Pulse Speed", rainbowPulseSpeed);
    WRITE("World", world);
    WRITE("Sky", sky);
    WRITE("customSkybox", customSkybox);
    WRITE("Deagle spinner", deagleSpinner);
    WRITE("Screen effect", screenEffect);
    WRITE("Hit effect", hitEffect);
    WRITE("Hit effect time", hitEffectTime);
    WRITE("Hit marker", hitMarker);
    WRITE("Hit marker time", hitMarkerTime);
    WRITE("Hit marker damage indicator", hitMarkerDamageIndicator);
    WRITE("Hit marker damage indicator Customize", hitMarkerDamageIndicatorCustomize);
    WRITE("Hit marker damage indicator Font", hitMarkerDamageIndicatorFont);
    WRITE("Hit marker damage indicator Alpha", hitMarkerDamageIndicatorAlpha);
    WRITE("Hit marker damage indicator Dist", hitMarkerDamageIndicatorDist);
    WRITE("Hit marker damage indicator Text X", hitMarkerDamageIndicatorTextX);
    WRITE("Hit marker damage indicator Text Y", hitMarkerDamageIndicatorTextY);
    WRITE("Hit marker damage indicator Ratio", hitMarkerDamageIndicatorRatio);
    WRITE("Playermodel T", playerModelT);
    WRITE("Playermodel CT", playerModelCT);
    WRITE("Bullet Tracer", bulletTracers);
    WRITE("Color correction", colorCorrection);
}

static void to_json(json& j, const ImVec4& o)
{
    j[0] = o.x;
    j[1] = o.y;
    j[2] = o.z;
    j[3] = o.w;
}

static void to_json(json& j, const Config::Style& o)
{
    const Config::Style dummy;

    WRITE("Menu style", menuStyle);
    WRITE("Menu colors", menuColors);

    auto& colors = j["Colors"];
    ImGuiStyle& style = ImGui::GetStyle();

    for (int i = 0; i < ImGuiCol_COUNT; i++)
        colors[ImGui::GetStyleColorName(i)] = style.Colors[i];

    WRITE("Custom Easy", customEasy);
}

static void to_json(json& j, const sticker_setting& o)
{
    const sticker_setting dummy;

    WRITE("Kit", kit);
    WRITE("Kit vector index", kit_vector_index);
    WRITE("Wear", wear);
    WRITE("Scale", scale);
    WRITE("Rotation", rotation);
}

static void to_json(json& j, const item_setting& o)
{
    const item_setting dummy;

    WRITE("Enabled", enabled);
    WRITE("Definition index", itemId);
    WRITE("Definition vector index", itemIdIndex);
    WRITE("Quality", quality);
    WRITE("Quality vector index", entity_quality_vector_index);
    WRITE("Paint Kit", paintKit);
    WRITE("Paint Kit vector index", paint_kit_vector_index);
    WRITE("Definition override", definition_override_index);
    WRITE("Definition override vector index", definition_override_vector_index);
    WRITE("Seed", seed);
    WRITE("StatTrak", stat_trak);
    WRITE("Wear", wear);
    if (o.custom_name[0])
        j["Custom name"] = o.custom_name;
    WRITE("Stickers", stickers);
}

void removeEmptyObjects(json& j) noexcept
{
    for (auto it = j.begin(); it != j.end();) {
        auto& val = it.value();
        if (val.is_object() || val.is_array())
            removeEmptyObjects(val);
        if (val.empty() && !j.is_array())
            it = j.erase(it);
        else
            ++it;
    }
}

void Config::save(size_t id) const noexcept
{
    std::error_code ec;
    std::filesystem::create_directory(path, ec);

    if (std::ofstream out{ path / (const char8_t*)configs[id].c_str() }; out.good()) {
        json j;

        j["Aimbot"] = aimbot;
        j["Triggerbot"] = triggerbot;
        j["Backtrack"] = backtrack;
        j["Anti aim"] = antiAim;
        j["Glow"] = glow;
        j["Chams"] = chams;
        j["ESP"] = streamProofESP;
        j["Sound"] = sound;
        j["Visuals"] = visuals;
        j["Misc"] = misc;
        j["Style"] = style;
        j["Skin changer"] = skinChanger;

        removeEmptyObjects(j);
        out << std::setw(2) << j;
    }
}

void Config::add(const char* name) noexcept
{
    if (*name && std::find(configs.cbegin(), configs.cend(), name) == configs.cend()) {
        configs.emplace_back(name);
        save(configs.size() - 1);
    }
}

void Config::remove(size_t id) noexcept
{
    std::error_code ec;
    std::filesystem::remove(path / (const char8_t*)configs[id].c_str(), ec);
    configs.erase(configs.cbegin() + id);
}

void Config::rename(size_t item, const char* newName) noexcept
{
    std::error_code ec;
    std::filesystem::rename(path / (const char8_t*)configs[item].c_str(), path / (const char8_t*)newName, ec);
    configs[item] = newName;
}

void Config::reset() noexcept
{
    aimbot = { };
    triggerbot = { };
    backtrack = { };
    glow = { };
    chams = { };
    streamProofESP = { };
    visuals = { };
    skinChanger = { };
    sound = { };
    style = { };
    misc = { };
}

void Config::listConfigs() noexcept
{
    configs.clear();

    std::error_code ec;
    std::transform(std::filesystem::directory_iterator{ path, ec },
        std::filesystem::directory_iterator{ },
        std::back_inserter(configs),
        [](const auto& entry) { return std::string{ (const char*)entry.path().filename().u8string().c_str() }; });
}

void Config::scheduleFontLoad(const std::string& name) noexcept
{
    scheduledFonts.push_back(name);
}

#ifdef _WIN32
static auto getFontData(const std::string& fontName) noexcept
{
    HFONT font = CreateFontA(0, 0, 0, 0,
        FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH, fontName.c_str());

    std::unique_ptr<std::byte[]> data;
    DWORD dataSize = GDI_ERROR;

    if (font) {
        HDC hdc = CreateCompatibleDC(nullptr);

        if (hdc) {
            SelectObject(hdc, font);
            dataSize = GetFontData(hdc, 0, 0, nullptr, 0);

            if (dataSize != GDI_ERROR) {
                data = std::make_unique<std::byte[]>(dataSize);
                dataSize = GetFontData(hdc, 0, 0, data.get(), dataSize);

                if (dataSize == GDI_ERROR)
                    data.reset();
            }
            DeleteDC(hdc);
        }
        DeleteObject(font);
    }
    return std::make_pair(std::move(data), dataSize);
}
#endif

bool Config::loadScheduledFonts() noexcept
{
    bool result = false;

    for (const auto& fontName : scheduledFonts) {
        if (fontName == "Default") {
            if (fonts.find("Default") == fonts.cend()) {
                ImFontConfig cfg;
                cfg.OversampleH = cfg.OversampleV = 1;
                cfg.PixelSnapH = true;

                Font newFont;

                cfg.SizePixels = 13.0f;
                newFont.big = ImGui::GetIO().Fonts->AddFontDefault(&cfg);

                cfg.SizePixels = 10.0f;
                newFont.medium = ImGui::GetIO().Fonts->AddFontDefault(&cfg);

                cfg.SizePixels = 8.0f;
                newFont.tiny = ImGui::GetIO().Fonts->AddFontDefault(&cfg);

                fonts.emplace(fontName, newFont);
                result = true;
            }
            continue;
        }

#ifdef _WIN32
        const auto [fontData, fontDataSize] = getFontData(fontName);
        if (fontDataSize == GDI_ERROR)
            continue;

        if (fonts.find(fontName) == fonts.cend()) {
            const auto ranges = Helpers::getFontGlyphRanges();
            ImFontConfig cfg;
            cfg.FontDataOwnedByAtlas = false;

            Font newFont;
            newFont.tiny = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(fontData.get(), fontDataSize, 8.0f, &cfg, ranges);
            newFont.medium = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(fontData.get(), fontDataSize, 10.0f, &cfg, ranges);
            newFont.big = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(fontData.get(), fontDataSize, 13.0f, &cfg, ranges);
            fonts.emplace(fontName, newFont);
            result = true;
        }
#endif
    }
    scheduledFonts.clear();
    return result;
}
