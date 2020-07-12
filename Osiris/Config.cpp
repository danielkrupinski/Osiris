#include <fstream>
#include <ShlObj.h>

#include "json/json.h"
#include "nlohmann/json.hpp"

#include "Config.h"

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
    if (PWSTR pathToDocuments; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &pathToDocuments))) {
        path = pathToDocuments;
        path /= name;
        CoTaskMemFree(pathToDocuments);
    }

    listConfigs();
    misc.clanTag[0] = '\0';

    LOGFONTA logfont;
    logfont.lfCharSet = ANSI_CHARSET;
    logfont.lfPitchAndFamily = DEFAULT_PITCH;
    logfont.lfFaceName[0] = '\0';

    EnumFontFamiliesExA(GetDC(nullptr), &logfont, fontCallback, (LPARAM)&systemFonts, 0);
    std::sort(std::next(systemFonts.begin()), systemFonts.end());
}

using json = nlohmann::basic_json<std::map, std::vector, std::string, bool, std::int64_t, std::uint64_t, float>;
using value_t = json::value_t;

template <value_t Type, typename T>
static void read(const json& j, const char* key, T& o) noexcept
{
    if (j.contains(key) && j[key].type() == Type)
        o = j[key];
}

template <typename T>
static void read_vector(const json& j, const char* key, std::vector<T>& o) noexcept
{
    if (j.contains(key) && j[key].type() == value_t::array)
        o = j[key].get<std::vector<T>>();
}

template <value_t Type, typename T, size_t Size>
static void read(const json& j, const char* key, std::array<T, Size>& o) noexcept
{
    if (j.contains(key) && j[key].type() == Type && j[key].size() == o.size())
        o = j[key];
}

template <typename T>
static void read_number(const json& j, const char* key, T& o) noexcept
{
    if (j.contains(key) && j[key].is_number())
        o = j[key];
}

template <typename T>
static void read_map(const json& j, const char* key, std::unordered_map<std::string, T>& o) noexcept
{
    if (j.contains(key) && j[key].is_object()) {
        for (auto& element : j[key].items())
            o[element.key()] = static_cast<const T&>(element.value());
    }
}

static void from_json(const json& j, ColorA& c)
{
    read<value_t::array>(j, "Color", c.color);
    read<value_t::boolean>(j, "Rainbow", c.rainbow);
    read_number(j, "Rainbow Speed", c.rainbowSpeed);
}

static void from_json(const json& j, ColorToggle& ct)
{
    from_json(j, static_cast<ColorA&>(ct));

    read<value_t::boolean>(j, "Enabled", ct.enabled);
}

static void from_json(const json& j, ColorToggleRounding& ctr)
{
    from_json(j, static_cast<ColorToggle&>(ctr));

    read_number(j, "Rounding", ctr.rounding);
}

static void from_json(const json& j, ColorToggleThickness& ctt)
{
    from_json(j, static_cast<ColorToggle&>(ctt));

    read_number(j, "Thickness", ctt.thickness);
}

static void from_json(const json& j, ColorToggleThicknessRounding& cttr)
{
    from_json(j, static_cast<ColorToggleRounding&>(cttr));

    read_number(j, "Thickness", cttr.thickness);
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

    read_number(j, "Type", s.type);
}

static void from_json(const json& j, Box& b)
{
    from_json(j, static_cast<ColorToggleThicknessRounding&>(b));

    read_number(j, "Type", b.type);
    read<value_t::array>(j, "Scale", b.scale);
}

static void from_json(const json& j, Shared& s)
{
    read<value_t::boolean>(j, "Enabled", s.enabled);
    read<value_t::object>(j, "Font", s.font);
    read<value_t::object>(j, "Snapline", s.snapline);
    read<value_t::object>(j, "Box", s.box);
    read<value_t::object>(j, "Name", s.name);
    read_number(j, "Text Cull Distance", s.textCullDistance);
}

static void from_json(const json& j, Weapon& w)
{
    from_json(j, static_cast<Shared&>(w));

    read<value_t::object>(j, "Ammo", w.ammo);
}

static void from_json(const json& j, Trail& t)
{
    from_json(j, static_cast<ColorToggleThickness&>(t));

    read_number(j, "Type", t.type);
    read_number(j, "Time", t.time);
}

