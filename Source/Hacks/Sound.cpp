#include <array>
#include <functional>
#include <memory>
#include <string_view>

#include <imgui/imgui.h>

#include "../ConfigStructs.h"
#include <CSGO/Entity.h>
#include <CSGO/EntityList.h>
#include <CSGO/LocalPlayer.h>

#include <Interfaces/ClientInterfaces.h>

#include "Sound.h"

static struct SoundConfig {
    int chickenVolume = 100;

    struct Player {
        int masterVolume = 100;
        int headshotVolume = 100;
        int weaponVolume = 100;
        int footstepVolume = 100;
    };

    std::array<Player, 3> players;
} soundConfig;

void Sound::modulateSound(const ClientInterfaces& clientInterfaces, const Memory& memory, std::string_view name, int entityIndex, float& volume) noexcept
{
    auto modulateVolume = [&](int SoundConfig::Player::* proj) {
        if (const auto entity = csgo::Entity::from(retSpoofGadgets->client, clientInterfaces.getEntityList().getEntity(entityIndex)); localPlayer && entity.getPOD() != nullptr && entity.isPlayer()) {
            if (entityIndex == localPlayer.get().getNetworkable().index())
                volume *= std::invoke(proj, soundConfig.players[0]) / 100.0f;
            else if (!entity.isOtherEnemy(memory, localPlayer.get()))
                volume *= std::invoke(proj, soundConfig.players[1]) / 100.0f;
            else
                volume *= std::invoke(proj, soundConfig.players[2]) / 100.0f;
        }
    };

    modulateVolume(&SoundConfig::Player::masterVolume);

    using namespace std::literals;

    if (name == "Player.DamageHelmetFeedback"sv)
        modulateVolume(&SoundConfig::Player::headshotVolume);
    else if (name.find("Weapon"sv) != std::string_view::npos && name.find("Single"sv) != std::string_view::npos)
        modulateVolume(&SoundConfig::Player::weaponVolume);
    else if (name.find("Step"sv) != std::string_view::npos)
        modulateVolume(&SoundConfig::Player::footstepVolume);
    else if (name.find("Chicken"sv) != std::string_view::npos)
       volume *= soundConfig.chickenVolume / 100.0f;
}

void Sound::tabItem() noexcept
{
    if (ImGui::BeginTabItem("音量效果")) {
        drawGUI(true);
        ImGui::EndTabItem();
    }
}

static bool soundWindowOpen = false;

void Sound::menuBarItem() noexcept
{
    if (ImGui::MenuItem("音量效果")) {
        soundWindowOpen = true;
        ImGui::SetWindowFocus("音量效果");
        ImGui::SetWindowPos("音量效果", { 100.0f, 100.0f });
    }
}

void Sound::drawGUI(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!soundWindowOpen)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("音量效果", &soundWindowOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    }

    ImGui::PushID("音量效果");
    ImGui::SliderInt("鸡鸡音效", &soundConfig.chickenVolume, 0, 200, "%d%%");

    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::Combo("", &currentCategory, "自己\0队友\0敌人\0");
    ImGui::PopItemWidth();
    ImGui::SliderInt("全局音量", &soundConfig.players[currentCategory].masterVolume, 0, 200, "%d%%");
    ImGui::SliderInt("爆头音量", &soundConfig.players[currentCategory].headshotVolume, 0, 200, "%d%%");
    ImGui::SliderInt("武器音量", &soundConfig.players[currentCategory].weaponVolume, 0, 200, "%d%%");
    ImGui::SliderInt("脚步音量", &soundConfig.players[currentCategory].footstepVolume, 0, 200, "%d%%");
    ImGui::PopID();

    if (!contentOnly)
        ImGui::End();
}

void Sound::resetConfig() noexcept
{
    soundConfig = {};
}

static void to_json(json& j, const SoundConfig::Player& o)
{
    const SoundConfig::Player dummy;

    WRITE("Master volume", masterVolume);
    WRITE("Headshot volume", headshotVolume);
    WRITE("Weapon volume", weaponVolume);
    WRITE("Footstep volume", footstepVolume);
}

json Sound::toJson() noexcept
{
    const SoundConfig dummy;

    json j;
    to_json(j["Chicken volume"], soundConfig.chickenVolume, dummy.chickenVolume);
    j["Players"] = soundConfig.players;
    return j;
}

static void from_json(const json& j, SoundConfig::Player& p)
{
    read(j, "Master volume", p.masterVolume);
    read(j, "Headshot volume", p.headshotVolume);
    read(j, "Weapon volume", p.weaponVolume);
    read(j, "Footstep volume", p.footstepVolume);
}

void Sound::fromJson(const json& j) noexcept
{
    read(j, "Chicken volume", soundConfig.chickenVolume);
    read(j, "Players", soundConfig.players);
}
