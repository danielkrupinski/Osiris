#include <fstream>

#ifdef _WIN32
#include <ShlObj.h>
#endif

#include "nlohmann/json.hpp"

#include "Config.h"
#include "Helpers.h"
#include "SDK/Platform.h"

#ifdef _WIN32
int CALLBACK fontCallback(const LOGFONTW* lpelfe, const TEXTMETRICW*, DWORD, LPARAM lParam)
{
    const wchar_t* const fontName = reinterpret_cast<const ENUMLOGFONTEXW*>(lpelfe)->elfFullName;

    if (fontName[0] == L'@')
        return TRUE;

    if (HFONT font = CreateFontW(0, 0, 0, 0,
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

        if (fontData == GDI_ERROR) {
            if (char buff[1024]; WideCharToMultiByte(CP_UTF8, 0, fontName, -1, buff, sizeof(buff), nullptr, nullptr))
                reinterpret_cast<std::vector<std::string>*>(lParam)->emplace_back(buff);
        }
    }
    return TRUE;
}
#endif

Config::Config(const char* name) noexcept
{
#ifdef _WIN32
    if (PWSTR pathToDocuments; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &pathToDocuments))) {
        path = pathToDocuments;
        CoTaskMemFree(pathToDocuments);
    }
#else
    if (const char* homeDir = getenv("HOME"))
        path = homeDir;
#endif

    path /= name;
    listConfigs();
    misc.clanTag[0] = '\0';

#ifdef _WIN32
    LOGFONTW logfont;
    logfont.lfCharSet = ANSI_CHARSET;
    logfont.lfPitchAndFamily = DEFAULT_PITCH;
    logfont.lfFaceName[0] = L'\0';

    EnumFontFamiliesExW(GetDC(nullptr), &logfont, fontCallback, (LPARAM)&systemFonts, 0);
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

