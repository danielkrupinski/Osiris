#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "xorstr.hpp"

#include "GUI.h"
#include "Memory.h"
#include <string>
#include "Interfaces.h"
#include "Config.h"

GUI::GUI()
{
    
}

void GUI::render()
{
    ImGui::SetNextWindowSize(ImVec2(650.0f, 500.0f));
    ImGui::Begin(xorstr("Osiris").crypt_get(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);

    static enum { AIMBOT, VISUALS, MISC } tab{ MISC };
 
    ImGui::PushItemWidth(165);
    if (ImGui::BeginChild(xorstr("#tabs").crypt_get(), ImVec2(0.0f, 460.0f), true)) {
        switch (tab) {
        case AIMBOT:
            if (interfaces.engineClient->IsInGame())
                ImGui::Text(xorstr("Player health: %d").crypt_get(), (*memory.localPlayer)->getHealth());
            break;
        case VISUALS:
            ImGui::Text(xorstr("Visuals tab welcomes!").crypt_get());
            break;
        case MISC:
            ImGui::Text(xorstr("Misc tab welcomes!").crypt_get());
            ImGui::Checkbox(xorstr_("Bunny hop"), &config.misc.bunnyHop);
            break;
        }
    }
    
    ImGui::EndChild();
    ImGui::PopItemWidth();

    if (ImGui::Button(xorstr("Aimbot").crypt_get(), ImVec2(207.0f, 20.0f)))
        tab = AIMBOT;
    ImGui::SameLine();

    if (ImGui::Button(xorstr("Visuals").crypt_get(), ImVec2(207.0f, 20.0f)))
        tab = VISUALS;
    ImGui::SameLine();

    if (ImGui::Button(xorstr("Misc").crypt_get(), ImVec2(207.0f, 20.0f)))
        tab = MISC;

    ImGui::End();
}