static void from_json(const json& j, Trails& t)
{
    read<value_t::boolean>(j, "Enabled", t.enabled);
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
    read<value_t::boolean>(j, "Audible Only", p.audibleOnly);
    read<value_t::boolean>(j, "Spotted Only", p.spottedOnly);
    read<value_t::object>(j, "Skeleton", p.skeleton);
}

static void from_json(const json& j, ImVec2& v)
{
    read_number(j, "X", v.x);
    read_number(j, "Y", v.y);
}

static void from_json(const json& j, Config::Aimbot& a)
{
    read<value_t::boolean>(j, "Enabled", a.enabled);
    read<value_t::boolean>(j, "On key", a.onKey);
    read_number(j, "Key", a.key);
    read_number(j, "Key mode", a.keyMode);
    read<value_t::boolean>(j, "Aimlock", a.aimlock);
    read<value_t::boolean>(j, "Silent", a.silent);
    read<value_t::boolean>(j, "Friendly fire", a.friendlyFire);
    read<value_t::boolean>(j, "Visible only", a.visibleOnly);
    read<value_t::boolean>(j, "Scoped only", a.scopedOnly);
    read<value_t::boolean>(j, "Ignore flash", a.ignoreFlash);
    read<value_t::boolean>(j, "Ignore smoke", a.ignoreSmoke);
    read<value_t::boolean>(j, "Auto shot", a.autoShot);
    read<value_t::boolean>(j, "Auto scope", a.autoScope);
    read_number(j, "Fov", a.fov);
    read_number(j, "Smooth", a.smooth);
    read_number(j, "Bone", a.bone);
    read_number(j, "Max aim inaccuracy", a.maxAimInaccuracy);
    read_number(j, "Max shot inaccuracy", a.maxShotInaccuracy);
    read_number(j, "Min damage", a.minDamage);
    read<value_t::boolean>(j, "Killshot", a.killshot);
    read<value_t::boolean>(j, "Between shots", a.betweenShots);
}

static void from_json(const json& j, Config::Triggerbot& t)
{
    read<value_t::boolean>(j, "Enabled", t.enabled);
    read<value_t::boolean>(j, "On key", t.onKey);
    read_number(j, "Key", t.key);
    read<value_t::boolean>(j, "Friendly fire", t.friendlyFire);
    read<value_t::boolean>(j, "Scoped only", t.scopedOnly);
    read<value_t::boolean>(j, "Ignore flash", t.ignoreFlash);
    read<value_t::boolean>(j, "Ignore smoke", t.ignoreSmoke);
    read_number(j, "Hitgroup", t.hitgroup);
    read_number(j, "Shot delay", t.shotDelay);
    read_number(j, "Min damage", t.minDamage);
    read<value_t::boolean>(j, "Killshot", t.killshot);
    read_number(j, "Burst Time", t.burstTime);
}

static void from_json(const json& j, Config::Backtrack& b)
{
    read<value_t::boolean>(j, "Enabled", b.enabled);
    read<value_t::boolean>(j, "Ignore smoke", b.ignoreSmoke);
    read<value_t::boolean>(j, "Recoil based fov", b.recoilBasedFov);
    read_number(j, "Time limit", b.timeLimit);
}

static void from_json(const json& j, Config::AntiAim& a)
{
    read<value_t::boolean>(j, "Enabled", a.enabled);
    read<value_t::boolean>(j, "Pitch", a.pitch);
    read<value_t::boolean>(j, "Yaw", a.yaw);
    read_number(j, "Pitch angle", a.pitchAngle);
}

static void from_json(const json& j, Config::Glow& g)
{
    from_json(j, static_cast<ColorA&>(g));

    read<value_t::boolean>(j, "Enabled", g.enabled);
    read<value_t::boolean>(j, "Health based", g.enabled);
    read_number(j, "Style", g.style);
}

static void from_json(const json& j, Config::Chams::Material& m)
{
    from_json(j, static_cast<ColorA&>(m));

    read<value_t::boolean>(j, "Enabled", m.enabled);
    read<value_t::boolean>(j, "Health based", m.healthBased);
    read<value_t::boolean>(j, "Blinking", m.healthBased);
    read<value_t::boolean>(j, "Wireframe", m.wireframe);
    read<value_t::boolean>(j, "Cover", m.cover);
    read<value_t::boolean>(j, "Ignore-Z", m.ignorez);
    read_number(j, "Material", m.material);
}