static void read(const json& j, const char* key, KeyBind& o) noexcept
{
    if (!j.contains(key))
        return;

    if (const auto& val = j[key]; val.is_string())
        o = val.get<std::string>().c_str();
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

    if (const auto it = std::find_if(config->getSystemFonts().begin(), config->getSystemFonts().end(), [&f](const auto& e) { return e == f.name; }); it != config->getSystemFonts().end())
        f.index = std::distance(config->getSystemFonts().begin(), it);
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

static void from_json(const json& j, OffscreenEnemies& o)
{
    read(j, "Enabled", o.enabled);
    read<value_t::object>(j, "Color", o.color);
}

static void from_json(const json& j, ImVec2& v)
{
    read(j, "X", v.x);
    read(j, "Y", v.y);
}

static void from_json(const json& j, Config::Aimbot& a)
{
    read(j, "Enabled", a.enabled);
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
    read(j, "Between shots", a.betweenShots);
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
}

static void from_json(const json& j, Config::Backtrack& b)
{
    read(j, "Enabled", b.enabled);
    read(j, "Ignore smoke", b.ignoreSmoke);
    read(j, "Recoil based fov", b.recoilBasedFov);
    read(j, "Time limit", b.timeLimit);
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
}

static void from_json(const json& j, Config::Visuals& v)
{
    read(j, "Disable post-processing", v.disablePostProcessing);
    read(j, "Inverse ragdoll gravity", v.inverseRagdollGravity);
    read(j, "No fog", v.noFog);
    read(j, "No 3d sky", v.no3dSky);
    read(j, "No aim punch", v.noAimPunch);
    read(j, "No view punch", v.noViewPunch);
    read(j, "No hands", v.noHands);
    read(j, "No sleeves", v.noSleeves);
    read(j, "No weapons", v.noWeapons);
    read(j, "No smoke", v.noSmoke);
    read(j, "No blur", v.noBlur);
    read(j, "No scope overlay", v.noScopeOverlay);
    read(j, "No grass", v.noGrass);
    read(j, "No shadows", v.noShadows);
    read(j, "Wireframe smoke", v.wireframeSmoke);
    read(j, "Zoom", v.zoom);
    read(j, "Zoom key", v.zoomKey);
    read(j, "Thirdperson", v.thirdperson);
    read(j, "Thirdperson key", v.thirdpersonKey);
    read(j, "Thirdperson distance", v.thirdpersonDistance);
    read(j, "Viewmodel FOV", v.viewmodelFov);
    read(j, "FOV", v.fov);
    read(j, "Far Z", v.farZ);
    read(j, "Flash reduction", v.flashReduction);
    read(j, "Brightness", v.brightness);
    read(j, "Skybox", v.skybox);
    read<value_t::object>(j, "World", v.world);
    read<value_t::object>(j, "Sky", v.sky);
    read(j, "Deagle spinner", v.deagleSpinner);
    read(j, "Screen effect", v.screenEffect);
    read(j, "Hit effect", v.hitEffect);
    read(j, "Hit effect time", v.hitEffectTime);
    read(j, "Hit marker", v.hitMarker);
    read(j, "Hit marker time", v.hitMarkerTime);
    read(j, "Playermodel T", v.playerModelT);
    read(j, "Playermodel CT", v.playerModelCT);
    read<value_t::object>(j, "Color correction", v.colorCorrection);
}

static void from_json(const json& j, sticker_setting& s)
{
    read(j, "Kit", s.kit);
    read(j, "Wear", s.wear);
    read(j, "Scale", s.scale);
    read(j, "Rotation", s.rotation);

    s.onLoad();
}

static void from_json(const json& j, item_setting& i)
{
    read(j, "Enabled", i.enabled);
    read(j, "Definition index", i.itemId);
    read(j, "Quality", i.quality);
    read(j, "Paint Kit", i.paintKit);
    read(j, "Definition override", i.definition_override_index);
    read(j, "Seed", i.seed);
    read(j, "StatTrak", i.stat_trak);
    read(j, "Wear", i.wear);

#ifdef _WIN32
    if (j.contains("Custom name"))
        strncpy_s(i.custom_name, j["Custom name"].get<std::string>().c_str(), _TRUNCATE);
#endif

    read(j, "Stickers", i.stickers);

    i.onLoad();
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
    read(j, "Bunny hop", m.bunnyHop);
    read(j, "Custom clan tag", m.customClanTag);
    read(j, "Clock tag", m.clocktag);
#ifdef _WIN32
    if (j.contains("Clan tag"))
        strncpy_s(m.clanTag, j["Clan tag"].get<std::string>().c_str(), _TRUNCATE);
#endif
    read(j, "Animated clan tag", m.animatedClanTag);
    read(j, "Fast duck", m.fastDuck);
    read(j, "Moonwalk", m.moonwalk);
    read(j, "Edge Jump", m.edgejump);
    read(j, "Edge Jump Key", m.edgejumpkey);
    read(j, "Slowwalk", m.slowwalk);
    read(j, "Slowwalk key", m.slowwalkKey);
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
    read<value_t::object>(j, "Offscreen Enemies", m.offscreenEnemies);
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
    read(j, "Fast plant", m.fastPlant);
    read(j, "Fast Stop", m.fastStop);
    read<value_t::object>(j, "Bomb timer", m.bombTimer);
    read(j, "Quick reload", m.quickReload);
    read(j, "Prepare revolver", m.prepareRevolver);
    read(j, "Prepare revolver key", m.prepareRevolverKey);
    read(j, "Hit sound", m.hitSound);
    read(j, "Choked packets", m.chokedPackets);
    read(j, "Choked packets key", m.chokedPacketsKey);
    read(j, "Quick healthshot key", m.quickHealthshotKey);
    read(j, "Grenade predict", m.nadePredict);
    read(j, "Fix tablet signal", m.fixTabletSignal);
    read(j, "Max angle delta", m.maxAngleDelta);
    read(j, "Fake prime", m.fakePrime);
    read(j, "Fix tablet signal", m.fixTabletSignal);
    read<value_t::object>(j, "Custom Hit Sound", m.customHitSound);
    read(j, "Kill sound", m.killSound);
    read<value_t::object>(j, "Custom Kill Sound", m.customKillSound);
    read<value_t::object>(j, "Purchase List", m.purchaseList);
    read<value_t::object>(j, "Reportbot", m.reportbot);
    read(j, "Opposite Hand Knife", m.oppositeHandKnife);
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
    read(j, "Aimbot On key", aimbotOnKey);
    read(j, "Aimbot Key", aimbotKey);
    read(j, "Aimbot Key mode", aimbotKeyMode);

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

static void to_json(json& j, const OffscreenEnemies& o, const OffscreenEnemies& dummy = {})
{
    WRITE("Enabled", enabled);
    WRITE("Color", color);
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
    WRITE("Between shots", betweenShots);
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
}

static void to_json(json& j, const Config::Backtrack& o, const Config::Backtrack& dummy = {})
{
    WRITE("Enabled", enabled);
    WRITE("Ignore smoke", ignoreSmoke);
    WRITE("Recoil based fov", recoilBasedFov);
    WRITE("Time limit", timeLimit);
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

static void to_json(json& j, const KeyBind& o, const KeyBind& dummy)
{
    if (o != dummy)
        j = o.toString();
}

static void to_json(json& j, const Config::Misc& o)
{
    const Config::Misc dummy;

    WRITE("Menu key", menuKey);
    WRITE("Anti AFK kick", antiAfkKick);
    WRITE("Auto strafe", autoStrafe);
    WRITE("Bunny hop", bunnyHop);
    WRITE("Custom clan tag", customClanTag);
    WRITE("Clock tag", clocktag);

    if (o.clanTag[0])
        j["Clan tag"] = o.clanTag;

    WRITE("Animated clan tag", animatedClanTag);
    WRITE("Fast duck", fastDuck);
    WRITE("Moonwalk", moonwalk);
    WRITE("Edge Jump", edgejump);
    WRITE("Edge Jump Key", edgejumpkey);
    WRITE("Slowwalk", slowwalk);
    WRITE("Slowwalk key", slowwalkKey);
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
    WRITE("Offscreen Enemies", offscreenEnemies);
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
    WRITE("Fast plant", fastPlant);
    WRITE("Fast Stop", fastStop);
    WRITE("Bomb timer", bombTimer);
    WRITE("Quick reload", quickReload);
    WRITE("Prepare revolver", prepareRevolver);
    WRITE("Prepare revolver key", prepareRevolverKey);
    WRITE("Hit sound", hitSound);
    WRITE("Choked packets", chokedPackets);
    WRITE("Choked packets key", chokedPacketsKey);
    WRITE("Quick healthshot key", quickHealthshotKey);
    WRITE("Grenade predict", nadePredict);
    WRITE("Fix tablet signal", fixTabletSignal);
    WRITE("Max angle delta", maxAngleDelta);
    WRITE("Fake prime", fakePrime);
    WRITE("Fix tablet signal", fixTabletSignal);
    WRITE("Custom Hit Sound", customHitSound);
    WRITE("Kill sound", killSound);
    WRITE("Custom Kill Sound", customKillSound);
    WRITE("Purchase List", purchaseList);
    WRITE("Reportbot", reportbot);
    WRITE("Opposite Hand Knife", oppositeHandKnife);
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
    WRITE("No fog", noFog);
    WRITE("No 3d sky", no3dSky);
    WRITE("No aim punch", noAimPunch);
    WRITE("No view punch", noViewPunch);
    WRITE("No hands", noHands);
    WRITE("No sleeves", noSleeves);
    WRITE("No weapons", noWeapons);
    WRITE("No smoke", noSmoke);
    WRITE("No blur", noBlur);
    WRITE("No scope overlay", noScopeOverlay);
    WRITE("No grass", noGrass);
    WRITE("No shadows", noShadows);
    WRITE("Wireframe smoke", wireframeSmoke);
    WRITE("Zoom", zoom);
    WRITE("Zoom key", zoomKey);
    WRITE("Thirdperson", thirdperson);
    WRITE("Thirdperson key", thirdpersonKey);
    WRITE("Thirdperson distance", thirdpersonDistance);
    WRITE("Viewmodel FOV", viewmodelFov);
    WRITE("FOV", fov);
    WRITE("Far Z", farZ);
    WRITE("Flash reduction", flashReduction);
    WRITE("Brightness", brightness);
    WRITE("Skybox", skybox);
    WRITE("World", world);
    WRITE("Sky", sky);
    WRITE("Deagle spinner", deagleSpinner);
    WRITE("Screen effect", screenEffect);
    WRITE("Hit effect", hitEffect);
    WRITE("Hit effect time", hitEffectTime);
    WRITE("Hit marker", hitMarker);
    WRITE("Hit marker time", hitMarkerTime);
    WRITE("Playermodel T", playerModelT);
    WRITE("Playermodel CT", playerModelCT);
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
}

static void to_json(json& j, const sticker_setting& o)
{
    const sticker_setting dummy;

    WRITE("Kit", kit);
    WRITE("Wear", wear);
    WRITE("Scale", scale);
    WRITE("Rotation", rotation);
}

static void to_json(json& j, const item_setting& o)
{
    const item_setting dummy;

    WRITE("Enabled", enabled);
    WRITE("Definition index", itemId);
    WRITE("Quality", quality);
    WRITE("Paint Kit", paintKit);
    WRITE("Definition override", definition_override_index);
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
    createConfigDir();

    if (std::ofstream out{ path / (const char8_t*)configs[id].c_str() }; out.good()) {
        json j;

        j["Aimbot"] = aimbot;
        j["Aimbot On key"] = aimbotOnKey;
        j["Aimbot Key"] = aimbotKey;
        j["Aimbot Key mode"] = aimbotKeyMode;

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

void Config::createConfigDir() const noexcept
{
    std::error_code ec; std::filesystem::create_directory(path, ec);
}

void Config::openConfigDir() const noexcept
{
    createConfigDir();
    int ret = std::system((WIN32_LINUX("start ", "xdg-open ") + path.string()).c_str());
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
                cfg.RasterizerMultiply = 1.7f;

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
            cfg.RasterizerMultiply = 1.7f;

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
