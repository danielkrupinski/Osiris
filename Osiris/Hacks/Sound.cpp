#include "../imgui/imgui.h"

#include "Sound.h"

static bool soundWindowOpen = true;

void Sound::menuBarItem() noexcept
{
    if (ImGui::MenuItem("Sound")) {
        soundWindowOpen = true;
        ImGui::SetWindowFocus("Sound");
        ImGui::SetWindowPos("Sound", { 100.0f, 100.0f });
    }
}