static void from_json(const json& j, Config::Chams& c)
{
    read_vector(j, "Materials", c.materials);
}

static void from_json(const json& j, Config::StreamProofESP& e)
{
    read_map(j, "Allies", e.allies);
    read_map(j, "Enemies", e.enemies);
    read_map(j, "Weapons", e.weapons);
    read_map(j, "Projectiles", e.projectiles);
    read_map(j, "Loot Crates", e.lootCrates);
    read_map(j, "Other Entities", e.otherEntities);
}

static void from_json(const json& j, Config::Visuals::ColorCorrection& c)
{
    read<value_t::boolean>(j, "Enabled", c.enabled);
    read_number(j, "Blue", c.blue);
    read_number(j, "Red", c.red);
    read_number(j, "Mono", c.mono);
    read_number(j, "Saturation", c.saturation);
    read_number(j, "Ghost", c.ghost);
    read_number(j, "Green", c.green);
    read_number(j, "Yellow", c.yellow);
}

static void from_json(const json& j, Config::Visuals& v)
{
    read<value_t::boolean>(j, "Disable post-processing", v.disablePostProcessing);
    read<value_t::boolean>(j, "Inverse ragdoll gravity", v.inverseRagdollGravity);
    read<value_t::boolean>(j, "No fog", v.noFog);
    read<value_t::boolean>(j, "No 3d sky", v.no3dSky);
    read<value_t::boolean>(j, "No aim punch", v.noAimPunch);
    read<value_t::boolean>(j, "No view punch", v.noViewPunch);
    read<value_t::boolean>(j, "No hands", v.noHands);
    read<value_t::boolean>(j, "No sleeves", v.noSleeves);
    read<value_t::boolean>(j, "No weapons", v.noWeapons);
    read<value_t::boolean>(j, "No smoke", v.noSmoke);
    read<value_t::boolean>(j, "No blur", v.noBlur);
    read<value_t::boolean>(j, "No scope overlay", v.noScopeOverlay);
    read<value_t::boolean>(j, "No grass", v.noGrass);
    read<value_t::boolean>(j, "No shadows", v.noShadows);
    read<value_t::boolean>(j, "Wireframe smoke", v.wireframeSmoke);
    read<value_t::boolean>(j, "Zoom", v.noScopeOverlay);
    read_number(j, "Zoom key", v.zoomKey);
    read<value_t::boolean>(j, "Thirdperson", v.thirdperson);
    read_number(j, "Thirdperson key", v.thirdpersonKey);
    read_number(j, "Thirdperson distance", v.thirdpersonDistance);
    read_number(j, "Viewmodel FOV", v.viewmodelFov);
    read_number(j, "FOV", v.fov);
    read_number(j, "Far Z", v.farZ);
    read_number(j, "Flash reduction", v.flashReduction);
    read_number(j, "Brightness", v.brightness);
    read_number(j, "Skybox", v.skybox);

  //  read<value_t::object>(j, "World", v.world);
  //  read<value_t::object>(j, "Sky", v.sky);

    read<value_t::boolean>(j, "Deagle spinner", v.deagleSpinner);
    read_number(j, "Screen effect", v.screenEffect);
    read_number(j, "Hit effect", v.hitEffect);
    read_number(j, "Hit effect time", v.hitEffectTime);
    read_number(j, "Hit marker", v.hitMarker);
    read_number(j, "Hit marker time", v.hitMarkerTime);
    read_number(j, "Playermodel T", v.playerModelT);
    read_number(j, "Playermodel CT", v.playerModelCT);
    read<value_t::object>(j, "Color correction", v.colorCorrection);
}

static void from_json(const json& j, sticker_setting& s)
{
    read_number(j, "Kit", s.kit);
    read_number(j, "Kit vector index", s.kit_vector_index);
    read_number(j, "Wear", s.wear);
    read_number(j, "Scale", s.scale);
    read_number(j, "Rotation", s.rotation);
}

