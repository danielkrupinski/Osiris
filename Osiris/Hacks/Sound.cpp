#include "../imgui/imgui.h"

#include "../Config.h"
#include "../Interfaces.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/LocalPlayer.h"

#include "Sound.h"

static bool soundWindowOpen = false;

void Sound::modulateSound(const char* name, int entityIndex, float& volume) noexcept
{
    auto modulateVolume = [&](int(*get)(int)) {
        if (const auto entity = interfaces->entityList->getEntity(entityIndex); localPlayer && entity && entity->isPlayer()) {
            if (entityIndex == localPlayer->index())
                volume *= get(0) / 100.0f;
            else if (!entity->isOtherEnemy(localPlayer.get()))
                volume *= get(1) / 100.0f;
            else
                volume *= get(2) / 100.0f;
        }
    };

    modulateVolume([](int index) { return config->sound.players[index].masterVolume; });

    if (std::strstr(name, "Weapon") && std::strstr(name, "Single")) {
        modulateVolume([](int index) { return config->sound.players[index].weaponVolume; });
    }

    if (!std::strcmp(name, "Player.DamageHelmetFeedback"))
        modulateVolume([](int index) { return config->sound.players[index].headshotVolume; });
    else if (std::strstr(name, "Step"))
        modulateVolume([](int index) { return config->sound.players[index].footstepVolume; });
    else if (std::strstr(name, "Chicken"))
       volume *= config->sound.chickenVolume / 100.0f;
}

void Sound::menuBarItem() noexcept
{
    if (ImGui::MenuItem("Sound")) {
        soundWindowOpen = true;
        ImGui::SetWindowFocus("Sound");
        ImGui::SetWindowPos("Sound", { 100.0f, 100.0f });
    }
}

void Sound::tabItem() noexcept
{
    if (ImGui::BeginTabItem("Sound")) {
        drawGUI(true);
        ImGui::EndTabItem();
    }
}

void Sound::drawGUI(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!soundWindowOpen)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("Sound", &soundWindowOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    }
    ImGui::SliderInt("Chicken volume", &config->sound.chickenVolume, 0, 200, "%d%%");

    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::Combo("", &currentCategory, "Local player\0Allies\0Enemies\0");
    ImGui::PopItemWidth();
    ImGui::SliderInt("Master volume", &config->sound.players[currentCategory].masterVolume, 0, 200, "%d%%");
    ImGui::SliderInt("Headshot volume", &config->sound.players[currentCategory].headshotVolume, 0, 200, "%d%%");
    ImGui::SliderInt("Weapon volume", &config->sound.players[currentCategory].weaponVolume, 0, 200, "%d%%");
    ImGui::SliderInt("Footstep volume", &config->sound.players[currentCategory].footstepVolume, 0, 200, "%d%%");

    if (!contentOnly)
        ImGui::End();
}