static void from_json(const json& j, item_setting& i)
{
    read<value_t::boolean>(j, "Enabled", i.enabled);
    read_number(j, "Definition index", i.itemId);
    read_number(j, "Definition vector index", i.itemIdIndex);
    read_number(j, "Quality", i.quality);
    read_number(j, "Quality vector index", i.entity_quality_vector_index);

    read_number(j, "Paint Kit", i.paintKit);
    read_number(j, "Paint Kit vector index", i.paint_kit_vector_index);

    read_number(j, "Definition override", i.definition_override_index);
    read_number(j, "Definition override vector index", i.definition_override_vector_index);

    read_number(j, "Seed", i.seed);
    read_number(j, "StatTrak", i.stat_trak);
    read_number(j, "Wear", i.wear);

    // if (skinChangerJson.isMember("custom_name")) strcpy_s(skinChangerConfig.custom_name, sizeof(skinChangerConfig.custom_name), skinChangerJson["custom_name"].asCString());

    read<value_t::array>(j, "Stickers", i.stickers);
}

static void from_json(const json& j, Config::Sound::Player& p)
{
    read_number(j, "Master volume", p.masterVolume);
    read_number(j, "Headshot volume", p.headshotVolume);
    read_number(j, "Weapon volume", p.weaponVolume);
    read_number(j, "Footstep volume", p.footstepVolume);
}

static void from_json(const json& j, Config::Sound& s)
{
    read_number(j, "Chicken volume", s.chickenVolume);
    read<value_t::array>(j, "Players", s.players);
}

static void from_json(const json& j, Config::Style& s)
{
    read_number(j, "Menu style", s.menuStyle);
    read_number(j, "Menu colors", s.menuColors);

    if (j.contains("Colors") && j["Colors"].is_object()) {
        const auto& colors = j["Colors"];

        ImGuiStyle& style = ImGui::GetStyle();

        for (int i = 0; i < ImGuiCol_COUNT; i++) {
            if (const char* name = ImGui::GetStyleColorName(i); colors.contains(name)) {
                std::array<float, 4> temp;
                read<value_t::array>(colors, name, temp);
                style.Colors[i].x = temp[0];
                style.Colors[i].y = temp[1];
                style.Colors[i].z = temp[2];
                style.Colors[i].w = temp[3];
            }
        }
    }
}

static void from_json(const json& j, PurchaseList& pl)
{
    read<value_t::boolean>(j, "Enabled", pl.enabled);
    read<value_t::boolean>(j, "Only During Freeze Time", pl.onlyDuringFreezeTime);
    read<value_t::boolean>(j, "Show Prices", pl.showPrices);
    read<value_t::boolean>(j, "No Title Bar", pl.noTitleBar);
    read_number(j, "Mode", pl.mode);
}

static void from_json(const json& j, Config::Misc& m)
{
    read_number(j, "Menu key", m.menuKey);
    read<value_t::boolean>(j, "Anti AFK kick", m.antiAfkKick);
    read<value_t::boolean>(j, "Auto strafe", m.autoStrafe);
    read<value_t::boolean>(j, "Bunny hop", m.bunnyHop);
    read<value_t::boolean>(j, "Custom clan tag", m.customClanTag);
    read<value_t::boolean>(j, "Clock tag", m.clocktag);
    if (j.contains("Clan tag"))
        strncpy_s(m.clanTag, j["Clan tag"].get<std::string>().c_str(), _TRUNCATE);
    read<value_t::boolean>(j, "Animated clan tag", m.animatedClanTag);
    read<value_t::boolean>(j, "Fast duck", m.fastDuck);
    read<value_t::boolean>(j, "Moonwalk", m.moonwalk);
    read<value_t::boolean>(j, "Edge Jump", m.edgejump);
    read_number(j, "Edge Jump Key", m.edgejumpkey);
    read<value_t::boolean>(j, "Slowwalk", m.slowwalk);
    read_number(j, "Slowwalk key", m.slowwalkKey);
    read<value_t::boolean>(j, "Sniper crosshair", m.sniperCrosshair);
    read<value_t::boolean>(j, "Recoil crosshair", m.recoilCrosshair);
    read<value_t::boolean>(j, "Auto pistol", m.autoPistol);
    read<value_t::boolean>(j, "Auto reload", m.autoReload);
    read<value_t::boolean>(j, "Auto accept", m.autoAccept);
    read<value_t::boolean>(j, "Radar hack", m.radarHack);
    read<value_t::boolean>(j, "Reveal ranks", m.revealRanks);
    read<value_t::boolean>(j, "Reveal money", m.revealMoney);
    read<value_t::boolean>(j, "Reveal suspect", m.revealSuspect);
    //  read<value_t::object>(j, "Spectator list", m.spectatorList);
    //  read<value_t::object>(j, "Watermark", m.watermark);
    read<value_t::boolean>(j, "Fix animation LOD", m.fixAnimationLOD);
    read<value_t::boolean>(j, "Fix bone matrix", m.fixBoneMatrix);
    read<value_t::boolean>(j, "Fix movement", m.fixMovement);
    read<value_t::boolean>(j, "Disable model occlusion", m.disableModelOcclusion);
    read_number(j, "Aspect Ratio", m.aspectratio);
    read<value_t::boolean>(j, "Kill message", m.killMessage);
    read<value_t::object>(j, "Kill message string", m.killMessageString);

    read<value_t::boolean>(j, "Name stealer", m.nameStealer);
    read<value_t::boolean>(j, "Disable HUD blur", m.disablePanoramablur);
    read_number(j, "Ban color", m.banColor);
    read<value_t::object>(j, "Ban text", m.banText);
    read<value_t::boolean>(j, "Fast plant", m.fastPlant);
    // read<value_t::object>(j, "Bomb timer", m.bombTimer);

    read<value_t::boolean>(j, "Quick reload", m.quickReload);
    read<value_t::boolean>(j, "Prepare revolver", m.prepareRevolver);
    read_number(j, "Prepare revolver key", m.prepareRevolverKey);
    read_number(j, "Hit sound", m.hitSound);
    read_number(j, "Choked packets", m.chokedPackets);
    read_number(j, "Choked packets key", m.chokedPacketsKey);
    read_number(j, "Quick healthshot key", m.quickHealthshotKey);
    read<value_t::boolean>(j, "Grenade predict", m.nadePredict);
    read<value_t::boolean>(j, "Fix tablet signal", m.fixTabletSignal);
    read_number(j, "Max angle delta", m.maxAngleDelta);
    read<value_t::boolean>(j, "Fake prime", m.fakePrime);
    read<value_t::boolean>(j, "Fix tablet signal", m.fixTabletSignal);
    read<value_t::object>(j, "Custom Hit Sound", m.customHitSound);
    read_number(j, "Kill sound", m.killSound);
    read<value_t::object>(j, "Custom Kill Sound", m.customKillSound);
    read<value_t::object>(j, "Purchase List", m.purchaseList);
}

static void from_json(const json& j, Config::Reportbot& r)
{
    read<value_t::boolean>(j, "Enabled", r.enabled);
    read_number(j, "Target", r.target);
    read_number(j, "Delay", r.delay);
    read_number(j, "Rounds", r.rounds);
    read<value_t::boolean>(j, "Abusive Communications", r.textAbuse);
    read<value_t::boolean>(j, "Griefing", r.griefing);
    read<value_t::boolean>(j, "Wall Hacking", r.wallhack);
    read<value_t::boolean>(j, "Aim Hacking", r.aimbot);
    read<value_t::boolean>(j, "Other Hacking", r.other);
}

void Config::load(size_t id) noexcept
{
    json j;

    if (std::ifstream in{ path / (const char8_t*)configs[id].c_str() }; in.good())
        in >> j;
    else
        return;

    read<value_t::array>(j, "Aimbot", aimbot);
    read<value_t::array>(j, "Triggerbot", triggerbot);
    read<value_t::object>(j, "Backtrack", backtrack);
    read<value_t::object>(j, "Anti aim", antiAim);
    read<value_t::array>(j, "Glow", glow);
    read<value_t::array>(j, "Chams", chams);
    read<value_t::object>(j, "ESP", streamProofESP);
    read<value_t::object>(j, "Visuals", visuals);
    read<value_t::array>(j, "Skin changer", skinChanger);
    read<value_t::object>(j, "Sound", sound);
    read<value_t::object>(j, "Style", style);
    read<value_t::object>(j, "Misc", misc);
    read<value_t::object>(j, "Reportbot", reportbot);
}

// WRITE macro requires:
// - json object named 'j'
// - object holding default values named 'dummy'
// - object to write to json named 'o'
#define WRITE(name, valueName) \
if (!(o.valueName == dummy.valueName)) \
    j[name] = o.valueName;

// WRITE_BASE macro requires:
// - json object named 'j'
// - object to write to json named 'o'
#define WRITE_BASE(structName) \
if (!(static_cast<const structName&>(o) == static_cast<const structName&>(dummy))) \
    j = static_cast<const structName&>(o);


static void to_json(json& j, const ColorA& o)
{
    const ColorA dummy;

    WRITE("Color", color)
    WRITE("Rainbow", rainbow)
    WRITE("Rainbow Speed", rainbowSpeed)
}

static void to_json(json& j, const ColorToggle& o)
{
    const ColorToggle dummy;

    WRITE_BASE(ColorA)
    WRITE("Enabled", enabled)
}

static void to_json(json& j, const ColorToggleRounding& o)
{
    const ColorToggleRounding dummy;

    WRITE_BASE(ColorToggle)
    WRITE("Rounding", rounding)
}

static void to_json(json& j, const ColorToggleThickness& o)
{
    const ColorToggleThickness dummy;

    WRITE_BASE(ColorToggle)
    WRITE("Thickness", thickness)
}

static void to_json(json& j, const ColorToggleThicknessRounding& o)
{
    const ColorToggleThicknessRounding dummy;

    WRITE_BASE(ColorToggleRounding)
    WRITE("Thickness", thickness)
}

static void to_json(json& j, const Font& o)
{
    const Font dummy;

    WRITE("Name", name)
}

static void to_json(json& j, const Snapline& o)
{
    const Snapline dummy;

    WRITE_BASE(ColorToggleThickness)
    WRITE("Type", type)
}

static void to_json(json& j, const Box& o)
{
    const Box dummy;

    WRITE_BASE(ColorToggleThicknessRounding)
    WRITE("Type", type)
    WRITE("Scale", scale)
}

static void to_json(json& j, const Shared& o)
{
    const Shared dummy;

    WRITE("Enabled", enabled)
    WRITE("Font", font)
    WRITE("Snapline", snapline)
    WRITE("Box", box)
    WRITE("Name", name)
    WRITE("Text Cull Distance", textCullDistance)
}

static void to_json(json& j, const Player& o)
{
    const Player dummy;

    WRITE_BASE(Shared)
    WRITE("Weapon", weapon)
    WRITE("Flash Duration", flashDuration)
    WRITE("Audible Only", audibleOnly)
    WRITE("Spotted Only", spottedOnly)
    WRITE("Skeleton", skeleton)
}

static void to_json(json& j, const Weapon& o)
{
    j = static_cast<Shared>(o);

    const Weapon dummy;

    WRITE("Ammo", ammo)
}

static void to_json(json& j, const Trail& o)
{
    j = static_cast<ColorToggleThickness>(o);

    const Trail dummy;

    WRITE("Type", type)
    WRITE("Time", time)
}

static void to_json(json& j, const Trails& o)
{
    const Trails dummy;

    WRITE("Enabled", enabled)
    WRITE("Local Player", localPlayer)
    WRITE("Allies", allies)
    WRITE("Enemies", enemies)
}

static void to_json(json& j, const Projectile& o)
{
    j = static_cast<Shared>(o);

    const Projectile dummy;

    WRITE("Trails", trails)
}

static void to_json(json& j, const ImVec2& o)
{
    const ImVec2 dummy;

    WRITE("X", x)
    WRITE("Y", y)
}

static void to_json(json& j, const Config::Aimbot& o)
{
    const Config::Aimbot dummy;

    WRITE("Enabled", enabled)
    WRITE("On key", onKey)
    WRITE("Key", key)
    WRITE("Key mode", keyMode)
    WRITE("Aimlock", aimlock)
    WRITE("Silent", silent)
    WRITE("Friendly fire", friendlyFire)
    WRITE("Visible only", visibleOnly)
    WRITE("Scoped only", scopedOnly)
    WRITE("Ignore flash", ignoreFlash)
    WRITE("Ignore smoke", ignoreSmoke)
    WRITE("Auto shot", autoShot)
    WRITE("Auto scope", autoScope)
    WRITE("Fov", fov)
    WRITE("Smooth", smooth)
    WRITE("Bone", bone)
    WRITE("Max aim inaccuracy", maxAimInaccuracy)
    WRITE("Max shot inaccuracy", maxShotInaccuracy)
    WRITE("Min damage", minDamage)
    WRITE("Killshot", killshot)
    WRITE("Between shots", betweenShots)
}

static void to_json(json& j, const Config::Triggerbot& o)
{
    const Config::Triggerbot dummy;

    WRITE("Enabled", enabled)
    WRITE("On key", onKey)
    WRITE("Key", key)
    WRITE("Friendly fire", friendlyFire)
    WRITE("Scoped only", scopedOnly)
    WRITE("Ignore flash", ignoreFlash)
    WRITE("Ignore smoke", ignoreSmoke)
    WRITE("Hitgroup", hitgroup)
    WRITE("Shot delay", shotDelay)
    WRITE("Min damage", minDamage)
    WRITE("Killshot", killshot)
    WRITE("Burst Time", burstTime)
}

static void to_json(json& j, const Config::Backtrack& o)
{
    const Config::Backtrack dummy;

    WRITE("Enabled", enabled)
    WRITE("Ignore smoke", ignoreSmoke)
    WRITE("Recoil based fov", recoilBasedFov)
    WRITE("Time limit", timeLimit)
}

static void to_json(json& j, const Config::AntiAim& o)
{
    const Config::AntiAim dummy;

    WRITE("Enabled", enabled)
    WRITE("Pitch", pitch)
    WRITE("Pitch angle", pitchAngle)
    WRITE("Yaw", yaw)
}

static void to_json(json& j, const Config::Glow& o)
{
    const Config::Glow dummy;

    WRITE_BASE(ColorA)
    WRITE("Enabled", enabled)
    WRITE("Health based", healthBased)
    WRITE("Style", style)
}

static void to_json(json& j, const Config::Chams::Material& o)
{
    const Config::Chams::Material dummy;

    WRITE_BASE(ColorA)
    WRITE("Enabled", enabled)
    WRITE("Health based", healthBased)
    WRITE("Blinking", blinking)
    WRITE("Wireframe", wireframe)
    WRITE("Cover", cover)
    WRITE("Ignore-Z", ignorez)
    WRITE("Material", material)
}

static void to_json(json& j, const Config::Chams& o)
{
    const Config::Chams dummy;

    j = o.materials;
}

template <typename T>
static void save_map(json& j, const char* name, const std::unordered_map<std::string, T>& map)
{
    const T dummy;

    for (const auto& [key, value] : map) {
        if (!(value == dummy))
            j[name][key] = value;
    }
}

static void to_json(json& j, const Config::StreamProofESP& o)
{
    save_map(j, "Allies", o.allies);
    save_map(j, "Enemies", o.enemies);
    save_map(j, "Weapons", o.weapons);
    save_map(j, "Projectiles", o.projectiles);
    save_map(j, "Loot Crates", o.lootCrates);
    save_map(j, "Other Entities", o.otherEntities);
}

void Config::save(size_t id) const noexcept
{
    ::json j;
    j["Aimbot"] = aimbot;
    j["Triggerbot"] = triggerbot;
    j["Backtrack"] = backtrack;
    j["Anti aim"] = antiAim;
    j["Glow"] = glow;
    j["Chams"] = chams;
    j["ESP"] = streamProofESP;

    Json::Value json;

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
        miscJson["Custom Hit Sound"] = misc.customHitSound;
        miscJson["Kill sound"] = misc.killSound;
        miscJson["Custom Kill Sound"] = misc.customKillSound;

        {
            auto& purchaseListJson = miscJson["Purchase List"];
            purchaseListJson["Enabled"] = misc.purchaseList.enabled;
            purchaseListJson["Only During Freeze Time"] = misc.purchaseList.onlyDuringFreezeTime;
            purchaseListJson["Show Prices"] = misc.purchaseList.showPrices;
            purchaseListJson["No Title Bar"] = misc.purchaseList.noTitleBar;
            purchaseListJson["Mode"] = misc.purchaseList.mode;
        }
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

    std::error_code ec;
    std::filesystem::create_directory(path, ec);

    if (std::ofstream out{ path / (const char8_t*)configs[id].c_str() }; out.good())
        out << json;
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
    esp = { };
    visuals = { };
    skinChanger = { };
    sound = { };
    style = { };
    misc = { };
    reportbot = { };
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

        static constexpr ImWchar ranges[]{ 0x0020, 0xFFFF, 0 };
        ImFontConfig cfg;
        cfg.FontDataOwnedByAtlas = false;

        if (fonts.find(fontName) == fonts.cend()) {
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
