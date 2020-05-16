                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           #include <fstream>
#include <functional>
#include <string>
#include <ShlObj.h>
#include <Windows.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_stdlib.h"

#include "imguiCustom.h"

#include "GUI.h"
#include "Config.h"
#include "Hacks/Misc.h"
#include "Hacks/Reportbot.h"
#include "Hacks/SkinChanger.h"
#include "Hooks.h"
#include "SDK/InputSystem.h"
#include "SDK/SearchEngine.h"
int tab_int;
int tab_intenglish;
static std::vector<SkinChanger::PaintKit> search_result;

constexpr auto windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
| ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

ImVec4 HI(float f) {
    return ImVec4(config->misc.menucol.color[0] - 0.243f, config->misc.menucol.color[1] - 0.102f, config->misc.menucol.color[2] - 0.287f, f);
}

ImVec4 MED(float f) {
    return  ImVec4(config->misc.menucol.color[0], config->misc.menucol.color[1], config->misc.menucol.color[2], f);
}

ImVec4 LOW(float f) {
    return  ImVec4(config->misc.menucol.color[0] - 0.117f, config->misc.menucol.color[1] - 0.251f, config->misc.menucol.color[2] - 0.518f, f);
}

ImVec4 BG(float f) {
    return  ImVec4(0.200f, 0.220f, 0.270f, f);
}

ImVec4 txt(float f) {
    return  ImVec4(0.860f, 0.930f, 0.890f, f);
}

GUI::GUI() noexcept
{
    /*ImGui::CreateContext();
    ImGui_ImplWin32_Init(FindWindowW(L"Valve001", NULL));*/
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text] = txt(0.78f);
    style.Colors[ImGuiCol_TextDisabled] = txt(0.28f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_PopupBg] = BG(0.9f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.317f, 0.505f, 0.788f, 0.00f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = BG(1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = MED(0.78f);
    style.Colors[ImGuiCol_FrameBgActive] = MED(1.00f);
    style.Colors[ImGuiCol_TitleBg] = LOW(1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = HI(1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = BG(0.75f);
    style.Colors[ImGuiCol_MenuBarBg] = BG(0.47f);
    style.Colors[ImGuiCol_ScrollbarBg] = BG(1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = MED(0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = MED(1.00f);
    style.Colors[ImGuiCol_CheckMark] = HI(1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.317f, 0.505f, 0.788f, 0.14f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.317f, 0.505f, 0.788f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_ButtonHovered] = MED(0.86f);
    style.Colors[ImGuiCol_ButtonActive] = MED(1.00f);
    style.Colors[ImGuiCol_Header] = MED(0.76f);
    style.Colors[ImGuiCol_HeaderHovered] = MED(0.86f);
    style.Colors[ImGuiCol_HeaderActive] = HI(1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.317f, 0.505f, 0.788f, 0.04f);
    style.Colors[ImGuiCol_ResizeGripHovered] = MED(0.78f);
    style.Colors[ImGuiCol_ResizeGripActive] = MED(1.00f);
    style.Colors[ImGuiCol_PlotLines] = txt(0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered] = MED(1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = txt(0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = MED(1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = MED(0.43f);
    style.WindowPadding = ImVec2(6, 4);
    style.WindowRounding = 12.0f;
    style.FramePadding = ImVec2(5, 2);
    style.FrameRounding = 0.0f;
    style.ItemSpacing = ImVec2(7, 1);
    style.ItemInnerSpacing = ImVec2(1, 1);
    style.TouchExtraPadding = ImVec2(0, 0);
    style.IndentSpacing = 6.0f;
    style.ScrollbarSize = 12.0f;
    style.ScrollbarRounding = 16.0f;
    style.GrabMinSize = 20.0f;
    style.GrabRounding = 2.0f;

    style.WindowTitleAlign.x = 0.50f;

    style.Colors[ImGuiCol_Border] = ImVec4(0.317f, 0.505f, 0.788f, 0.162f);
    style.FrameBorderSize = 0.0f;
    style.WindowBorderSize = 1.0f;
    style.Colors[ImGuiCol_Text] = ImVec4(0.86f, 0.93f, 0.89f, 0.78f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.86f, 0.93f, 0.89f, 0.28f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.47f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.92f, 0.18f, 0.29f, 0.76f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.92f, 0.18f, 0.29f, 0.43f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.9f);
    style.Alpha = .0f;

    style.WindowPadding = ImVec2(6, 4);
    style.WindowRounding = 0.0f;
    style.FramePadding = ImVec2(5, 2);
    style.FrameRounding = 0.0f;
    style.ItemSpacing = ImVec2(7, 1);
    style.ItemInnerSpacing = ImVec2(1, 1);
    style.TouchExtraPadding = ImVec2(0, 0);
    style.IndentSpacing = 6.0f;
    style.ScrollbarSize = 12.0f;
    style.ScrollbarRounding = 16.0f;
    style.GrabMinSize = 20.0f;
    style.GrabRounding = 2.0f;
    style.FrameBorderSize = 0.0f;
    style.WindowBorderSize = 1.0f;
    style.WindowTitleAlign.x = 0.50f;
    style.Alpha = 1.0;



    style.WindowPadding = ImVec2(6, 4);
    style.WindowRounding = 0.0f;
    style.FramePadding = ImVec2(5, 2);
    style.FrameRounding = 0.0f;
    style.ItemSpacing = ImVec2(7, 1);
    style.ItemInnerSpacing = ImVec2(1, 1);
    style.TouchExtraPadding = ImVec2(0, 0);
    style.IndentSpacing = 6.0f;
    style.ScrollbarSize = 12.0f;
    style.ScrollbarRounding = 16.0f;
    style.GrabMinSize = 20.0f;
    style.GrabRounding = 2.0f;
    style.FrameBorderSize = 0.0f;
    style.WindowBorderSize = 1.0f;
    style.WindowTitleAlign.x = 0.50f;

    style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.00f, 0.40f, 0.41f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 1.00f, 1.00f, 0.65f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.44f, 0.80f, 0.80f, 0.18f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.44f, 0.80f, 0.80f, 0.27f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.44f, 0.81f, 0.86f, 0.66f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.18f, 0.21f, 0.73f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.27f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.22f, 0.29f, 0.30f, 0.71f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.44f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 1.00f, 1.00f, 0.68f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.36f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.76f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.00f, 0.65f, 0.65f, 0.46f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.01f, 1.00f, 1.00f, 0.43f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.62f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.00f, 1.00f, 1.00f, 0.33f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.42f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 1.00f, 1.00f, 0.22f);
    if (PWSTR pathToFonts; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Fonts, 0, nullptr, &pathToFonts))) {
        const std::filesystem::path path{ pathToFonts };
        CoTaskMemFree(pathToFonts);

        static constexpr ImWchar ranges[]{ 0x0020, 0xFFFF, 0 };
        //fonts.tahoma = io.Fonts->AddFontFromFileTTF("c:/windows/fonts/msyhbd.ttc", 18.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
        //fonts.segoeui = io.Fonts->AddFontFromFileTTF("c:/windows/fonts/msyhbd.ttc", 20.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    //newfont

        ImFontConfig cfg;
        cfg.OversampleV = 3;

        fonts.tahoma = io.Fonts->AddFontFromFileTTF("c:/windows/fonts/��������������W.TTF", 13.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
        fonts.segoeui = io.Fonts->AddFontFromFileTTF("c:/windows/fonts/��������������W.TTF", 14.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());


    }
}

void GUI::render() noexcept
{
    renderMenuBar();
}



void GUI::hotkey(int& key) noexcept
{
    switch (config->misc.language) {
    case 0: {
        key ? ImGui::Text("[ %s ]", interfaces->inputSystem->virtualKeyToString(key)) : ImGui::TextUnformatted("[ �ȼ� ]");

        if (!ImGui::IsItemHovered())
            return;

        ImGui::SetTooltip("����������޸��ȼ�");
        break;
    }
    case 1: {
        key ? ImGui::Text("[ %s ]", interfaces->inputSystem->virtualKeyToString(key)) : ImGui::TextUnformatted("[ Hotkey ]");

        if (!ImGui::IsItemHovered())
            return;

        ImGui::SetTooltip("Press anykey to change KeyBind");
        break;
    }

    }
    ImGuiIO& io = ImGui::GetIO();
    for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
        if (ImGui::IsKeyPressed(i) && i != config->misc.menuKey)
            key = i != VK_ESCAPE ? i : 0;

    for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
        if (ImGui::IsMouseDown(i) && i + (i > 1 ? 2 : 1) != config->misc.menuKey)
            key = i + (i > 1 ? 2 : 1);
}

void GUI::renderMenuBar() noexcept
{
    ImGui::SetNextWindowSize({ 700.0f, 700.0f });
    ImGui::Begin("ZyCheat", nullptr, windowFlags);
    {
        switch(config->misc.language)
        { 
        
        case 0: 
        {
            ImGui::SetCursorPos(ImVec2(10, 40));

            if (ImGui::Button("����&���", ImVec2(80, 80)))
                tab_int = 0;

            ImGui::SameLine();

            ImGui::BeginChild("ZyCheat", ImVec2(600, 680), true, 0);
            {
                ImGui::Separator();
                switch (tab_int) {

                    case 0: {
                    static int currentCategory{ 0 };

                    ImGui::PushItemWidth(110.0f);
                    ImGui::PushID(0);
                    ImGui::Combo("", &currentCategory, "ȫ��\0��ǹ\0��������\0���ǹ\0��ǹ\0");
                    ImGui::PopID();
                    ImGui::SameLine();
                    static int currentWeapon{ 0 };
                    ImGui::PushID(1);

                    switch (currentCategory) {
                    case 0: {
                        currentWeapon = 0;
                        ImGui::NewLine();
                        break;
                    }
                    case 1: {
                        static int currentPistol{ 0 };
                        static constexpr const char* pistols[]{ "������ǹ", "�����18", "P2000", "USP-S", "˫�ֱ�����", "P250", "Tec-9", "FN57", "CZ-75", "ɳĮ֮ӥ", "R8����" };

                        ImGui::Combo("", &currentPistol, [](void* data, int idx, const char** out_text) {
                            if (config->aimbot[idx ? idx : 35].enabled) {
                                static std::string name;
                                name = pistols[idx];
                                *out_text = name.append(" *").c_str();
                            }
                            else {
                                *out_text = pistols[idx];
                            }
                            return true;
                            }, nullptr, IM_ARRAYSIZE(pistols));

                        currentWeapon = currentPistol ? currentPistol : 35;
                        break;
                    }
                    case 2: {
                        static int currentHeavy{ 0 };
                        static constexpr const char* heavies[]{ "������������", "����", "XM1014", "�ض�����ǹ", "MAG-7", "M249", "�ڸ��" };

                        ImGui::Combo("", &currentHeavy, [](void* data, int idx, const char** out_text) {
                            if (config->aimbot[idx ? idx + 10 : 36].enabled) {
                                static std::string name;
                                name = heavies[idx];
                                *out_text = name.append(" *").c_str();
                            }
                            else {
                                *out_text = heavies[idx];
                            }
                            return true;
                            }, nullptr, IM_ARRAYSIZE(heavies));

                        currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
                        break;
                    }
                    case 3: {
                        static int currentSmg{ 0 };
                        static constexpr const char* smgs[]{ "���г��ǹ", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PPҰţ" };

                        ImGui::Combo("", &currentSmg, [](void* data, int idx, const char** out_text) {
                            if (config->aimbot[idx ? idx + 16 : 37].enabled) {
                                static std::string name;
                                name = smgs[idx];
                                *out_text = name.append(" *").c_str();
                            }
                            else {
                                *out_text = smgs[idx];
                            }
                            return true;
                            }, nullptr, IM_ARRAYSIZE(smgs));

                        currentWeapon = currentSmg ? currentSmg + 16 : 37;
                        break;
                    }
                    case 4: {
                        static int currentRifle{ 0 };
                        static constexpr const char* rifles[]{ "ȫ����ǹ", "������AR", "����˹", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };

                        ImGui::Combo("", &currentRifle, [](void* data, int idx, const char** out_text) {
                            if (config->aimbot[idx ? idx + 23 : 38].enabled) {
                                static std::string name;
                                name = rifles[idx];
                                *out_text = name.append(" *").c_str();
                            }
                            else {
                                *out_text = rifles[idx];
                            }
                            return true;
                            }, nullptr, IM_ARRAYSIZE(rifles));

                        currentWeapon = currentRifle ? currentRifle + 23 : 38;
                        break;
                    }
                    }
                          ImGui::PopID();
                          ImGui::SameLine();
                          ImGui::Combo("����&���", &config->backtrack.amode, "����\0���\0");

                          ImGui::Separator();
                          ImGui::PushID(2);
                          ImGui::PushItemWidth(70.0f);
                          ImGui::PopItemWidth();
                          ImGui::PopID();
                          ImGui::Columns(2, nullptr, false);
                          ImGui::SetColumnOffset(1, 220.0f);

                          switch (config->backtrack.amode) {

                          case 0: {
                              /*if (config->aimbot[currentWeapon].enabled == true) {
                                  config->ragebot[currentWeapon].enabled = false;
                              }
                              else if (config->ragebot[currentWeapon].enabled == true)
                              {
                                  config->aimbot[currentWeapon].enabled = false;
                              }*/

                              ImGui::Combo("����λ��", &config->aimbot[currentWeapon].bone, "׼�Ǹ���\0����˺�\0ͷ��\0����\0�ع�\0�ز�\0θ��\0ƨ��\0");
                              ImGui::Checkbox("��������", &config->aimbot[currentWeapon].enabled);
                              ImGui::Checkbox("ǿ������", &config->aimbot[currentWeapon].aimlock);
                              ImGui::Checkbox("��Ĭ��׼", &config->aimbot[currentWeapon].silent);
                              ImGui::Checkbox("�Ѿ��˺�", &config->aimbot[currentWeapon].friendlyFire);
                              ImGui::Checkbox("����ɼ�", &config->aimbot[currentWeapon].visibleOnly);
                              ImGui::Checkbox("������ʱ����", &config->aimbot[currentWeapon].scopedOnly);
                              ImGui::Checkbox("��������", &config->aimbot[currentWeapon].ignoreFlash);
                              ImGui::Checkbox("��������", &config->aimbot[currentWeapon].ignoreSmoke);

                               float res = 0.0;

                               ImGui::Checkbox("����������", &config->aimbot[currentWeapon].recoilbasedFov);

                               if (config->aimbot[currentWeapon].recoilbasedFov) {
                                   ImGui::SliderFloat("��ֱѹǹ����", &config->aimbot[currentWeapon].recoilControlX, 0.0f, 1.0f, "%.2f");
                                   ImGui::SliderFloat("ˮƽѹǹ����", &config->aimbot[currentWeapon].recoilControlY, 0.0f, 1.0f, "%.2f");
                                   ImGui::Checkbox("���RCSѹǹ", &config->aimbot[currentWeapon].standaloneRCS);
                                   ImGui::InputInt("��ǹ��", &config->aimbot[currentWeapon].shotsFired);
                                   config->aimbot[currentWeapon].shotsFired = std::clamp(config->aimbot[currentWeapon].shotsFired, 0, 10);
                               }

                               if (config->aimbot[currentWeapon].recoilbasedFov == false) {
                                   config->aimbot[currentWeapon].recoilControlX = res;
                                   config->aimbot[currentWeapon].recoilControlY = res;
                               }
                               



                              ImGui::NextColumn();
                              ImGui::Checkbox("��������", &config->aimbot[currentWeapon].onKey);
                              ImGui::SameLine();
                              ImGui::Text("����:");
                              ImGui::SameLine();
                              hotkey(config->aimbot[currentWeapon].key);
                              ImGui::Checkbox("R8����Ԥ��", &config->misc.prepareRevolver);

                              ImGui::Combo("���鷽ʽ", &config->aimbot[currentWeapon].keyMode, "��ס����\0���¿���\0");
                              ImGui::PushItemWidth(240.0f);
                              ImGui::SliderFloat("��Χ", &config->aimbot[currentWeapon].fov, 0.0f, 8.0f, "%.2f", 2.5f);
                              ImGui::SliderFloat("ƽ��", &config->aimbot[currentWeapon].smooth, 1.0f, 100.0f, "%.2f");
                              ImGui::InputInt("��С�˺�", &config->aimbot[currentWeapon].minDamage, 0, 150);
                              config->aimbot[currentWeapon].minDamage = std::clamp(config->aimbot[currentWeapon].minDamage, 0, 250);
                              ImGui::Checkbox("����Ӧ��׼��λ", &config->aimbot[currentWeapon].killshot);
                              ImGui::Checkbox("�������", &config->aimbot[currentWeapon].betweenShots);
                              //ImGui::Checkbox("�������鷶Χ", &config->misc.drawAimbotFov);
                              ImGui::PopItemWidth();
                              ImGui::Columns(1);
                              break;
                          }

                          case 1: {
                              {
                                  static int currentCategory{ 0 };
                                  ImGui::PushItemWidth(110.0f);
                                  ImGui::PushID(0);
                                  ImGui::Combo("", &currentCategory, "All\0��ǹ\0��������\0���ǹ\0��ǹ\0��˹X27\0");
                                  ImGui::PopID();
                                  ImGui::SameLine();
                                  static int currentWeapon{ 0 };
                                  ImGui::PushID(1);
                                  switch (currentCategory) {
                                  case 0:
                                      currentWeapon = 0;
                                      ImGui::NewLine();
                                      break;
                                  case 5:
                                      currentWeapon = 39;
                                      ImGui::NewLine();
                                      break;

                                  case 1: {
                                      static int currentPistol{ 0 };
                                      static constexpr const char* pistols[]{ "������ǹ", "�����18", "P2000", "USP-S", "˫�ֱ�����", "P250", "Tec-9", "FN57", "CZ-75", "ɳĮ֮ӥ", "R8����" };

                                      ImGui::Combo("", &currentPistol, [](void* data, int idx, const char** out_text) {
                                          if (config->triggerbot[idx ? idx : 35].enabled) {
                                              static std::string name;
                                              name = pistols[idx];
                                              *out_text = name.append(" *").c_str();
                                          }
                                          else {
                                              *out_text = pistols[idx];
                                          }
                                          return true;
                                          }, nullptr, IM_ARRAYSIZE(pistols));

                                      currentWeapon = currentPistol ? currentPistol : 35;
                                      break;
                                  }
                                  case 2: {
                                      static int currentHeavy{ 0 };
                                      static constexpr const char* heavies[]{ "������������", "����", "XM1014", "�ض�����ǹ", "MAG-7", "M249", "�ڸ��" };

                                      ImGui::Combo("", &currentHeavy, [](void* data, int idx, const char** out_text) {
                                          if (config->triggerbot[idx ? idx + 10 : 36].enabled) {
                                              static std::string name;
                                              name = heavies[idx];
                                              *out_text = name.append(" *").c_str();
                                          }
                                          else {
                                              *out_text = heavies[idx];
                                          }
                                          return true;
                                          }, nullptr, IM_ARRAYSIZE(heavies));

                                      currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
                                      break;
                                  }
                                  case 3: {
                                      static int currentSmg{ 0 };
                                      static constexpr const char* smgs[]{ "���г��ǹ", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PPҰţ" };

                                      ImGui::Combo("", &currentSmg, [](void* data, int idx, const char** out_text) {
                                          if (config->triggerbot[idx ? idx + 16 : 37].enabled) {
                                              static std::string name;
                                              name = smgs[idx];
                                              *out_text = name.append(" *").c_str();
                                          }
                                          else {
                                              *out_text = smgs[idx];
                                          }
                                          return true;
                                          }, nullptr, IM_ARRAYSIZE(smgs));

                                      currentWeapon = currentSmg ? currentSmg + 16 : 37;
                                      break;
                                  }
                                  case 4: {
                                      static int currentRifle{ 0 };
                                      static constexpr const char* rifles[]{ "ȫ����ǹ", "������AR", "����˹", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };

                                      ImGui::Combo("", &currentRifle, [](void* data, int idx, const char** out_text) {
                                          if (config->triggerbot[idx ? idx + 23 : 38].enabled) {
                                              static std::string name;
                                              name = rifles[idx];
                                              *out_text = name.append(" *").c_str();
                                          }
                                          else {
                                              *out_text = rifles[idx];
                                          }
                                          return true;
                                          }, nullptr, IM_ARRAYSIZE(rifles));

                                      currentWeapon = currentRifle ? currentRifle + 23 : 38;
                                      break;
                                  }
                                  }
                                  ImGui::PopID();
                                  ImGui::SameLine();
                                  ImGui::Checkbox("����", &config->triggerbot[currentWeapon].enabled);
                                  ImGui::Separator();
                                  ImGui::Checkbox("��������", &config->triggerbot[currentWeapon].onKey);
                                  ImGui::Text("����:");
                                  ImGui::SameLine();
                                  hotkey(config->triggerbot[currentWeapon].key);
                                  ImGui::Checkbox("���Ѿ�����", &config->triggerbot[currentWeapon].friendlyFire);
                                  ImGui::Checkbox("������ʱ����", &config->triggerbot[currentWeapon].scopedOnly);
                                  ImGui::Checkbox("�������ⵯ", &config->triggerbot[currentWeapon].ignoreFlash);
                                  ImGui::Checkbox("��������", &config->triggerbot[currentWeapon].ignoreSmoke);
                                  ImGui::SetNextItemWidth(85.0f);
                                  ImGui::Combo("���λ��", &config->triggerbot[currentWeapon].hitgroup, "���в�λ\0ͷ��\0�ز�\0θ��\0����\0����\0����\0����\0");
                                  ImGui::PushItemWidth(220.0f);
                                  ImGui::SliderInt("��ǹ��ʱ", &config->triggerbot[currentWeapon].shotDelay, 0, 250, "%d ms");
                                  ImGui::InputInt("��С�˺�", &config->triggerbot[currentWeapon].minDamage);
                                  config->triggerbot[currentWeapon].minDamage = std::clamp(config->triggerbot[currentWeapon].minDamage, 0, 250);
                                  ImGui::Checkbox("����Ӧ��׼��λ", &config->triggerbot[currentWeapon].killshot);
                                  ImGui::SliderFloat("����ʱ��", &config->triggerbot[currentWeapon].burstTime, 0.0f, 0.5f, "%.3f s");

                              }
                              break;
                          }

                          }
                          
                    
                    
                    break;
                }
                        //Legit
                    case 1: {


                    break;
                }
                        //Rage
                    case 2: {
                        ImGui::Checkbox("����", &config->antiAim.enabled);
                        ImGui::Checkbox("##pitch", &config->antiAim.pitch);
                        ImGui::SameLine();
                        ImGui::SliderFloat("P��", &config->antiAim.pitchAngle, -89.0f, 89.0f, "%.2f");
                        ImGui::Checkbox("Y��", &config->antiAim.yaw);
                        ImGui::Checkbox("����", &config->antiAim.Slowwalk);
                        ImGui::SliderFloat("�����ٶ�", &config->antiAim.slowwalkspeed, 0, 64, "%d");
                        ImGui::SameLine();
                        hotkey(config->antiAim.slowwalkkey);
                        break;
                    }
                          //AntiAim
                    case 3: {

                        

                        ImGui::BeginChild("Glow", { 400.0f,150.0f });
                        {
                            static int currentCategory{ 0 };
                            ImGui::PushItemWidth(110.0f);
                            ImGui::Text("����͸��");
                            ImGui::SameLine();
                            ImGui::PushID(0);
                            ImGui::Combo("", &currentCategory, "�Ѿ�\0����\0���ڰ���ը����\0���ڲ��ը����\0�������\0����\0C4ը��\0�Ѿ����ŵ�C4\0С��\0��װ��\0Ͷ����\0����\0ʬ��\0");
                            ImGui::PopID();
                            static int currentItem{ 0 };
                            if (currentCategory <= 3) {
                                ImGui::SameLine();
                                static int currentType{ 0 };
                                ImGui::PushID(1);
                                ImGui::Combo("", &currentType, "����\0�ɼ���\0���ɼ���\0");
                                ImGui::PopID();
                                currentItem = currentCategory * 3 + currentType;
                            }
                            else {
                                currentItem = currentCategory + 8;
                            }
                            ImGui::SameLine();
                            ImGui::Checkbox("����", &config->glow[currentItem].enabled);
                            ImGui::Separator();
                            ImGui::Checkbox("��Ѫ����ɫ", &config->glow[currentItem].healthBased);
                            ImGuiCustom::colorPicker("��ɫ", config->glow[currentItem].color.color, nullptr, &config->glow[currentItem].color.rainbow, &config->glow[currentItem].color.rainbowSpeed);
                            ImGui::PushItemWidth(220.0f);
                            ImGui::SliderFloat("��Ե���", &config->glow[currentItem].thickness, 0.0f, 1.0f, "%.2f");
                            ImGui::SliderFloat("������", &config->glow[currentItem].alpha, 0.0f, 1.0f, "%.2f");
                            ImGui::SliderInt("������", &config->glow[currentItem].style, 0, 3);
                            ImGui::EndChild();
                        }


                        ImGui::Separator();

                        ImGui::BeginChild("Chams", { 400.0f,150.0f });
                        {
                            static int currentCategorys{ 0 };
                            ImGui::PushItemWidth(110.0f);
                            ImGui::Text("��ɫ͸��");
                            ImGui::SameLine();
                            ImGui::PushID(0);
                            ImGui::Combo("", &currentCategorys, "�Ѿ�\0����\0����\0��\0����\0����\0�ֲ�\0����\0����\0");
                            ImGui::PopID();
                            static int currentItems{ 0 };

                            if (currentCategorys <= 3) {
                                ImGui::SameLine();
                                static int currentTypes{ 0 };
                                ImGui::PushID(1);
                                ImGui::Combo("", &currentTypes, "����\0�ɼ���\0���ɼ���\0");
                                ImGui::PopID();
                                currentItems = currentCategorys * 3 + currentTypes;
                            }
                            else {
                                currentItems = currentCategorys + 8;
                            }

                            ImGui::SameLine();
                            static int material = 1;

                            if (ImGui::ArrowButton("##left", ImGuiDir_Left) && material > 1)
                                --material;
                            ImGui::SameLine();
                            ImGui::Text("%d", material);
                            ImGui::SameLine();
                            if (ImGui::ArrowButton("##right", ImGuiDir_Right) && material < int(config->chams[0].materials.size()))
                                ++material;

                            ImGui::SameLine();
                            auto& chams{ config->chams[currentItems].materials[material - 1] };

                            ImGui::Checkbox("����", &chams.enabled);
                            ImGui::Separator();
                            ImGui::Checkbox("��Ѫ����ɫ", &chams.healthBased);
                            ImGui::Checkbox("��˸ ", &chams.blinking);
                            ImGui::Combo("����", &chams.material, "��ͨ\0��ɫ\0��̬\0����\0Glass\0Chrome\0Crystal\0Silver\0Gold\0Plastic\0Glow\0");
                            ImGui::Checkbox("����", &chams.wireframe);
                            ImGuiCustom::colorPicker("��ɫ", chams.color.color, nullptr, &chams.color.rainbow, &chams.color.rainbowSpeed);
                            ImGui::SetNextItemWidth(220.0f);
                            ImGui::SliderFloat("���", &chams.alpha, 0.0f, 1.0f, "%.2f");
                            ImGui::EndChild();
                        }

                        ImGui::Separator();

                        ImGui::BeginChild("Esp", { 500.0f,500.0f });
                        {
                            static int currentCategory = 0;
                            static int currentItem = 0;

                            if (ImGui::ListBoxHeader("##", { 125.0f, 475.0f })) {
                                static constexpr const char* players[]{ "����", "�ɼ���", "���ɼ���" };

                                ImGui::Text("�Ѿ�");
                                ImGui::Indent();
                                ImGui::PushID("Allies");
                                ImGui::PushFont(fonts.segoeui);

                                for (int i = 0; i < IM_ARRAYSIZE(players); i++) {
                                    bool isSelected = currentCategory == 0 && currentItem == i;

                                    if ((i == 0 || !config->esp.players[0].enabled) && ImGui::Selectable(players[i], isSelected)) {
                                        currentItem = i;
                                        currentCategory = 0;
                                    }
                                }

                                ImGui::PopFont();
                                ImGui::PopID();
                                ImGui::Unindent();
                                ImGui::Text("����");
                                ImGui::Indent();
                                ImGui::PushID("Enemies");
                                ImGui::PushFont(fonts.segoeui);

                                for (int i = 0; i < IM_ARRAYSIZE(players); i++) {
                                    bool isSelected = currentCategory == 1 && currentItem == i;

                                    if ((i == 0 || !config->esp.players[3].enabled) && ImGui::Selectable(players[i], isSelected)) {
                                        currentItem = i;
                                        currentCategory = 1;
                                    }
                                }

                                ImGui::PopFont();
                                ImGui::PopID();
                                ImGui::Unindent();
                                if (bool isSelected = currentCategory == 2; ImGui::Selectable("Weapons", isSelected))
                                    currentCategory = 2;

                                ImGui::Text("Ͷ����");
                                ImGui::Indent();
                                ImGui::PushID("Projectiles");
                                ImGui::PushFont(fonts.segoeui);
                                static constexpr const char* projectiles[]{ "���ⵯ", "����", "ң��ը��", "����ը��", "�ն�����", "ȼ��ƿ", "ս��̽������", "����", "ѩ��" };

                                for (int i = 0; i < IM_ARRAYSIZE(projectiles); i++) {
                                    bool isSelected = currentCategory == 3 && currentItem == i;

                                    if (ImGui::Selectable(projectiles[i], isSelected)) {
                                        currentItem = i;
                                        currentCategory = 3;
                                    }
                                }

                                ImGui::PopFont();
                                ImGui::PopID();
                                ImGui::Unindent();

                                ImGui::Text("ͷ����ѵ");
                                ImGui::Indent();
                                ImGui::PushID("Danger Zone");
                                ImGui::PushFont(fonts.segoeui);
                                static constexpr const char* dangerZone[]{ "�ڽ��ǹ", "���˻�", "�ֽ�", "�ֽ����", "��ǹ������", "��������", "��������", "�ױ�ը����", "������", "ȫ��", "���", "ͷ��", "����ɡ", "�߷�Ŀ��", "ƽ��������", "Exo��Ծ", "��ҩ��", "�״������" };

                                for (int i = 0; i < IM_ARRAYSIZE(dangerZone); i++) {
                                    bool isSelected = currentCategory == 4 && currentItem == i;

                                    if (ImGui::Selectable(dangerZone[i], isSelected)) {
                                        currentItem = i;
                                        currentCategory = 4;
                                    }
                                }

                                ImGui::PopFont();
                                ImGui::PopID();
                                ImGui::ListBoxFooter();

                            }
                            ImGui::SameLine();
                            if (ImGui::BeginChild("##child", { 400.0f, 0.0f })) {
                                switch (currentCategory) {
                                case 0:
                                case 1: {
                                    int selected = currentCategory * 3 + currentItem;
                                    ImGui::Checkbox("����", &config->esp.players[selected].enabled);
                                    ImGui::SameLine();
                                    ImGui::SetNextItemWidth(85.0f);
                                    ImGui::InputInt("����", &config->esp.players[selected].font, 1, 294);
                                    config->esp.players[selected].font = std::clamp(config->esp.players[selected].font, 1, 294);

                                    ImGui::Separator();

                                    constexpr auto spacing{ 200.0f };
                                    ImGuiCustom::colorPicker("����", config->esp.players[selected].snaplines);
                                    ImGui::SameLine();
                                    ImGuiCustom::colorPicker("����", config->esp.players[selected].box);
                                    ImGui::SameLine();
                                    ImGui::SetNextItemWidth(95.0f);
                                    ImGui::Combo("", &config->esp.players[selected].boxType, "2D\0""2D corners\0""3D\0""3D corners\0");
                                    ImGuiCustom::colorPicker("����", config->esp.players[selected].eyeTraces);
                                    ImGui::SameLine();
                                    ImGuiCustom::colorPicker("����", config->esp.players[selected].health);
                                    ImGuiCustom::colorPicker("ͷ���׵�", config->esp.players[selected].headDot);
                                    ImGui::SameLine();
                                    ImGuiCustom::colorPicker("��״Ѫ��", config->esp.players[selected].healthBar);
                                    // ImGui::Combo("##HP side", &config->esp.players[selected].hpside, "Left\0Bottom\0Right\0");
                                    ImGui::PushID("hotfix");
                                    ImGuiCustom::colorPicker("����", config->esp.players[selected].name);
                                    ImGui::SameLine();
                                    ImGuiCustom::colorPicker("����", config->esp.players[selected].armor);
                                    ImGuiCustom::colorPicker("��Ǯ", config->esp.players[selected].money);
                                    ImGui::SameLine();
                                    ImGuiCustom::colorPicker("����", config->esp.players[selected].armorBar);
                                    // ImGui::Combo("##AR side", &config->esp.players[selected].armorside, "Left\0Bottom\0Right\0");
                                    ImGuiCustom::colorPicker("����", config->esp.players[selected].outline);
                                    ImGui::SameLine();
                                    ImGuiCustom::colorPicker("����", config->esp.players[selected].distance);
                                    ImGuiCustom::colorPicker("����", config->esp.players[selected].activeWeapon);
                                    ImGui::SameLine();
                                    ImGui::Checkbox("��������", &config->esp.players[selected].deadesp);
                                    ImGui::SliderFloat("�����ƾ���", &config->esp.players[selected].maxDistance, 0.0f, 200.0f, "%.2fm");
                                    break;
                                }
                                case 2: {
                                    ImGui::Checkbox("����", &config->esp.weapon.enabled);
                                    ImGui::SameLine();
                                    ImGui::SetNextItemWidth(85.0f);
                                    ImGui::InputInt("����", &config->esp.weapon.font, 1, 294);
                                    config->esp.weapon.font = std::clamp(config->esp.weapon.font, 1, 294);

                                    ImGui::Separator();

                                    constexpr auto spacing{ 200.0f };
                                    ImGuiCustom::colorPicker("����", config->esp.weapon.snaplines);
                                    ImGui::SameLine();
                                    ImGuiCustom::colorPicker("����", config->esp.weapon.box);
                                    ImGui::SameLine();
                                    ImGui::SetNextItemWidth(95.0f);
                                    ImGui::Combo("", &config->esp.weapon.boxType, "2D\0""2D corners\0""3D\0""3D corners\0");
                                    ImGuiCustom::colorPicker("��ҩ", config->esp.weapon.ammo);
                                    ImGuiCustom::colorPicker("����", config->esp.weapon.name);
                                    ImGui::SameLine();
                                    ImGuiCustom::colorPicker("����", config->esp.weapon.outline);
                                    ImGuiCustom::colorPicker("����", config->esp.weapon.distance);
                                    ImGui::SliderFloat("�����ƾ���", &config->esp.weapon.maxDistance, 0.0f, 200.0f, "%.2fm");
                                    break;
                                }
                                case 3: {
                                    ImGui::Checkbox("����", &config->esp.projectiles[currentItem].enabled);
                                    ImGui::SameLine();
                                    ImGui::SetNextItemWidth(85.0f);
                                    ImGui::InputInt("����", &config->esp.projectiles[currentItem].font, 1, 294);
                                    config->esp.projectiles[currentItem].font = std::clamp(config->esp.projectiles[currentItem].font, 1, 294);

                                    ImGui::Separator();

                                    constexpr auto spacing{ 200.0f };
                                    ImGuiCustom::colorPicker("����", config->esp.projectiles[currentItem].snaplines);
                                    ImGui::SameLine();
                                    ImGuiCustom::colorPicker("����", config->esp.projectiles[currentItem].box);
                                    ImGui::SameLine();
                                    ImGui::SetNextItemWidth(95.0f);
                                    ImGui::Combo("", &config->esp.projectiles[currentItem].boxType, "2D\0""2D corners\0""3D\0""3D corners\0");
                                    ImGuiCustom::colorPicker("����", config->esp.projectiles[currentItem].name);
                                    ImGui::SameLine();
                                    ImGuiCustom::colorPicker("����", config->esp.projectiles[currentItem].outline);
                                    ImGuiCustom::colorPicker("����", config->esp.projectiles[currentItem].distance);
                                    ImGui::SliderFloat("�����ƾ���", &config->esp.projectiles[currentItem].maxDistance, 0.0f, 200.0f, "%.2fm");
                                    break;
                                }
                                case 4: {
                                    {
                                        int selected = currentItem;
                                        ImGui::Checkbox("����", &config->esp.dangerZone[selected].enabled);
                                        ImGui::SameLine();
                                        ImGui::SetNextItemWidth(85.0f);
                                        ImGui::InputInt("����", &config->esp.dangerZone[selected].font, 1, 294);
                                        config->esp.dangerZone[selected].font = std::clamp(config->esp.dangerZone[selected].font, 1, 294);

                                        ImGui::Separator();

                                        constexpr auto spacing{ 200.0f };
                                        ImGuiCustom::colorPicker("����", config->esp.dangerZone[selected].snaplines);
                                        ImGui::SameLine();
                                        ImGuiCustom::colorPicker("����", config->esp.dangerZone[selected].box);
                                        ImGui::SameLine();
                                        ImGui::SetNextItemWidth(95.0f);
                                        ImGui::Combo("", &config->esp.dangerZone[selected].boxType, "2D\0""2D corners\0""3D\0""3D corners\0");
                                        ImGuiCustom::colorPicker("����", config->esp.dangerZone[selected].name);
                                        ImGui::SameLine();
                                        ImGuiCustom::colorPicker("����", config->esp.dangerZone[selected].outline);
                                        ImGuiCustom::colorPicker("����", config->esp.dangerZone[selected].distance);
                                        ImGui::SliderFloat("�����ƾ���", &config->esp.dangerZone[selected].maxDistance, 0.0f, 200.0f, "%.2fm");
                                        break;
                                    }
                                }
                                      ImGui::EndChild();

                                }
                            }



                            ImGui::EndChild();
                            ImGui::EndChild();
                        }

                        break;

                    }
                          //WallHack                
                    case 4: {
                        {
                            ImGui::Columns(2, nullptr, false);
                            ImGui::SetColumnOffset(1, 280.0f);
                            ImGui::PushItemWidth(350.0f);

                            ImGui::Combo("���ģ��", &config->visuals.skybox, "Ĭ��\0cs_baggage_skybox_\0cs_tibet\0embassy\0italy\0jungle\0nukeblank\0office\0sky_cs15_daylight01_hdr\0sky_cs15_daylight02_hdr\0sky_cs15_daylight03_hdr\0sky_cs15_daylight04_hdr\0sky_csgo_cloudy01\0sky_csgo_night_flat\0sky_csgo_night02\0sky_day02_05_hdr\0sky_day02_05\0sky_dust\0sky_l4d_rural02_ldr\0sky_venice\0vertigo_hdr\0vertigo\0vertigoblue_hdr\0vietnam\0");
                           /* if (!config->visuals.fullBright) {
                                ImGui::PushID(5);
                                ImGui::SliderFloat("", &config->visuals.brightness, 0.0f, 1.0f, "����: %.2f");
                                ImGui::PopID();
                            }
                            else

                            {
                                ImGui::PushID(6);
                                ImGui::SliderFloat("", &config->visuals.brightness, 0.0f, 0.0f, "����");
                                ImGui::PopID();
                            };*/
                            ImGuiCustom::colorPicker("������ɫ", config->visuals.world);
                            ImGuiCustom::colorPicker("�����ɫ", config->visuals.sky);
                            ImGui::Checkbox("���ú��ڴ���", &config->visuals.disablePostProcessing);
                            //ImGui::Checkbox("FullBright", &config->visuals.fullBright);
                            ImGui::Checkbox("��ɱ����", &config->visuals.inverseRagdollGravity);
                            ImGui::Checkbox("�Ƴ�����", &config->visuals.noFog);
                            ImGui::Checkbox("�Ƴ�3D���", &config->visuals.no3dSky);
                            ImGui::Checkbox("�Ƴ�����", &config->visuals.noSmoke);
                            ImGui::Checkbox("�Ƴ�ģ��", &config->visuals.noBlur);
                            ImGui::Checkbox("�Ƴ������ڱ�", &config->visuals.noScopeOverlay);
                            ImGui::Checkbox("�Ƴ��ݵ�", &config->visuals.noGrass);
                            ImGui::Checkbox("�Ƴ���Ӱ", &config->visuals.noShadows);
                            ImGui::Checkbox("�߻�����", &config->visuals.wireframeSmoke);



                        }
                        ImGui::NextColumn();



                        ImGui::Checkbox("�Ƴ�ǹе������", &config->visuals.noAimPunch);
                        ImGui::Checkbox("�Ƴ��Ӿ�������", &config->visuals.noViewPunch);
                        ImGui::Checkbox("�Ƴ��ֱ�", &config->visuals.noHands);
                        ImGui::Checkbox("�Ƴ�����", &config->visuals.noSleeves);
                        ImGui::Checkbox("�Ƴ�����", &config->visuals.noWeapons);
                        ImGui::Combo("��ĻЧ��", &config->visuals.screenEffect, "��\0Drone cam\0Drone cam with noise\0Underwater\0Healthboost\0Dangerzone\0");

                        ImGui::Combo("����Ч��", &config->visuals.hitEffect, "��\0Drone cam\0Drone cam with noise\0Underwater\0Healthboost\0ͷ����ѵ\0");
                        ImGui::SliderFloat("����ʱ��", &config->visuals.hitEffectTime, 0.1f, 1.5f, "%.2fs");
                        ImGui::Combo("����Ч��1", &config->visuals.hitMarker, "��\0Ĭ��(��Խ)\0");
                        ImGui::SliderFloat("����ʱ��2", &config->visuals.hitMarkerTime, 0.1f, 1.5f, "%.2fs");
                        ImGui::Checkbox("��ɫ����", &config->visuals.colorCorrection.enabled);
                        ImGui::SameLine();
                        bool ccPopup = ImGui::Button("�༭");

                        if (ccPopup)
                            ImGui::OpenPopup("##popup");

                        if (ImGui::BeginPopup("##popup")) {
                            ImGui::VSliderFloat("##1", { 40.0f, 160.0f }, &config->visuals.colorCorrection.blue, 0.0f, 1.0f, "��ɫ\n%.3f"); ImGui::SameLine();
                            ImGui::VSliderFloat("##2", { 40.0f, 160.0f }, &config->visuals.colorCorrection.red, 0.0f, 1.0f, "��ɫ\n%.3f"); ImGui::SameLine();
                            ImGui::VSliderFloat("##3", { 40.0f, 160.0f }, &config->visuals.colorCorrection.mono, 0.0f, 1.0f, "Mono\n%.3f"); ImGui::SameLine();
                            ImGui::VSliderFloat("##4", { 40.0f, 160.0f }, &config->visuals.colorCorrection.saturation, 0.0f, 1.0f, "Sat\n%.3f"); ImGui::SameLine();
                            ImGui::VSliderFloat("##5", { 40.0f, 160.0f }, &config->visuals.colorCorrection.ghost, 0.0f, 1.0f, "Ghost\n%.3f"); ImGui::SameLine();
                            ImGui::VSliderFloat("##6", { 40.0f, 160.0f }, &config->visuals.colorCorrection.green, 0.0f, 1.0f, "��ɫ\n%.3f"); ImGui::SameLine();
                            ImGui::VSliderFloat("##7", { 40.0f, 160.0f }, &config->visuals.colorCorrection.yellow, 0.0f, 1.0f, "��ɫ\n%.3f"); ImGui::SameLine();
                            ImGui::EndPopup();

                        }
                        ImGui::Checkbox("��Ұ", &config->visuals.zoom);
                        ImGui::SameLine();
                        hotkey(config->visuals.zoomKey);
                        ImGui::Checkbox("�����˳�", &config->visuals.thirdperson);
                        ImGui::SameLine();
                        hotkey(config->visuals.thirdpersonKey);
                        ImGui::SetNextItemWidth(140.0f);
                        /*ImGui::Combo("Thirdperson Angles", &config->antiAim.thirdpersonMode, "����\0����\0Current Tick\0");

                        if (config->visuals.thirdperson)
                        {
                            ImGui::Checkbox("����������", &config->visuals.deadThirdperson);
                        }*/


                        ImGui::PushItemWidth(290.0f);
                        ImGui::PushID(0);
                        ImGui::SliderInt("", &config->visuals.thirdpersonDistance, 0, 1000, "�����˳ƾ���: %d");
                        ImGui::PopID();
                        //ImGui::PushID(1);
                        //ImGui::SliderInt("", &config->visuals.viewmodelFov, -60, 150, "�ֱ۳���: %d");
                        //ImGui::PopID();
                        ImGui::PushID(2);
                        ImGui::SliderInt("", &config->visuals.fov, -60, 60, "��Ұ��С: %d");
                        ImGui::PopID();
                        ImGui::PushID(3);
                        ImGui::SliderInt("", &config->visuals.farZ, 0, 2000, "��Ⱦ����: %d");
                        ImGui::PopID();
                        ImGui::PushID(4);
                        ImGui::SliderInt("", &config->visuals.flashReduction, 0, 100, "���ⵯȥ���̶�: %d%%");
                        ImGui::PopID();

                        break;
                    }
                        //Visual
                    case 5: {
                        static auto itemIndex = 0;

                        ImGui::PushItemWidth(90.0f);
                        ImGui::Combo("##1", &itemIndex, [](void* data, int idx, const char** out_text) {
                            *out_text = game_data::weapon_names[idx].name;
                            return true;
                            }, nullptr, IM_ARRAYSIZE(game_data::weapon_names), 5);
                        ImGui::PopItemWidth();

                        auto& selected_entry = config->skinChanger[itemIndex];
                        selected_entry.itemIdIndex = itemIndex;

                        {
                            ImGui::SameLine();
                            ImGui::Checkbox("����", &selected_entry.enabled);
                            ImGui::Separator();
                            ImGui::Columns(2, nullptr, false);
                            ImGui::InputInt("Ƥ������", &selected_entry.seed);
                            ImGui::InputInt("���������", &selected_entry.stat_trak);
                            ImGui::SliderFloat("ĥ���", &selected_entry.wear, FLT_MIN, 1.f, "%.10f", 5);

                            ImGui::Combo("Ƥ��ѡ��", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text) {
                                *out_text = (itemIndex == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits)[idx].name.c_str();
                                return true;
                                }, nullptr, (itemIndex == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits).size(), 10);


                            ImGui::Combo("Ʒ��", &selected_entry.entity_quality_vector_index, [](void* data, int idx, const char** out_text) {
                                *out_text = game_data::quality_names[idx].name;
                                return true;
                                }, nullptr, IM_ARRAYSIZE(game_data::quality_names), 5);

                            if (itemIndex == 0) {
                                ImGui::Combo("ذ��", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text) {
                                    *out_text = game_data::knife_names[idx].name;
                                    return true;
                                    }, nullptr, IM_ARRAYSIZE(game_data::knife_names), 5);
                            }
                            else if (itemIndex == 1) {
                                ImGui::Combo("����", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text) {
                                    *out_text = game_data::glove_names[idx].name;
                                    return true;
                                    }, nullptr, IM_ARRAYSIZE(game_data::glove_names), 5);
                            }
                            else {
                                static auto unused_value = 0;
                                selected_entry.definition_override_vector_index = 0;
                                ImGui::Combo("�����õ�", &unused_value, "�����ڵ�������\0");
                            }

                            ImGui::InputText("���Ʊ�ǩ", selected_entry.custom_name, 32);

                            ImGui::NextColumn();

                            {
                                ImGui::PushID("sticker");

                                static auto selectedStickerSlot = 0;

                                ImGui::PushItemWidth(-1);

                                ImGui::ListBox("", &selectedStickerSlot, [](void* data, int idx, const char** out_text) {
                                    static char elementName[64];
                                    auto kit_vector_index = config->skinChanger[itemIndex].stickers[idx].kit_vector_index;
                                    sprintf_s(elementName, "#%d (%s)", idx + 1, SkinChanger::stickerKits[kit_vector_index].name.c_str());
                                    *out_text = elementName;
                                    return true;
                                    }, nullptr, 5, 5);

                                ImGui::PopItemWidth();

                                auto& selected_sticker = selected_entry.stickers[selectedStickerSlot];

                                ImGui::Combo("ӡ��ѡ��", &selected_sticker.kit_vector_index, [](void* data, int idx, const char** out_text) {
                                    *out_text = SkinChanger::stickerKits[idx].name.c_str();
                                    return true;
                                    }, nullptr, SkinChanger::stickerKits.size(), 10);

                                ImGui::SliderFloat("ĥ���", &selected_sticker.wear, FLT_MIN, 1.0f, "%.10f", 5.0f);
                                ImGui::SliderFloat("��С����", &selected_sticker.scale, 0.1f, 5.0f);
                                ImGui::SliderFloat("��ת�Ƕ�", &selected_sticker.rotation, 0.0f, 360.0f);

                                ImGui::PopID();
                            }
                            selected_entry.update();

                            ImGui::Columns(1);

                            ImGui::Separator();



                            //Ƥ��������ʼ
                                                  {
                            if (ImGui::Button("Ƥ������", { 178.0f,25.0f }))
                            {
                                ImGui::OpenPopup("Search");
                            }
                            if (ImGui::BeginPopup("Search"))
                            {
                                static char skin_name[256];
                                static int select_current = 0;

                                ImGui::Text("Ƥ������");
                                ImGui::Separator();
                                ImGui::InputText("", skin_name, IM_ARRAYSIZE(skin_name));
                                ImGui::Text("�����Ϸ�����Ƥ��ƴ����ǹе����");
                                ImGui::Separator();
                                if (ImGui::Button("��ʼ����"))
                                {
                                    search_result.clear();

                                    for (auto skin : SkinChanger::skinKits)
                                    {
                                        auto skin_copy = skin;

                                        char in_buffer[1024];
                                        strcpy_s<1024U>(in_buffer, skin_copy.name.c_str());

                                        char* out_buffer = new char[HZ2PY_OUTPUT_BUF_ARRAY_SIZE];

                                        memset(out_buffer, '\0', sizeof(char) * HZ2PY_OUTPUT_BUF_ARRAY_SIZE);

                                        if (is_utf8_string(in_buffer))
                                            pinyin_utf8(in_buffer, out_buffer);
                                        else
                                            pinyin_gb2312(in_buffer, out_buffer, false, false, true, true, true);

                                        if (std::string p(out_buffer); p.find(skin_name) != std::string::npos)
                                        {
                                            skin_copy.name = skin_copy.name + " (" + out_buffer + ")";
                                            search_result.push_back(skin_copy);
                                        }
                                    }
                                }
                                ImGui::SameLine();
                                if (ImGui::Button("ȷ��"))
                                {
                                    for (int i = 0; i < SkinChanger::skinKits.size(); i++)
                                    {
                                        if (SkinChanger::skinKits[i].id == search_result[select_current].id)
                                        {
                                            selected_entry.paint_kit_vector_index = i;
                                            ImGui::CloseCurrentPopup();
                                        }
                                    }
                                }
                                ImGui::ListBox("", &select_current,
                                    [](void* data, int idx, const char** out_text)  -> bool
                                    {
                                        auto& vector = *static_cast<std::vector<SkinChanger::PaintKit>*>(data);
                                        *out_text = vector[idx].name.c_str();
                                        return true;
                                    },
                                    &search_result, search_result.size(), 10);

                                if (ImGui::Button("�˳������˵�"))
                                {
                                    ImGui::CloseCurrentPopup();
                                }

                                ImGui::EndPopup();

                            }

                        }

                            
                            
                            
                            ImGui::Combo("CT ���ģ��", &config->visuals.playerModelCT, "Ĭ��\0Ava�ع� | ��������(FBI)\0���ֱ� | �������֣�FBI���ؾ�\0Markus Delrow | �������֣�FBI������Ӫ�ȶ�\0Michael Syfers | �������֣�FBI���ѻ���\0B Squadronָ�ӹ� | Ӣ���վ��ر𲿶�\0�������ӵ����ֶ�ʿ�� | ����ˮ��ս���ĺ�������\0Ǧ�� | ����ˮ��ս���ĺ�������\0½����ξ����Ricksaw | ����ˮ��ս���ĺ�������\0�������ֱ��� | �¹����ֲ���ͻ����\0'����'McCoy | �����վ�ս�����й��Ʋ���\0Dragomir | Sabre\0׼��������Rezan | Sabre\0��ҽ����Romanov | Sabre\0Maximus | Sabre\0Blackwolf | Sabre\0��ӢMuhlik���� | �������\0�����Ѿ� | �������\0Osiris | �������\0Shahmat���� | �������\0ִ���� | ���սʿ\0���� | ���սʿ\0ǹ�� | ���սʿ\0");
                            ImGui::Combo("T ���ģ��", &config->visuals.playerModelT, "Ĭ��\0Ava�ع� | ��������(FBI)\0���ֱ� | �������֣�FBI���ؾ�\0Markus Delrow | �������֣�FBI������Ӫ�ȶ�\0Michael Syfers | �������֣�FBI���ѻ���\0B Squadronָ�ӹ� | Ӣ���վ��ر𲿶�\0�������ӵ����ֶ�ʿ�� | ����ˮ��ս���ĺ�������\0Ǧ�� | ����ˮ��ս���ĺ�������\0½����ξ����Ricksaw | ����ˮ��ս���ĺ�������\0�������ֱ��� | �¹����ֲ���ͻ����\0'����'McCoy | �����վ�ս�����й��Ʋ���\0Dragomir | Sabre\0׼��������Rezan | Sabre\0��ҽ����Romanov | Sabre\0Maximus | Sabre\0Blackwolf | Sabre\0��ӢMuhlik���� | �������\0�����Ѿ� | �������\0Osiris | �������\0Shahmat���� | �������\0ִ���� | ���սʿ\0���� | ���սʿ\0ǹ�� | ���սʿ\0");
                            /*ImGui::SliderFloat("##Custom Viewmodel X", &config->visuals.viewmodel_x, -100, 100, "�Զ����ֱ۳���X��: %.2f");
                            ImGui::SliderFloat("##Custom Viewmodel Y", &config->visuals.viewmodel_y, -100, 100, "�Զ����ֱ۳���Y��: %.2f");
                            ImGui::SliderFloat("##Custom Viewmodel Z", &config->visuals.viewmodel_z, -100, 100, "�Զ����ֱ۳���Z��: %.2f");*/
                            ImGui::Checkbox("��תɳӥ", &config->visuals.deagleSpinner);
                            if (ImGui::Button("ˢ���޸�", { 130.0f, 30.0f }))
                                SkinChanger::scheduleHudUpdate();
                            
                        }

                        break;
                    }
                    //Skin
                    case 6: {
                        ImGui::Columns(2, nullptr, false);
                        ImGui::SetColumnOffset(1, 230.0f);
                        ImGui::Combo("Language/����ѡ��", &config->misc.language, "����/Chinese\0Ӣ��/English\0");
                        ImGui::Checkbox("��ֹϵͳ�һ��Զ��߳�", &config->misc.antiAfkKick);
                        ImGui::Checkbox("�Զ�����", &config->misc.bunnyHop);
                        /*ImGui::SliderInt("����������", &config->misc.bhopHitchance, 0, 100, "%d%");
                        ImGui::SliderInt("��С������", &config->misc.bhopMinHits, 0, 20, "%d%");
                        ImGui::SliderInt("��������", &config->misc.bhopMaxHits, 0, 20, "%d%");*/
                        ImGui::Checkbox("�Զ�ת��", &config->misc.autoStrafe);
                        ImGui::Checkbox("���ٶ���", &config->misc.fastDuck);
                        ImGui::Checkbox("��������", &config->misc.moonwalk);
                        ImGui::Checkbox("���α�Ե�Զ���Ծ", &config->misc.edgejump);
                        ImGui::SameLine();
                        hotkey(config->misc.edgejumpkey);
                        /*ImGui::Checkbox("����", &config->misc.slowwalk);
                        ImGui::SameLine();
                        hotkey(config->misc.slowwalkKey);*/
                        ImGui::Checkbox("�ѻ�׼��", &config->misc.sniperCrosshair);
                        ImGui::Checkbox("������׼��", &config->misc.recoilCrosshair);
                        ImGui::Checkbox("�Զ���ǹ ", &config->misc.autoPistol);
                        ImGui::Checkbox("�Զ��ϵ�", &config->misc.autoReload);
                        ImGui::Checkbox("�Զ�����", &config->misc.autoAccept);
                        ImGui::Checkbox("�״�͸��", &config->misc.radarHack);
                        ImGui::Checkbox("��ʾ��λ", &config->misc.revealRanks);
                        ImGui::Checkbox("��ʾ��Ǯ", &config->misc.revealMoney);
                        ImGui::Checkbox("�ҷ�������", &config->misc.revealSuspect);
                        ImGuiCustom::colorPicker("�������� ", config->misc.spectatorList);
                        ImGuiCustom::colorPicker("ˮӡ", config->misc.watermark);
                        ImGui::Checkbox("�޸����龫��", &config->misc.fixAnimationLOD);
                        ImGui::Checkbox("�޸����鲿λ", &config->misc.fixBoneMatrix);
                        ImGui::Checkbox("�޸������˶�", &config->misc.fixMovement);
                        ImGui::Checkbox("ģ�ͻ��", &config->misc.disableModelOcclusion);
                        ImGui::SliderFloat("�ݺ��", &config->misc.aspectratio, 0.0f, 5.0f, "%.2f ");
                        ImGui::NextColumn();
                        ImGui::Checkbox("����UI͸����", &config->misc.disablePanoramablur);
                        ImGui::Checkbox("��̬����", &config->misc.animatedClanTag);
                        ImGui::Checkbox("ʱ������", &config->misc.clocktag);
                        ImGui::Checkbox("�Զ�������", &config->misc.customClanTag);
                        ImGui::SameLine();
                        ImGui::PushItemWidth(120.0f);
                        ImGui::PushID(0);
                        if (ImGui::InputText("", &config->misc.clanTag))
                            Misc::updateClanTag(true);
                        ImGui::PopID();

                        ImGui::PushItemWidth(120.0f);

                        ImGui::Checkbox("��ɱ����", &config->misc.killMessage);
                        ImGui::SameLine();
                        ImGui::PushItemWidth(120.0f);
                        ImGui::PushID(1);
                        ImGui::InputText("", &config->misc.killMessageString);
                        ImGui::PopID();
                        /*ImGui::Combo("Fake����ѡ��", &config->misc.nameChangeSelection, "�ر�\0�ٷ��\0�ٽ���(����)\0��ȡ(�Զ���)����");
                        if (config->misc.nameChangeSelection == 1)
                        {
                            ImGui::PushID(3);
                            ImGui::SetNextItemWidth(100.0f);
                            ImGui::Combo("", &config->misc.banColor, "��ɫ\0��ɫ\0��ɫ\0��ɫ\0ǳ��ɫ\0��ɫ\0ǳ��ɫ\0��ɫ\0��ɫ\0��ɫ2\0ǳ��ɫ\0�ϻ�ɫ\0��ɫ\0��ɫ\0����ɫ\0��ɫ\0");
                            ImGui::PopID();
                            ImGui::SameLine();
                            ImGui::PushID(4);
                            ImGui::InputText("", &config->misc.banText);
                            ImGui::PopID();
                            ImGui::SameLine();
                            if (ImGui::Button("�ٷ��"))
                                Misc::fakeBan(true);
                        }
                        else if (config->misc.nameChangeSelection == 2)
                        {
                            ImGui::SetNextItemWidth(200.0f);
                            ImGuiCustom::MultiCombo("Fake Item Flags", config->misc.fakeItemFlags, config->misc.selectedFakeItemFlags, 4);
                            ImGui::SetNextItemWidth(200.0f);
                            ImGui::Combo("��Ӫ", &config->misc.fakeItemTeam, "����\0�˼�");
                            ImGui::SetNextItemWidth(200.0f);
                            ImGui::Combo("Fake��Ϣѡ��", &config->misc.fakeItemMessageType, "�����õ�\0���׻�õ�\0");
                            ImGui::SetNextItemWidth(200.0f);
                            ImGui::Combo("����ѡ��", &config->misc.fakeItemType, "AK-47\0AUG\0AWP\0Bayonet\0Bowie Knife\0Butterfly Knife\0CZ75-Auto\0Classic Knife\0Desert Eagle\0Dual Berettas\0FAMAS\0Falchion Knife\0Five-SeveN\0Flip Knife\0G3SG1\0Galil AR\0Glock-18\0Gut Knife\0Huntsman Knife\0Karambit\0M249\0M4A1-S\0M4A4\0M9 Bayonet\0MAC-10\0MAG-7\0MP5-SD\0MP7\0MP9\0Navaja Knife\0Negev\0Nomad Knife\0Nova\0P2000\0P250\0P90\0PP-Bizon\0Paracord Knife\0R8 Revolver\0SCAR-20\0SG 553\0SSG 08\0Sawed-Off\0Shadow Daggers\0Skeleton Knife\0Spectral Shiv\0Stiletto Knife\0Survival Knife\0Talon Knife\0Tec-9\0UMP-45\0USP-S\0Ursus Knife\0XM1014\0Hand Wraps\0Moto Gloves\0Specialist Gloves\0Sport Gloves\0Bloodhound Gloves\0Hydra Gloves\0Driver Gloves\0");
                            ImGui::SetNextItemWidth(200.0f);
                            ImGui::Combo("Ƥ����ɫ(����)", &config->misc.fakeItemRarity, "��ɫƷ��\0����ɫƷ��\0��ɫƷ��\0��ɫƷ��\0��ɫƷ��\0��ɫƷ��\0��ɫ|��ɫƷ��\0");
                            ImGui::Combo("�����ɫ", &config->misc.fakeItemPlayerColor, "��ɫ\0��ɫ\0��ɫ\0��ɫ\0��ɫ");
                            ImGui::InputText("�������", &config->misc.fakeItemPlayerName);
                            ImGui::InputText("Ƥ������", &config->misc.fakeItemName);
                            if (ImGui::Button("������Ƥ��"))
                                Misc::fakeItem(true);
                        }
                        else if (config->misc.nameChangeSelection == 3)
                        {
                            ImGui::Checkbox("������ȡ", &config->misc.nameStealer);
                            ImGui::InputText("�Զ�������", &config->misc.customName);
                            if (ImGui::Button("�ı��ǳ�"))
                                Misc::setName(true);
                        }*/

                        ImGui::Checkbox("�����°�", &config->misc.fastPlant);
                        //ImGui::Checkbox("C4�˺���", &config->misc.bombDamage);
                        ImGuiCustom::colorPicker("C4��ʱ��", config->misc.bombTimer);
                        ImGui::Checkbox("���ٻ���", &config->misc.quickReload);
                        if (ImGui::Button("�����Զ��������Ч"))
                            config->misc.hitSound = 5;

                        ImGui::Combo("������Ч", &config->misc.hitSound, "��\0����\0SK\0������\0��������\0�Զ���\0");
                        if (config->misc.hitSound == 5) {
                            ImGui::InputText("������Ч�ļ���", &config->misc.customHitSound);
                            if (ImGui::IsItemHovered())
                                ImGui::SetTooltip("����CSGO��Ŀ¼csgo/sound/���������Ч�ļ�xxx.wav");
                        }
                        if (ImGui::Button("�����Զ����ɱ��Ч"))
                            config->misc.killSound = 5;

                        ImGui::PushID(5);
                        ImGui::Combo("��ɱ��Ч", &config->misc.killSound, "��\0����\0SK\0������\0��������\0�Զ���\0");
                        if (config->misc.killSound == 5) {
                            ImGui::InputText("��ɱ��Ч�ļ���", &config->misc.customKillSound);
                            if (ImGui::IsItemHovered())
                                ImGui::SetTooltip("����CSGO��Ŀ¼csgo/sound/���������Ч�ļ�xxx.wav");
                        }
                        ImGui::PopID();
                        ImGui::SetNextItemWidth(90.0f);

                        ImGui::Text("���ٴ���");
                        ImGui::SameLine();
                        hotkey(config->misc.quickHealthshotKey);
                        ImGui::Checkbox("Ͷ����Ԥ����", &config->misc.nadePredict);
                        ImGui::Checkbox("�޸�ͷ����ѵ�ź�", &config->misc.fixTabletSignal);
                        ImGui::SetNextItemWidth(120.0f);
                        ImGui::SliderFloat("����ӽǱ仯", &config->misc.maxAngleDelta, 0.0f, 255.0f, "%.2f");
                        ImGui::Checkbox("ǿ������", &config->misc.fakePrime);
                        ImGui::Checkbox("Bypass_pure", &config->misc.pure);
                        ImGui::Checkbox("�����嵥", &config->misc.purchaseList.enabled);
                        ImGui::SameLine();

                        ImGui::PushID("�����嵥");
                        if (ImGui::Button("..."))
                            ImGui::OpenPopup("");

                        if (ImGui::BeginPopup("")) {
                            ImGui::SetNextItemWidth(75.0f);
                            ImGui::Combo("ģʽ", &config->misc.purchaseList.mode, "��ϸ\0����\0");
                            ImGui::Checkbox("ֻ�ڶ���ʱ������", &config->misc.purchaseList.onlyDuringFreezeTime);
                            ImGui::Checkbox("��ʾ�۸�", &config->misc.purchaseList.showPrices);
                            ImGui::Checkbox("û�д��ڱ���", &config->misc.purchaseList.noTitleBar);
                            ImGui::EndPopup();
                        }
                        ImGui::Text("��������������Ϸ����������������");
                        static int currentCategory{ 0 };
                        ImGui::PushItemWidth(110.0f);
                        ImGui::Combo("", &currentCategory, "����\0�Ѿ�\0����\0");
                        ImGui::PopItemWidth();
                        ImGui::SliderInt("ȫ����Ч", &config->sound.players[currentCategory].masterVolume, 0, 200, "%d%%");
                        ImGui::SliderInt("��ͷ��Ч", &config->sound.players[currentCategory].headshotVolume, 0, 200, "%d%%");
                        ImGui::SliderInt("��ǹ����", &config->sound.players[currentCategory].weaponVolume, 0, 200, "%d%%");
                        ImGui::SliderInt("�Ų�����", &config->sound.players[currentCategory].footstepVolume, 0, 200, "%d%%");
                        ImGui::SliderInt("������", &config->sound.chickenVolume, 0, 200, "%d%%");
                        ImGui::Columns(1);
                        break;
                    }
                        //Misc
                    case 7: {
                        static char buffer[16];
                        {
                            /* static float Gavno = 0.4f;
                             Gavno -= 0.01f;
                             if (Gavno < 0.f)
                                 Gavno += 1.f;

                             static float Gavno2 = 0.1f;
                             Gavno2 -= 0.01f;
                             if (Gavno2 < 0.f)
                                 Gavno2 += 1.f;
                             ImGui::GetWindowDrawList()->AddText(ImVec2(ImGui::GetWindowPos().x + 14, ImGui::GetWindowPos().y + 18), ImColor(0.4f, 0.8f, 0.8f, Gavno), "Shadow Ware No.1");
                             ImGui::GetWindowDrawList()->AddText(ImVec2(ImGui::GetWindowPos().x + 22, ImGui::GetWindowPos().y + 18), ImColor(0.8f, 0.4f, 0.8f, Gavno2), "Shadow Ware No.1");
                             ImGui::GetWindowDrawList()->AddText(ImVec2(ImGui::GetWindowPos().x + 18, ImGui::GetWindowPos().y + 19), ImColor(0.8f, 0.8f, 0.8f, 0.8f), "Shadow Ware No.1");*/

                            ImGui::Columns(2, nullptr, false);

                            ImGui::PushItemWidth(160.0f);

                            auto& configItems = config->getConfigs();
                            static int currentConfig = -1;

                            if (static_cast<size_t>(currentConfig) >= configItems.size())
                                currentConfig = -1;

                            static std::string buffer;

                            if (ImGui::ListBox("", &currentConfig, [](void* data, int idx, const char** out_text) {
                                auto& vector = *static_cast<std::vector<std::string>*>(data);
                                *out_text = vector[idx].c_str();
                                return true;
                                }, &configItems, configItems.size(), 10) && currentConfig != -1)
                                buffer = configItems[currentConfig];


                                ImGui::PushID(0);
                                if (ImGui::InputText("", &buffer, ImGuiInputTextFlags_EnterReturnsTrue)) {
                                    if (currentConfig != -1)
                                        config->rename(currentConfig, buffer.c_str());
                                }
                                ImGui::PopID();



                                if (ImGui::Button("ˢ�²���", { 160.0F, 25.0F }))
                                    //config->Config::Config("ShadowWare");// old  �ļ�������
                                    config->listConfigs(); //New 

                                if (ImGui::Button("��������", { 100.0f, 25.0f }))
                                    config->add(buffer.c_str());

                                if (ImGui::Button("���ò���", { 100.0f, 25.0f }))
                                    ImGui::OpenPopup("���ò���");

                                if (ImGui::BeginPopup("���ò���")) {
                                    static constexpr const char* names[]{ "ȷ������" };
                                    for (int i = 0; i < IM_ARRAYSIZE(names); i++) {
                                        if (i == 1) ImGui::Separator();

                                        if (ImGui::Selectable(names[i])) {
                                            switch (i) {
                                            case 0: config->reset(); Misc::updateClanTag(true); SkinChanger::scheduleHudUpdate(); break;
                                            }
                                        }
                                    }
                                    ImGui::EndPopup();
                                }
                                ImGui::PushItemWidth(100.0f);
                                if (currentConfig != -1) {
                                    if (ImGui::Button("���ز���", { 100.0f, 25.0f })) {
                                        config->load(currentConfig);
                                        SkinChanger::scheduleHudUpdate();
                                        Misc::updateClanTag(true);
                                    }

                                    if (ImGui::Button("�������", { 100.0f, 25.0f }))
                                        config->save(currentConfig);

                                    if (ImGui::Button("ɾ������", { 100.0f, 25.0f }))
                                        config->remove(currentConfig);
                                }



                                ImGui::NextColumn();
                                ImGui::TextUnformatted("Zy-Cheat�˵��ȼ�");
                                ImGui::SameLine();
                                hotkey(config->misc.menuKey);

                                //ImGui::Checkbox("LUA�༭", &LUA_WINDOW);

                                //if (ImGui::Button("����LUA", { 100.0f, 25.0f })) 
                                //c_lua::luaSettingLoad();
                                //if (ImGui::Button("����LUA", { 100.0f, 25.0f }))


                                

                                if (ImGui::Button("�˳�Zy-Cheat"))
                                    hooks->uninstall();



                                break;
                        }
                        //Config  & LUA

                    }
                    }
                
                ImGui::EndChild();
                ImGui::SetCursorPos(ImVec2(10, 120));

                if (ImGui::Button("Updating", ImVec2(80, 80)))
                    tab_int = 1;

                ImGui::SetCursorPos(ImVec2(10, 200));

                if (ImGui::Button("AntiAim", ImVec2(80, 80)))
                    tab_int = 2;

                ImGui::SetCursorPos(ImVec2(10, 280));

                if (ImGui::Button("͸��ѡ��", ImVec2(80, 80)))
                    tab_int = 3;

                ImGui::SetCursorPos(ImVec2(10, 360));

                if (ImGui::Button("�Ӿ��޸�", ImVec2(80, 80)))
                    tab_int = 4;

                ImGui::SetCursorPos(ImVec2(10, 440));

                if (ImGui::Button("Ƥ���޸�", ImVec2(80, 80)))
                    tab_int = 5;

                ImGui::SetCursorPos(ImVec2(10, 520));

                if (ImGui::Button("����", ImVec2(80, 80)))
                    tab_int = 6;

                ImGui::SetCursorPos(ImVec2(10, 600));

                if (ImGui::Button("CFG", ImVec2(80, 80)))
                    tab_int = 7;
                }

                break;
}
        case 1: 
        {
            
                ImGui::SetCursorPos(ImVec2(10, 40));

                if (ImGui::Button("Aimbot", ImVec2(80, 80)))
                    tab_intenglish = 0;

                ImGui::SameLine();

                ImGui::BeginChild("ZyCheat", ImVec2(600, 680), true, 0);
                {
                    ImGui::Separator();
                    switch (tab_intenglish) {

                    case 0: {
                        static int currentCategory{ 0 };

                        ImGui::PushItemWidth(110.0f);
                        ImGui::PushID(0);
                        ImGui::Combo("", &currentCategory, "All\0Pistols\0Heavy\0SMG\0Rifles\0");
                        ImGui::PopID();
                        ImGui::SameLine();
                        static int currentWeapon{ 0 };
                        ImGui::PushID(1);

                        switch (currentCategory) {
                        case 0:
                            currentWeapon = 0;
                            ImGui::NewLine();
                            break;
                        case 1: {
                            static int currentPistol{ 0 };
                            static constexpr const char* pistols[]{ "All", "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-Seven", "CZ-75", "Desert Eagle", "Revolver" };

                            ImGui::Combo("", &currentPistol, [](void* data, int idx, const char** out_text) {
                                if (config->aimbot[idx ? idx : 35].enabled) {
                                    static std::string name;
                                    name = pistols[idx];
                                    *out_text = name.append(" *").c_str();
                                }
                                else {
                                    *out_text = pistols[idx];
                                }
                                return true;
                                }, nullptr, IM_ARRAYSIZE(pistols));

                            currentWeapon = currentPistol ? currentPistol : 35;
                            break;
                        }
                        case 2: {
                            static int currentHeavy{ 0 };
                            static constexpr const char* heavies[]{ "All", "Nova", "XM1014", "Sawed-off", "MAG-7", "M249", "Negev" };

                            ImGui::Combo("", &currentHeavy, [](void* data, int idx, const char** out_text) {
                                if (config->aimbot[idx ? idx + 10 : 36].enabled) {
                                    static std::string name;
                                    name = heavies[idx];
                                    *out_text = name.append(" *").c_str();
                                }
                                else {
                                    *out_text = heavies[idx];
                                }
                                return true;
                                }, nullptr, IM_ARRAYSIZE(heavies));

                            currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
                            break;
                        }
                        case 3: {
                            static int currentSmg{ 0 };
                            static constexpr const char* smgs[]{ "All", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon" };

                            ImGui::Combo("", &currentSmg, [](void* data, int idx, const char** out_text) {
                                if (config->aimbot[idx ? idx + 16 : 37].enabled) {
                                    static std::string name;
                                    name = smgs[idx];
                                    *out_text = name.append(" *").c_str();
                                }
                                else {
                                    *out_text = smgs[idx];
                                }
                                return true;
                                }, nullptr, IM_ARRAYSIZE(smgs));

                            currentWeapon = currentSmg ? currentSmg + 16 : 37;
                            break;
                        }
                        case 4: {
                            static int currentRifle{ 0 };
                            static constexpr const char* rifles[]{ "All", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };

                            ImGui::Combo("", &currentRifle, [](void* data, int idx, const char** out_text) {
                                if (config->aimbot[idx ? idx + 23 : 38].enabled) {
                                    static std::string name;
                                    name = rifles[idx];
                                    *out_text = name.append(" *").c_str();
                                }
                                else {
                                    *out_text = rifles[idx];
                                }
                                return true;
                                }, nullptr, IM_ARRAYSIZE(rifles));

                            currentWeapon = currentRifle ? currentRifle + 23 : 38;
                            break;
                        }
                        }
                        ImGui::PopID();
                        ImGui::SameLine();
                        ImGui::Combo("Mode", &config->backtrack.amode, "Aimbot\0TriggerBot\0");

                        ImGui::Separator();
                        ImGui::PushID(2);
                        ImGui::PushItemWidth(70.0f);
                        ImGui::PopItemWidth();
                        ImGui::PopID();
                        ImGui::Columns(2, nullptr, false);
                        ImGui::SetColumnOffset(1, 220.0f);

                        switch (config->backtrack.amode) {

                        case 0: {
                            /*if (config->aimbot[currentWeapon].enabled == true) {
                                config->ragebot[currentWeapon].enabled = false;
                            }
                            else if (config->ragebot[currentWeapon].enabled == true)
                            {
                                config->aimbot[currentWeapon].enabled = false;
                            }*/

                            ImGui::Combo("Bone", &config->aimbot[currentWeapon].bone, "Nearest\0Best damage\0Head\0Neck\0Sternum\0Chest\0Stomach\0Pelvis\0");
                            ImGui::Checkbox("Enabled", &config->aimbot[currentWeapon].enabled);
                            ImGui::Checkbox("Aimlock", &config->aimbot[currentWeapon].aimlock);
                            ImGui::Checkbox("Silent", &config->aimbot[currentWeapon].silent);
                            ImGui::Checkbox("FriendlyFire", &config->aimbot[currentWeapon].friendlyFire);
                            ImGui::Checkbox("VisibleOnly", &config->aimbot[currentWeapon].visibleOnly);
                            ImGui::Checkbox("ScopedOnly", &config->aimbot[currentWeapon].scopedOnly);
                            ImGui::Checkbox("IgnoreFlash", &config->aimbot[currentWeapon].ignoreFlash);
                            ImGui::Checkbox("IgnoreSmoke", &config->aimbot[currentWeapon].ignoreSmoke);

                            float res = 0.0;

                            ImGui::Checkbox("RecoilbasedFov", &config->aimbot[currentWeapon].recoilbasedFov);

                            if (config->aimbot[currentWeapon].recoilbasedFov) {
                                ImGui::SliderFloat("RecoilControlX", &config->aimbot[currentWeapon].recoilControlX, 0.0f, 1.0f, "%.2f");
                                ImGui::SliderFloat("RecoilControlY", &config->aimbot[currentWeapon].recoilControlY, 0.0f, 1.0f, "%.2f");
                                ImGui::Checkbox("StandaloneRCS", &config->aimbot[currentWeapon].standaloneRCS);
                                ImGui::InputInt("ShotsFired", &config->aimbot[currentWeapon].shotsFired);
                                config->aimbot[currentWeapon].shotsFired = std::clamp(config->aimbot[currentWeapon].shotsFired, 0, 10);
                            }

                            if (config->aimbot[currentWeapon].recoilbasedFov == false) {
                                config->aimbot[currentWeapon].recoilControlX = res;
                                config->aimbot[currentWeapon].recoilControlY = res;
                            }




                            ImGui::NextColumn();
                            ImGui::Checkbox("onKey", &config->aimbot[currentWeapon].onKey);
                            ImGui::SameLine();
                            ImGui::Text("Key:");
                            ImGui::SameLine();
                            hotkey(config->aimbot[currentWeapon].key);
                            

                            ImGui::Combo("KeyMode", &config->aimbot[currentWeapon].keyMode, "Hold\0Toggle\0");
                            ImGui::PushItemWidth(240.0f);
                            ImGui::SliderFloat("Fov", &config->aimbot[currentWeapon].fov, 0.0f, 8.0f, "%.2f", 2.5f);
                            ImGui::SliderFloat("Smooth", &config->aimbot[currentWeapon].smooth, 1.0f, 100.0f, "%.2f");
                            ImGui::InputInt("MinDamage", &config->aimbot[currentWeapon].minDamage, 0, 150);
                            config->aimbot[currentWeapon].minDamage = std::clamp(config->aimbot[currentWeapon].minDamage, 0, 250);
                            ImGui::Checkbox("Killshot", &config->aimbot[currentWeapon].killshot);
                            ImGui::Checkbox("BetweenShots", &config->aimbot[currentWeapon].betweenShots);
                            //ImGui::Checkbox("�������鷶Χ", &config->misc.drawAimbotFov);
                            ImGui::PopItemWidth();
                            ImGui::Columns(1);
                            break;
                        }
                        
                        case 1: {
                            {
                                static int currentCategory{ 0 };
                                ImGui::PushItemWidth(110.0f);
                                ImGui::PushID(0);
                                ImGui::Combo("", &currentCategory, "All\0Pistols\0Heavy\0SMG\0Rifles\0Zeus x27\0");
                                ImGui::PopID();
                                ImGui::SameLine();
                                static int currentWeapon{ 0 };
                                ImGui::PushID(1);
                                switch (currentCategory) {
                                case 0:
                                    currentWeapon = 0;
                                    ImGui::NewLine();
                                    break;
                                case 5:
                                    currentWeapon = 39;
                                    ImGui::NewLine();
                                    break;

                                case 1: {
                                    static int currentPistol{ 0 };
                                    static constexpr const char* pistols[]{ "All", "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-Seven", "CZ-75", "Desert Eagle", "Revolver" };

                                    ImGui::Combo("", &currentPistol, [](void* data, int idx, const char** out_text) {
                                        if (config->triggerbot[idx ? idx : 35].enabled) {
                                            static std::string name;
                                            name = pistols[idx];
                                            *out_text = name.append(" *").c_str();
                                        }
                                        else {
                                            *out_text = pistols[idx];
                                        }
                                        return true;
                                        }, nullptr, IM_ARRAYSIZE(pistols));

                                    currentWeapon = currentPistol ? currentPistol : 35;
                                    break;
                                }
                                case 2: {
                                    static int currentHeavy{ 0 };
                                    static constexpr const char* heavies[]{ "All", "Nova", "XM1014", "Sawed-off", "MAG-7", "M249", "Negev" };

                                    ImGui::Combo("", &currentHeavy, [](void* data, int idx, const char** out_text) {
                                        if (config->triggerbot[idx ? idx + 10 : 36].enabled) {
                                            static std::string name;
                                            name = heavies[idx];
                                            *out_text = name.append(" *").c_str();
                                        }
                                        else {
                                            *out_text = heavies[idx];
                                        }
                                        return true;
                                        }, nullptr, IM_ARRAYSIZE(heavies));

                                    currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
                                    break;
                                }
                                case 3: {
                                    static int currentSmg{ 0 };
                                    static constexpr const char* smgs[]{ "All", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon" };

                                    ImGui::Combo("", &currentSmg, [](void* data, int idx, const char** out_text) {
                                        if (config->triggerbot[idx ? idx + 16 : 37].enabled) {
                                            static std::string name;
                                            name = smgs[idx];
                                            *out_text = name.append(" *").c_str();
                                        }
                                        else {
                                            *out_text = smgs[idx];
                                        }
                                        return true;
                                        }, nullptr, IM_ARRAYSIZE(smgs));

                                    currentWeapon = currentSmg ? currentSmg + 16 : 37;
                                    break;
                                }
                                case 4: {
                                    static int currentRifle{ 0 };
                                    static constexpr const char* rifles[]{ "All", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };

                                    ImGui::Combo("", &currentRifle, [](void* data, int idx, const char** out_text) {
                                        if (config->triggerbot[idx ? idx + 23 : 38].enabled) {
                                            static std::string name;
                                            name = rifles[idx];
                                            *out_text = name.append(" *").c_str();
                                        }
                                        else {
                                            *out_text = rifles[idx];
                                        }
                                        return true;
                                        }, nullptr, IM_ARRAYSIZE(rifles));

                                    currentWeapon = currentRifle ? currentRifle + 23 : 38;
                                    break;
                                }
                                }
                                ImGui::PopID();
                                ImGui::SameLine();
                                ImGui::Checkbox("Enabled", &config->triggerbot[currentWeapon].enabled);
                                ImGui::Separator();
                                ImGui::Checkbox("On key", &config->triggerbot[currentWeapon].onKey);
                                ImGui::SameLine();
                                hotkey(config->triggerbot[currentWeapon].key);
                                ImGui::Checkbox("Friendly fire", &config->triggerbot[currentWeapon].friendlyFire);
                                ImGui::Checkbox("Scoped only", &config->triggerbot[currentWeapon].scopedOnly);
                                ImGui::Checkbox("Ignore flash", &config->triggerbot[currentWeapon].ignoreFlash);
                                ImGui::Checkbox("Ignore smoke", &config->triggerbot[currentWeapon].ignoreSmoke);
                                ImGui::SetNextItemWidth(85.0f);
                                ImGui::Combo("Hitgroup", &config->triggerbot[currentWeapon].hitgroup, "All\0Head\0Chest\0Stomach\0Left arm\0Right arm\0Left leg\0Right leg\0");
                                ImGui::PushItemWidth(220.0f);
                                ImGui::SliderInt("Shot delay", &config->triggerbot[currentWeapon].shotDelay, 0, 250, "%d ms");
                                ImGui::InputInt("Min damage", &config->triggerbot[currentWeapon].minDamage);
                                config->triggerbot[currentWeapon].minDamage = std::clamp(config->triggerbot[currentWeapon].minDamage, 0, 250);
                                ImGui::Checkbox("Killshot", &config->triggerbot[currentWeapon].killshot);
                                ImGui::SliderFloat("Burst Time", &config->triggerbot[currentWeapon].burstTime, 0.0f, 0.5f, "%.3f s");

                            }
                            break;
                        }
                      }       //Legit
                    
                      break;
}
                    
                    case 1: {


                        break;
                    }
                          //Rage

                    case 2: {
                        ImGui::Checkbox("Enabled", &config->antiAim.enabled);
                        ImGui::Checkbox("##pitch", &config->antiAim.pitch);
                        ImGui::SameLine();
                        ImGui::SliderFloat("Pitch", &config->antiAim.pitchAngle, -89.0f, 89.0f, "%.2f");
                        ImGui::Checkbox("Yaw", &config->antiAim.yaw);
                        ImGui::Checkbox("SlowWalk", &config->antiAim.Slowwalk);
                        ImGui::SameLine();
                        hotkey(config->antiAim.slowwalkkey);
                        ImGui::SliderFloat("SlowWalkSpeed", &config->antiAim.slowwalkspeed, 0, 64, "%.2f");
                        
                        break;
                    }
                          //AntiAim
                    case 3: {



                        ImGui::BeginChild("Glow", { 400.0f,150.0f });
                        {
                            static int currentCategory{ 0 };
                            ImGui::PushItemWidth(110.0f);
                            ImGui::PushID(0);
                            ImGui::Combo("", &currentCategory, "Allies\0Enemies\0Planting\0Defusing\0Local player\0Weapons\0C4\0Planted C4\0Chickens\0Defuse kits\0Projectiles\0Hostages\0Ragdolls\0");
                            ImGui::PopID();
                            static int currentItem{ 0 };
                            if (currentCategory <= 3) {
                                ImGui::SameLine();
                                static int currentType{ 0 };
                                ImGui::PushID(1);
                                ImGui::Combo("", &currentType, "All\0Visible\0Occluded\0");
                                ImGui::PopID();
                                currentItem = currentCategory * 3 + currentType;
                            }
                            else {
                                currentItem = currentCategory + 8;
                            }

                            ImGui::SameLine();
                            ImGui::Checkbox("Enabled", &config->glow[currentItem].enabled);
                            ImGui::Separator();
                            ImGui::Columns(2, nullptr, false);
                            ImGui::SetColumnOffset(1, 150.0f);
                            ImGui::Checkbox("Health based", &config->glow[currentItem].healthBased);

                            ImGuiCustom::colorPicker("Color", config->glow[currentItem].color.color, nullptr, &config->glow[currentItem].color.rainbow, &config->glow[currentItem].color.rainbowSpeed);

                            ImGui::NextColumn();
                            ImGui::PushItemWidth(220.0f);
                            ImGui::SliderFloat("Thickness", &config->glow[currentItem].thickness, 0.0f, 1.0f, "%.2f");
                            ImGui::SliderFloat("Alpha", &config->glow[currentItem].alpha, 0.0f, 1.0f, "%.2f");
                            ImGui::SliderInt("Style", &config->glow[currentItem].style, 0, 3);
                            ImGui::Columns(1);
                            
                        }
                        ImGui::EndChild();

                        ImGui::Separator();

                        ImGui::BeginChild("Chams", { 400.0f,150.0f });
                        {
                            static int currentCategory{ 0 };
                            ImGui::PushItemWidth(110.0f);
                            ImGui::PushID(0);
                            ImGui::Combo("", &currentCategory, "Allies\0Enemies\0Planting\0Defusing\0Local player\0Weapons\0Hands\0Backtrack\0Sleeves\0");
                            ImGui::PopID();
                            static int currentItem{ 0 };

                            if (currentCategory <= 3) {
                                ImGui::SameLine();
                                static int currentType{ 0 };
                                ImGui::PushID(1);
                                ImGui::Combo("", &currentType, "All\0Visible\0Occluded\0");
                                ImGui::PopID();
                                currentItem = currentCategory * 3 + currentType;
                            }
                            else {
                                currentItem = currentCategory + 8;
                            }

                            ImGui::SameLine();
                            static int material = 1;

                            if (ImGui::ArrowButton("##left", ImGuiDir_Left) && material > 1)
                                --material;
                            ImGui::SameLine();
                            ImGui::Text("%d", material);
                            ImGui::SameLine();
                            if (ImGui::ArrowButton("##right", ImGuiDir_Right) && material < int(config->chams[0].materials.size()))
                                ++material;

                            ImGui::SameLine();
                            auto& chams{ config->chams[currentItem].materials[material - 1] };

                            ImGui::Checkbox("Enabled", &chams.enabled);
                            ImGui::Separator();
                            ImGui::Checkbox("Health based", &chams.healthBased);
                            ImGui::Checkbox("Blinking", &chams.blinking);
                            ImGui::Combo("Material", &chams.material, "Normal\0Flat\0Animated\0Platinum\0Glass\0Chrome\0Crystal\0Silver\0Gold\0Plastic\0Glow\0");
                            ImGui::Checkbox("Wireframe", &chams.wireframe);
                            ImGuiCustom::colorPicker("Color", chams.color.color, nullptr, &chams.color.rainbow, &chams.color.rainbowSpeed);
                            ImGui::SetNextItemWidth(220.0f);
                            ImGui::SliderFloat("Alpha", &chams.alpha, 0.0f, 1.0f, "%.2f");

                           
                        }
                        ImGui::EndChild();
                        
                        ImGui::Separator();

                        ImGui::BeginChild("Esp", { 500.0f,500.0f });
                        {
                            static int currentCategory = 0;
                            static int currentItem = 0;

                            if (ImGui::ListBoxHeader("##", { 125.0f, 300.0f })) {
                                static constexpr const char* players[]{ "All", "Visible", "Occluded" };

                                ImGui::Text("Allies");
                                ImGui::Indent();
                                ImGui::PushID("Allies");
                                ImGui::PushFont(fonts.segoeui);

                                for (int i = 0; i < IM_ARRAYSIZE(players); i++) {
                                    bool isSelected = currentCategory == 0 && currentItem == i;

                                    if ((i == 0 || !config->esp.players[0].enabled) && ImGui::Selectable(players[i], isSelected)) {
                                        currentItem = i;
                                        currentCategory = 0;
                                    }
                                }

                                ImGui::PopFont();
                                ImGui::PopID();
                                ImGui::Unindent();
                                ImGui::Text("Enemies");
                                ImGui::Indent();
                                ImGui::PushID("Enemies");
                                ImGui::PushFont(fonts.segoeui);

                                for (int i = 0; i < IM_ARRAYSIZE(players); i++) {
                                    bool isSelected = currentCategory == 1 && currentItem == i;

                                    if ((i == 0 || !config->esp.players[3].enabled) && ImGui::Selectable(players[i], isSelected)) {
                                        currentItem = i;
                                        currentCategory = 1;
                                    }
                                }

                                ImGui::PopFont();
                                ImGui::PopID();
                                ImGui::Unindent();
                                if (bool isSelected = currentCategory == 2; ImGui::Selectable("Weapons", isSelected))
                                    currentCategory = 2;

                                ImGui::Text("Projectiles");
                                ImGui::Indent();
                                ImGui::PushID("Projectiles");
                                ImGui::PushFont(fonts.segoeui);
                                static constexpr const char* projectiles[]{ "Flashbang", "HE Grenade", "Breach Charge", "Bump Mine", "Decoy Grenade", "Molotov", "TA Grenade", "Smoke Grenade", "Snowball" };

                                for (int i = 0; i < IM_ARRAYSIZE(projectiles); i++) {
                                    bool isSelected = currentCategory == 3 && currentItem == i;

                                    if (ImGui::Selectable(projectiles[i], isSelected)) {
                                        currentItem = i;
                                        currentCategory = 3;
                                    }
                                }

                                ImGui::PopFont();
                                ImGui::PopID();
                                ImGui::Unindent();

                                ImGui::Text("Danger Zone");
                                ImGui::Indent();
                                ImGui::PushID("Danger Zone");
                                ImGui::PushFont(fonts.segoeui);
                                static constexpr const char* dangerZone[]{ "Sentries", "Drones", "Cash", "Cash Dufflebag", "Pistol Case", "Light Case", "Heavy Case", "Explosive Case", "Tools Case", "Full Armor", "Armor", "Helmet", "Parachute", "Briefcase", "Tablet Upgrade", "ExoJump", "Ammobox", "Radar Jammer" };

                                for (int i = 0; i < IM_ARRAYSIZE(dangerZone); i++) {
                                    bool isSelected = currentCategory == 4 && currentItem == i;

                                    if (ImGui::Selectable(dangerZone[i], isSelected)) {
                                        currentItem = i;
                                        currentCategory = 4;
                                    }
                                }

                                ImGui::PopFont();
                                ImGui::PopID();
                                ImGui::ListBoxFooter();
                            }
                            ImGui::SameLine();
                            if (ImGui::BeginChild("##child", { 400.0f, 0.0f })) {
                                switch (currentCategory) {
                                case 0:
                                case 1: {
                                    int selected = currentCategory * 3 + currentItem;
                                    ImGui::Checkbox("Enabled", &config->esp.players[selected].enabled);
                                    ImGui::SameLine(0.0f, 50.0f);
                                    ImGui::SetNextItemWidth(85.0f);
                                    ImGui::InputInt("Font", &config->esp.players[selected].font, 1, 294);
                                    config->esp.players[selected].font = std::clamp(config->esp.players[selected].font, 1, 294);

                                    ImGui::Separator();

                                    constexpr auto spacing{ 185.0f };
                                    ImGuiCustom::colorPicker("Snaplines", config->esp.players[selected].snaplines);
                                    ImGui::SameLine(spacing);
                                    ImGuiCustom::colorPicker("Box", config->esp.players[selected].box);
                                    ImGui::SameLine();
                                    ImGui::SetNextItemWidth(95.0f);
                                    ImGui::Combo("", &config->esp.players[selected].boxType, "2D\0""2D corners\0""3D\0""3D corners\0");
                                    ImGuiCustom::colorPicker("Eye traces", config->esp.players[selected].eyeTraces);
                                    ImGui::SameLine(spacing);
                                    ImGuiCustom::colorPicker("Health", config->esp.players[selected].health);
                                    ImGuiCustom::colorPicker("Head dot", config->esp.players[selected].headDot);
                                    ImGui::SameLine(spacing);
                                    ImGuiCustom::colorPicker("Health bar", config->esp.players[selected].healthBar);
                                    ImGui::SameLine();
                                    ImGui::SetNextItemWidth(70.f);
                                    ImGui::Combo("##HP side", &config->esp.players[selected].hpside, "Left\0Bottom\0Right\0");
                                    ImGui::PushID("hotfix");
                                    ImGuiCustom::colorPicker("Name", config->esp.players[selected].name);
                                    ImGui::SameLine(spacing);
                                    ImGuiCustom::colorPicker("Armor", config->esp.players[selected].armor);
                                    ImGuiCustom::colorPicker("Money", config->esp.players[selected].money);
                                    ImGui::SameLine(spacing);
                                    ImGuiCustom::colorPicker("Armor bar", config->esp.players[selected].armorBar);
                                    ImGui::SameLine();
                                    ImGui::SetNextItemWidth(70.f);
                                    ImGui::PopID();
                                    ImGui::Combo("##AR side", &config->esp.players[selected].armorside, "Left\0Bottom\0Right\0");
                                    ImGuiCustom::colorPicker("Outline", config->esp.players[selected].outline);
                                    ImGui::SameLine(spacing);
                                    ImGuiCustom::colorPicker("Distance", config->esp.players[selected].distance);
                                    ImGuiCustom::colorPicker("Active Weapon", config->esp.players[selected].activeWeapon);
                                    ImGui::SameLine(spacing);
                                    ImGui::Checkbox("Dead ESP", &config->esp.players[selected].deadesp);
                                    ImGui::SliderFloat("Max distance", &config->esp.players[selected].maxDistance, 0.0f, 200.0f, "%.2fm");
                                    break;
                                }
                                case 2: {
                                    ImGui::Checkbox("Enabled", &config->esp.weapon.enabled);
                                    ImGui::SameLine(0.0f, 50.0f);
                                    ImGui::SetNextItemWidth(85.0f);
                                    ImGui::InputInt("Font", &config->esp.weapon.font, 1, 294);
                                    config->esp.weapon.font = std::clamp(config->esp.weapon.font, 1, 294);

                                    ImGui::Separator();

                                    constexpr auto spacing{ 200.0f };
                                    ImGuiCustom::colorPicker("Snaplines", config->esp.weapon.snaplines);
                                    ImGui::SameLine(spacing);
                                    ImGuiCustom::colorPicker("Box", config->esp.weapon.box);
                                    ImGui::SameLine();
                                    ImGui::SetNextItemWidth(95.0f);
                                    ImGui::Combo("", &config->esp.weapon.boxType, "2D\0""2D corners\0""3D\0""3D corners\0");
                                    ImGuiCustom::colorPicker("Name", config->esp.weapon.name);
                                    ImGui::SameLine(spacing);
                                    ImGuiCustom::colorPicker("Ammo", config->esp.weapon.ammo);
                                    ImGuiCustom::colorPicker("Outline", config->esp.weapon.outline);
                                    ImGuiCustom::colorPicker("Distance", config->esp.weapon.distance);
                                    ImGui::SliderFloat("Max distance", &config->esp.weapon.maxDistance, 0.0f, 200.0f, "%.2fm");
                                    break;
                                }
                                case 3: {
                                    ImGui::Checkbox("Enabled", &config->esp.projectiles[currentItem].enabled);
                                    ImGui::SameLine(0.0f, 50.0f);
                                    ImGui::SetNextItemWidth(85.0f);
                                    ImGui::InputInt("Font", &config->esp.projectiles[currentItem].font, 1, 294);
                                    config->esp.projectiles[currentItem].font = std::clamp(config->esp.projectiles[currentItem].font, 1, 294);

                                    ImGui::Separator();

                                    constexpr auto spacing{ 200.0f };
                                    ImGuiCustom::colorPicker("Snaplines", config->esp.projectiles[currentItem].snaplines);
                                    ImGui::SameLine(spacing);
                                    ImGuiCustom::colorPicker("Box", config->esp.projectiles[currentItem].box);
                                    ImGui::SameLine();
                                    ImGui::SetNextItemWidth(95.0f);
                                    ImGui::Combo("", &config->esp.projectiles[currentItem].boxType, "2D\0""2D corners\0""3D\0""3D corners\0");
                                    ImGuiCustom::colorPicker("Name", config->esp.projectiles[currentItem].name);
                                    ImGui::SameLine(spacing);
                                    ImGuiCustom::colorPicker("Outline", config->esp.projectiles[currentItem].outline);
                                    ImGuiCustom::colorPicker("Distance", config->esp.projectiles[currentItem].distance);
                                    ImGui::SliderFloat("Max distance", &config->esp.projectiles[currentItem].maxDistance, 0.0f, 200.0f, "%.2fm");
                                    break;
                                }
                                case 4: {
                                    int selected = currentItem;
                                    ImGui::Checkbox("Enabled", &config->esp.dangerZone[selected].enabled);
                                    ImGui::SameLine(0.0f, 50.0f);
                                    ImGui::SetNextItemWidth(85.0f);
                                    ImGui::InputInt("Font", &config->esp.dangerZone[selected].font, 1, 294);
                                    config->esp.dangerZone[selected].font = std::clamp(config->esp.dangerZone[selected].font, 1, 294);

                                    ImGui::Separator();

                                    constexpr auto spacing{ 200.0f };
                                    ImGuiCustom::colorPicker("Snaplines", config->esp.dangerZone[selected].snaplines);
                                    ImGui::SameLine(spacing);
                                    ImGuiCustom::colorPicker("Box", config->esp.dangerZone[selected].box);
                                    ImGui::SameLine();
                                    ImGui::SetNextItemWidth(95.0f);
                                    ImGui::Combo("", &config->esp.dangerZone[selected].boxType, "2D\0""2D corners\0""3D\0""3D corners\0");
                                    ImGuiCustom::colorPicker("Name", config->esp.dangerZone[selected].name);
                                    ImGui::SameLine(spacing);
                                    ImGuiCustom::colorPicker("Outline", config->esp.dangerZone[selected].outline);
                                    ImGuiCustom::colorPicker("Distance", config->esp.dangerZone[selected].distance);
                                    ImGui::SliderFloat("Max distance", &config->esp.dangerZone[selected].maxDistance, 0.0f, 200.0f, "%.2fm");
                                    break;
                                }
                                }

                                ImGui::EndChild();



                            }




                           


                            

                        }
                        ImGui::EndChild();
                       
                        //WallHack                
                        break;
 }
                          
                    case 4: {
                        {
                            ImGui::Columns(2, nullptr, false);
                            ImGui::SetColumnOffset(1, 280.0f);
                            ImGui::Checkbox("Disable post-processing", &config->visuals.disablePostProcessing);
                            ImGui::Checkbox("Inverse ragdoll gravity", &config->visuals.inverseRagdollGravity);
                            ImGui::Checkbox("No fog", &config->visuals.noFog);
                            ImGui::Checkbox("No 3d sky", &config->visuals.no3dSky);
                            ImGui::Checkbox("No aim punch", &config->visuals.noAimPunch);
                            ImGui::Checkbox("No view punch", &config->visuals.noViewPunch);
                            ImGui::Checkbox("No hands", &config->visuals.noHands);
                            ImGui::Checkbox("No sleeves", &config->visuals.noSleeves);
                            ImGui::Checkbox("No weapons", &config->visuals.noWeapons);
                            ImGui::Checkbox("No smoke", &config->visuals.noSmoke);
                            ImGui::Checkbox("No blur", &config->visuals.noBlur);
                            ImGui::Checkbox("No scope overlay", &config->visuals.noScopeOverlay);
                            ImGui::Checkbox("No grass", &config->visuals.noGrass);
                            ImGui::Checkbox("No shadows", &config->visuals.noShadows);
                            ImGui::Checkbox("Wireframe smoke", &config->visuals.wireframeSmoke);
                            ImGui::NextColumn();
                            ImGui::Checkbox("Zoom", &config->visuals.zoom);
                            ImGui::SameLine();
                            hotkey(config->visuals.zoomKey);
                            ImGui::Checkbox("Thirdperson", &config->visuals.thirdperson);
                            ImGui::SameLine();
                            hotkey(config->visuals.thirdpersonKey);
                            ImGui::PushItemWidth(290.0f);
                            ImGui::PushID(0);
                            ImGui::SliderInt("", &config->visuals.thirdpersonDistance, 0, 1000, "Thirdperson distance: %d");
                            ImGui::PopID();
                            ImGui::PushID(1);
                            ImGui::SliderInt("", &config->visuals.viewmodelFov, -60, 60, "Viewmodel FOV: %d");
                            ImGui::PopID();
                            ImGui::PushID(2);
                            ImGui::SliderInt("", &config->visuals.fov, -60, 60, "FOV: %d");
                            ImGui::PopID();
                            ImGui::PushID(3);
                            ImGui::SliderInt("", &config->visuals.farZ, 0, 2000, "Far Z: %d");
                            ImGui::PopID();
                            ImGui::PushID(4);
                            ImGui::SliderInt("", &config->visuals.flashReduction, 0, 100, "Flash reduction: %d%%");
                            ImGui::PopID();
                            ImGui::PushID(5);
                            ImGui::SliderFloat("", &config->visuals.brightness, 0.0f, 1.0f, "Brightness: %.2f");
                            ImGui::PopID();
                            ImGui::PopItemWidth();
                            ImGui::Combo("Skybox", &config->visuals.skybox, "Default\0cs_baggage_skybox_\0cs_tibet\0embassy\0italy\0jungle\0nukeblank\0office\0sky_cs15_daylight01_hdr\0sky_cs15_daylight02_hdr\0sky_cs15_daylight03_hdr\0sky_cs15_daylight04_hdr\0sky_csgo_cloudy01\0sky_csgo_night_flat\0sky_csgo_night02\0sky_day02_05_hdr\0sky_day02_05\0sky_dust\0sky_l4d_rural02_ldr\0sky_venice\0vertigo_hdr\0vertigo\0vertigoblue_hdr\0vietnam\0");
                            ImGuiCustom::colorPicker("World color", config->visuals.world);
                            ImGuiCustom::colorPicker("Sky color", config->visuals.sky);
                            ImGui::Checkbox("Deagle spinner", &config->visuals.deagleSpinner);
                            ImGui::Combo("Screen effect", &config->visuals.screenEffect, "None\0Drone cam\0Drone cam with noise\0Underwater\0Healthboost\0Dangerzone\0");
                            ImGui::Combo("Hit effect", &config->visuals.hitEffect, "None\0Drone cam\0Drone cam with noise\0Underwater\0Healthboost\0Dangerzone\0");
                            ImGui::SliderFloat("Hit effect time", &config->visuals.hitEffectTime, 0.1f, 1.5f, "%.2fs");
                            ImGui::Combo("Hit marker", &config->visuals.hitMarker, "None\0Default (Cross)\0");
                            ImGui::SliderFloat("Hit marker time", &config->visuals.hitMarkerTime, 0.1f, 1.5f, "%.2fs");
                            ImGui::Checkbox("Color correction", &config->visuals.colorCorrection.enabled);
                            ImGui::SameLine();
                            bool ccPopup = ImGui::Button("Edit");

                            if (ccPopup)
                                ImGui::OpenPopup("##popup");

                            if (ImGui::BeginPopup("##popup")) {
                                ImGui::VSliderFloat("##1", { 40.0f, 160.0f }, &config->visuals.colorCorrection.blue, 0.0f, 1.0f, "Blue\n%.3f"); ImGui::SameLine();
                                ImGui::VSliderFloat("##2", { 40.0f, 160.0f }, &config->visuals.colorCorrection.red, 0.0f, 1.0f, "Red\n%.3f"); ImGui::SameLine();
                                ImGui::VSliderFloat("##3", { 40.0f, 160.0f }, &config->visuals.colorCorrection.mono, 0.0f, 1.0f, "Mono\n%.3f"); ImGui::SameLine();
                                ImGui::VSliderFloat("##4", { 40.0f, 160.0f }, &config->visuals.colorCorrection.saturation, 0.0f, 1.0f, "Sat\n%.3f"); ImGui::SameLine();
                                ImGui::VSliderFloat("##5", { 40.0f, 160.0f }, &config->visuals.colorCorrection.ghost, 0.0f, 1.0f, "Ghost\n%.3f"); ImGui::SameLine();
                                ImGui::VSliderFloat("##6", { 40.0f, 160.0f }, &config->visuals.colorCorrection.green, 0.0f, 1.0f, "Green\n%.3f"); ImGui::SameLine();
                                ImGui::VSliderFloat("##7", { 40.0f, 160.0f }, &config->visuals.colorCorrection.yellow, 0.0f, 1.0f, "Yellow\n%.3f"); ImGui::SameLine();
                                ImGui::EndPopup();
                            }
                            ImGui::Columns(1);

                            break;
                        }
                    }
                        //Visual
                    case 5: {
                        static auto itemIndex = 0;

                        ImGui::PushItemWidth(90.0f);
                        ImGui::Combo("##1", &itemIndex, [](void* data, int idx, const char** out_text) {
                            *out_text = game_data::weapon_names[idx].name;
                            return true;
                            }, nullptr, IM_ARRAYSIZE(game_data::weapon_names), 5);
                        ImGui::PopItemWidth();

                        auto& selected_entry = config->skinChanger[itemIndex];
                        selected_entry.itemIdIndex = itemIndex;

                        {
                            ImGui::SameLine();
                            ImGui::Checkbox("Enabled", &selected_entry.enabled);
                            ImGui::Separator();
                            ImGui::Columns(2, nullptr, false);
                            ImGui::InputInt("Seed", &selected_entry.seed);
                            ImGui::InputInt("StatTrak", &selected_entry.stat_trak);
                            ImGui::SliderFloat("Wear", &selected_entry.wear, FLT_MIN, 1.f, "%.10f", 5);

                            ImGui::Combo("Paint Kit", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text) {
                                *out_text = (itemIndex == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits)[idx].name.c_str();
                                return true;
                                }, nullptr, (itemIndex == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits).size(), 10);

                            ImGui::Combo("Quality", &selected_entry.entity_quality_vector_index, [](void* data, int idx, const char** out_text) {
                                *out_text = game_data::quality_names[idx].name;
                                return true;
                                }, nullptr, IM_ARRAYSIZE(game_data::quality_names), 5);

                            if (itemIndex == 0) {
                                ImGui::Combo("Knife", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text) {
                                    *out_text = game_data::knife_names[idx].name;
                                    return true;
                                    }, nullptr, IM_ARRAYSIZE(game_data::knife_names), 5);
                            }
                            else if (itemIndex == 1) {
                                ImGui::Combo("Glove", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text) {
                                    *out_text = game_data::glove_names[idx].name;
                                    return true;
                                    }, nullptr, IM_ARRAYSIZE(game_data::glove_names), 5);
                            }
                            else {
                                static auto unused_value = 0;
                                selected_entry.definition_override_vector_index = 0;
                                ImGui::Combo("Unavailable", &unused_value, "For knives or gloves\0");
                            }

                            ImGui::InputText("Name Tag", selected_entry.custom_name, 32);
                        }

                        ImGui::NextColumn();

                        {
                            ImGui::PushID("sticker");

                            static auto selectedStickerSlot = 0;

                            ImGui::PushItemWidth(-1);

                            if (ImGui::ListBoxHeader("", 5)) {
                                for (int i = 0; i < 5; ++i) {
                                    ImGui::PushID(i);

                                    const auto kit_vector_index = config->skinChanger[itemIndex].stickers[i].kit_vector_index;
                                    const std::string text = '#' + std::to_string(i + 1) + "  " + SkinChanger::stickerKits[kit_vector_index].name;

                                    if (ImGui::Selectable(text.c_str(), i == selectedStickerSlot))
                                        selectedStickerSlot = i;

                                    ImGui::PopID();
                                }
                                ImGui::ListBoxFooter();
                            }

                            ImGui::PopItemWidth();

                            auto& selected_sticker = selected_entry.stickers[selectedStickerSlot];

                            ImGui::Combo("Sticker Kit", &selected_sticker.kit_vector_index, [](void* data, int idx, const char** out_text) {
                                *out_text = SkinChanger::stickerKits[idx].name.c_str();
                                return true;
                                }, nullptr, SkinChanger::stickerKits.size(), 10);

                            ImGui::SliderFloat("Wear", &selected_sticker.wear, FLT_MIN, 1.0f, "%.10f", 5.0f);
                            ImGui::SliderFloat("Scale", &selected_sticker.scale, 0.1f, 5.0f);
                            ImGui::SliderFloat("Rotation", &selected_sticker.rotation, 0.0f, 360.0f);

                            ImGui::PopID();
                        }
                            selected_entry.update();

                            ImGui::Columns(1);

                            ImGui::Separator();



                            //Ƥ��������ʼ
                            {
                                if (ImGui::Button("SkinSearch", { 178.0f,25.0f }))
                                {
                                    ImGui::OpenPopup("Search");
                                }
                                if (ImGui::BeginPopup("Search"))
                                {
                                    static char skin_name[256];
                                    static int select_current = 0;

                                    ImGui::Text("SkinName");
                                    ImGui::Separator();
                                    ImGui::InputText("", skin_name, IM_ARRAYSIZE(skin_name));
                                    ImGui::Separator();
                                    if (ImGui::Button("Search"))
                                    {
                                        search_result.clear();

                                        for (auto skin : SkinChanger::skinKits)
                                        {
                                            auto skin_copy = skin;

                                            char in_buffer[1024];
                                            strcpy_s<1024U>(in_buffer, skin_copy.name.c_str());

                                            char* out_buffer = new char[HZ2PY_OUTPUT_BUF_ARRAY_SIZE];

                                            memset(out_buffer, '\0', sizeof(char) * HZ2PY_OUTPUT_BUF_ARRAY_SIZE);

                                            if (is_utf8_string(in_buffer))
                                                pinyin_utf8(in_buffer, out_buffer);
                                            else
                                                pinyin_gb2312(in_buffer, out_buffer, false, false, true, true, true);

                                            if (std::string p(out_buffer); p.find(skin_name) != std::string::npos)
                                            {
                                                skin_copy.name = skin_copy.name + " (" + out_buffer + ")";
                                                search_result.push_back(skin_copy);
                                            }
                                        }
                                    }
                                    ImGui::SameLine();
                                    if (ImGui::Button("Apply"))
                                    {
                                        for (int i = 0; i < SkinChanger::skinKits.size(); i++)
                                        {
                                            if (SkinChanger::skinKits[i].id == search_result[select_current].id)
                                            {
                                                selected_entry.paint_kit_vector_index = i;
                                                ImGui::CloseCurrentPopup();
                                            }
                                        }
                                    }
                                    ImGui::ListBox("", &select_current,
                                        [](void* data, int idx, const char** out_text)  -> bool
                                        {
                                            auto& vector = *static_cast<std::vector<SkinChanger::PaintKit>*>(data);
                                            *out_text = vector[idx].name.c_str();
                                            return true;
                                        },
                                        &search_result, search_result.size(), 10);

                                    if (ImGui::Button("Quit Search"))
                                    {
                                        ImGui::CloseCurrentPopup();
                                    }

                                    ImGui::EndPopup();

                                }

                            }




                            ImGui::Combo("T Player Model", &config->visuals.playerModelT, "Default\0Special Agent Ava | FBI\0Operator | FBI SWAT\0Markus Delrow | FBI HRT\0Michael Syfers | FBI Sniper\0B Squadron Officer | SAS\0Seal Team 6 Soldier | NSWC SEAL\0Buckshot | NSWC SEAL\0Lt. Commander Ricksaw | NSWC SEAL\0Third Commando Company | KSK\0'Two Times' McCoy | USAF TACP\0Dragomir | Sabre\0Rezan The Ready | Sabre\0'The Doctor' Romanov | Sabre\0Maximus | Sabre\0Blackwolf | Sabre\0The Elite Mr. Muhlik | Elite Crew\0Ground Rebel | Elite Crew\0Osiris | Elite Crew\0Prof. Shahmat | Elite Crew\0Enforcer | Phoenix\0Slingshot | Phoenix\0Soldier | Phoenix\0");
                            ImGui::Combo("CT Player Model", &config->visuals.playerModelCT, "Default\0Special Agent Ava | FBI\0Operator | FBI SWAT\0Markus Delrow | FBI HRT\0Michael Syfers | FBI Sniper\0B Squadron Officer | SAS\0Seal Team 6 Soldier | NSWC SEAL\0Buckshot | NSWC SEAL\0Lt. Commander Ricksaw | NSWC SEAL\0Third Commando Company | KSK\0'Two Times' McCoy | USAF TACP\0Dragomir | Sabre\0Rezan The Ready | Sabre\0'The Doctor' Romanov | Sabre\0Maximus | Sabre\0Blackwolf | Sabre\0The Elite Mr. Muhlik | Elite Crew\0Ground Rebel | Elite Crew\0Osiris | Elite Crew\0Prof. Shahmat | Elite Crew\0Enforcer | Phoenix\0Slingshot | Phoenix\0Soldier | Phoenix\0");
                            /*ImGui::SliderFloat("##Custom Viewmodel X", &config->visuals.viewmodel_x, -100, 100, "�Զ����ֱ۳���X��: %.2f");
                            ImGui::SliderFloat("##Custom Viewmodel Y", &config->visuals.viewmodel_y, -100, 100, "�Զ����ֱ۳���Y��: %.2f");
                            ImGui::SliderFloat("##Custom Viewmodel Z", &config->visuals.viewmodel_z, -100, 100, "�Զ����ֱ۳���Z��: %.2f");*/
                            ImGui::Checkbox("DeagleSpinner", &config->visuals.deagleSpinner);
                            if (ImGui::Button("Update", { 130.0f, 30.0f }))
                                SkinChanger::scheduleHudUpdate();
                            //Visuals::customViewmodel();
                        

                        break;
                    }
                         //Skinchanger
                    case 6: {
                        ImGui::Columns(2, nullptr, false);
                        ImGui::SetColumnOffset(1, 230.0f);
                        ImGui::Combo("Language/����ѡ��", &config->misc.language, "����/Chinese\0Ӣ��/English\0");
                        ImGui::Checkbox("Anti AFK kick", &config->misc.antiAfkKick);
                        ImGui::Checkbox("Auto strafe", &config->misc.autoStrafe);
                        ImGui::Checkbox("Bunny hop", &config->misc.bunnyHop);
                        ImGui::Checkbox("Fast duck", &config->misc.fastDuck);
                        ImGui::Checkbox("Moonwalk", &config->misc.moonwalk);
                        ImGui::Checkbox("Edge Jump", &config->misc.edgejump);
                        ImGui::SameLine();
                        hotkey(config->misc.edgejumpkey);
                        /*ImGui::Checkbox("Slowwalk", &config->misc.slowwalk);
                        ImGui::SameLine();
                        hotkey(config->misc.slowwalkKey);*/
                        ImGui::Checkbox("Sniper crosshair", &config->misc.sniperCrosshair);
                        ImGui::Checkbox("Recoil crosshair", &config->misc.recoilCrosshair);
                        ImGui::Checkbox("Auto pistol", &config->misc.autoPistol);
                        ImGui::Checkbox("Auto reload", &config->misc.autoReload);
                        ImGui::Checkbox("Auto accept", &config->misc.autoAccept);
                        ImGui::Checkbox("Radar hack", &config->misc.radarHack);
                        ImGui::Checkbox("Reveal ranks", &config->misc.revealRanks);
                        ImGui::Checkbox("Reveal money", &config->misc.revealMoney);
                        ImGui::Checkbox("Reveal suspect", &config->misc.revealSuspect);
                        ImGuiCustom::colorPicker("Spectator list", config->misc.spectatorList);
                        ImGuiCustom::colorPicker("Watermark", config->misc.watermark);
                        ImGui::Checkbox("Fix animation LOD", &config->misc.fixAnimationLOD);
                        ImGui::Checkbox("Fix bone matrix", &config->misc.fixBoneMatrix);
                        ImGui::Checkbox("Fix movement", &config->misc.fixMovement);
                        ImGui::Checkbox("Disable model occlusion", &config->misc.disableModelOcclusion);
                        ImGui::SliderFloat("Aspect Ratio", &config->misc.aspectratio, 0.0f, 5.0f, "%.2f");
                        ImGui::Text("ReportBot");
                        ImGui::Checkbox("Enabled", &config->reportbot.enabled);
                        ImGui::SameLine(0.0f, 50.0f);
                        if (ImGui::Button("Reset"))
                            Reportbot::reset();
                        ImGui::Separator();
                        ImGui::PushItemWidth(80.0f);
                        ImGui::Combo("Target", &config->reportbot.target, "Enemies\0Allies\0All\0");
                        ImGui::InputInt("Delay (s)", &config->reportbot.delay);
                        config->reportbot.delay = (std::max)(config->reportbot.delay, 1);
                        ImGui::InputInt("Rounds", &config->reportbot.rounds);
                        config->reportbot.rounds = (std::max)(config->reportbot.rounds, 1);
                        ImGui::PopItemWidth();
                        ImGui::Checkbox("Abusive Communications", &config->reportbot.textAbuse);
                        ImGui::Checkbox("Griefing", &config->reportbot.griefing);
                        ImGui::Checkbox("Wall Hacking", &config->reportbot.wallhack);
                        ImGui::Checkbox("Aim Hacking", &config->reportbot.aimbot);
                        ImGui::Checkbox("Other Hacking", &config->reportbot.other);
                        ImGui::NextColumn();
                        ImGui::Checkbox("Disable HUD blur", &config->misc.disablePanoramablur);
                        ImGui::Checkbox("Animated clan tag", &config->misc.animatedClanTag);
                        ImGui::Checkbox("Clock tag", &config->misc.clocktag);
                        ImGui::Checkbox("Custom clantag", &config->misc.customClanTag);
                        ImGui::SameLine();
                        ImGui::PushItemWidth(120.0f);
                        ImGui::PushID(0);
                        if (ImGui::InputText("", &config->misc.clanTag))
                            Misc::updateClanTag(true);
                        ImGui::PopID();
                        ImGui::Checkbox("Kill message", &config->misc.killMessage);
                        ImGui::SameLine();
                        ImGui::PushItemWidth(120.0f);
                        ImGui::PushID(1);
                        ImGui::InputText("", &config->misc.killMessageString);
                        ImGui::PopID();
                        ImGui::Checkbox("Name stealer", &config->misc.nameStealer);
                        ImGui::PushID(3);
                        ImGui::SetNextItemWidth(100.0f);
                        ImGui::Combo("", &config->misc.banColor, "White\0Red\0Purple\0Green\0Light green\0Turquoise\0Light red\0Gray\0Yellow\0Gray 2\0Light blue\0Gray/Purple\0Blue\0Pink\0Dark orange\0Orange\0");
                        ImGui::PopID();
                        ImGui::SameLine();
                        ImGui::PushID(4);
                        ImGui::InputText("", &config->misc.banText);
                        ImGui::PopID();
                        ImGui::SameLine();
                        if (ImGui::Button("Setup fake ban"))
                            Misc::fakeBan(true);
                        ImGui::Checkbox("Fast plant", &config->misc.fastPlant);
                        ImGuiCustom::colorPicker("Bomb timer", config->misc.bombTimer);
                        ImGui::Checkbox("Quick reload", &config->misc.quickReload);
                        ImGui::Checkbox("Prepare revolver", &config->misc.prepareRevolver);
                        ImGui::SameLine();
                        hotkey(config->misc.prepareRevolverKey);
                        ImGui::Combo("Hit Sound", &config->misc.hitSound, "None\0Metal\0Gamesense\0Bell\0Glass\0Custom\0");
                        if (config->misc.hitSound == 5) {
                            ImGui::InputText("Hit Sound filename", &config->misc.customHitSound);
                            if (ImGui::IsItemHovered())
                                ImGui::SetTooltip("audio file must be put in csgo/sound/ directory");
                        }
                        ImGui::PushID(5);
                        ImGui::Combo("Kill Sound", &config->misc.killSound, "None\0Metal\0Gamesense\0Bell\0Glass\0Custom\0");
                        if (config->misc.killSound == 5) {
                            ImGui::InputText("Kill Sound filename", &config->misc.customKillSound);
                            if (ImGui::IsItemHovered())
                                ImGui::SetTooltip("audio file must be put in csgo/sound/ directory");
                        }
                        ImGui::PopID();
                        ImGui::SetNextItemWidth(90.0f);
                        ImGui::InputInt("Choked packets", &config->misc.chokedPackets, 1, 5);
                        config->misc.chokedPackets = std::clamp(config->misc.chokedPackets, 0, 64);
                        ImGui::SameLine();
                        hotkey(config->misc.chokedPacketsKey);
                        ImGui::Text("Quick healthshot");
                        ImGui::SameLine();
                        hotkey(config->misc.quickHealthshotKey);
                        ImGui::Checkbox("Grenade Prediction", &config->misc.nadePredict);
                        ImGui::Checkbox("Fix tablet signal", &config->misc.fixTabletSignal);
                        ImGui::SetNextItemWidth(120.0f);
                        ImGui::SliderFloat("Max angle delta", &config->misc.maxAngleDelta, 0.0f, 255.0f, "%.2f");
                        ImGui::Checkbox("Bypass_pure", &config->misc.pure);
                        ImGui::Checkbox("Fake prime", &config->misc.fakePrime);
                        ImGui::Checkbox("Purchase List", &config->misc.purchaseList.enabled);
                        ImGui::SameLine();

                        ImGui::PushID("Purchase List");
                        if (ImGui::Button("..."))
                            ImGui::OpenPopup("");

                        if (ImGui::BeginPopup("")) {
                            ImGui::SetNextItemWidth(75.0f);
                            ImGui::Combo("Mode", &config->misc.purchaseList.mode, "Details\0Summary\0");
                            ImGui::Checkbox("Only During Freeze Time", &config->misc.purchaseList.onlyDuringFreezeTime);
                            ImGui::Checkbox("Show Prices", &config->misc.purchaseList.showPrices);
                            ImGui::Checkbox("No Title Bar", &config->misc.purchaseList.noTitleBar);
                            ImGui::EndPopup();
                        }
                        ImGui::PopID();
                        ImGui::Text("������������GameSounds������������");
                        static int currentCategory{ 0 };
                        ImGui::PushItemWidth(110.0f);
                        ImGui::Combo("", &currentCategory, "Local player\0Allies\0Enemies\0");
                        ImGui::PopItemWidth();
                        ImGui::SliderInt("MasterVolume", &config->sound.players[currentCategory].masterVolume, 0, 200, "%d%%");
                        ImGui::SliderInt("HeadShotVolume", &config->sound.players[currentCategory].headshotVolume, 0, 200, "%d%%");
                        ImGui::SliderInt("WeaponVolume", &config->sound.players[currentCategory].weaponVolume, 0, 200, "%d%%");
                        ImGui::SliderInt("FootStepVolume", &config->sound.players[currentCategory].footstepVolume, 0, 200, "%d%%");
                        ImGui::SliderInt("ChickenVolume", &config->sound.chickenVolume, 0, 200, "%d%%");
                        ImGui::Columns(1);
                        break;
                    }
                          //Misc
                    case 7: {
                        static char buffer[16];
                        {
                            /* static float Gavno = 0.4f;
                             Gavno -= 0.01f;
                             if (Gavno < 0.f)
                                 Gavno += 1.f;

                             static float Gavno2 = 0.1f;
                             Gavno2 -= 0.01f;
                             if (Gavno2 < 0.f)
                                 Gavno2 += 1.f;
                             ImGui::GetWindowDrawList()->AddText(ImVec2(ImGui::GetWindowPos().x + 14, ImGui::GetWindowPos().y + 18), ImColor(0.4f, 0.8f, 0.8f, Gavno), "Shadow Ware No.1");
                             ImGui::GetWindowDrawList()->AddText(ImVec2(ImGui::GetWindowPos().x + 22, ImGui::GetWindowPos().y + 18), ImColor(0.8f, 0.4f, 0.8f, Gavno2), "Shadow Ware No.1");
                             ImGui::GetWindowDrawList()->AddText(ImVec2(ImGui::GetWindowPos().x + 18, ImGui::GetWindowPos().y + 19), ImColor(0.8f, 0.8f, 0.8f, 0.8f), "Shadow Ware No.1");*/

                            ImGui::Columns(2, nullptr, false);

                            ImGui::PushItemWidth(160.0f);

                            auto& configItems = config->getConfigs();
                            static int currentConfig = -1;

                            if (static_cast<size_t>(currentConfig) >= configItems.size())
                                currentConfig = -1;

                            static std::string buffer;

                            if (ImGui::ListBox("", &currentConfig, [](void* data, int idx, const char** out_text) {
                                auto& vector = *static_cast<std::vector<std::string>*>(data);
                                *out_text = vector[idx].c_str();
                                return true;
                                }, &configItems, configItems.size(), 10) && currentConfig != -1)
                                buffer = configItems[currentConfig];


                                ImGui::PushID(0);
                                if (ImGui::InputText("", &buffer, ImGuiInputTextFlags_EnterReturnsTrue)) {
                                    if (currentConfig != -1)
                                        config->rename(currentConfig, buffer.c_str());
                                }
                                ImGui::PopID();



                                if (ImGui::Button("Refresh", { 160.0F, 25.0F }))
                                    //config->Config::Config("ShadowWare");// old  �ļ�������
                                    config->listConfigs(); //New 

                                if (ImGui::Button("Creat Config", { 100.0f, 25.0f }))
                                    config->add(buffer.c_str());

                                if (ImGui::Button("Reset Config", { 100.0f, 25.0f }))
                                    ImGui::OpenPopup("Reset");

                                if (ImGui::BeginPopup("Reset")) {
                                    static constexpr const char* names[]{ "Reset All" };
                                    for (int i = 0; i < IM_ARRAYSIZE(names); i++) {
                                        if (i == 1) ImGui::Separator();

                                        if (ImGui::Selectable(names[i])) {
                                            switch (i) {
                                            case 0: config->reset(); Misc::updateClanTag(true); SkinChanger::scheduleHudUpdate(); break;
                                            }
                                        }
                                    }
                                    ImGui::EndPopup();
                                }
                                ImGui::PushItemWidth(100.0f);
                                if (currentConfig != -1) {
                                    if (ImGui::Button("Load Config", { 100.0f, 25.0f })) {
                                        config->load(currentConfig);
                                        SkinChanger::scheduleHudUpdate();
                                        Misc::updateClanTag(true);
                                    }

                                    if (ImGui::Button("Save Config", { 100.0f, 25.0f }))
                                        config->save(currentConfig);

                                    if (ImGui::Button("Deleted Config", { 100.0f, 25.0f }))
                                        config->remove(currentConfig);
                                }



                                ImGui::NextColumn();
                                ImGui::TextUnformatted("Menu HotKey");
                                ImGui::SameLine();
                                hotkey(config->misc.menuKey);



                                if (ImGui::Button("Rage Quit"))
                                    hooks->uninstall();



                                



                                break;
                        }
                        //Config  & LUA

                    }
                    }
                
                ImGui::EndChild();
                ImGui::SetCursorPos(ImVec2(10, 120));

                if (ImGui::Button("Updating", ImVec2(80, 80)))
                    tab_intenglish = 1;

                ImGui::SetCursorPos(ImVec2(10, 200));

                if (ImGui::Button("AA", ImVec2(80, 80)))
                    tab_intenglish = 2;

                ImGui::SetCursorPos(ImVec2(10, 280));

                if (ImGui::Button("WallHack", ImVec2(80, 80)))
                    tab_intenglish = 3;

                ImGui::SetCursorPos(ImVec2(10, 360));

                if (ImGui::Button("Visual", ImVec2(80, 80)))
                    tab_intenglish = 4;

                ImGui::SetCursorPos(ImVec2(10, 440));

                if (ImGui::Button("Skinchanger", ImVec2(80, 80)))
                    tab_intenglish = 5;

                ImGui::SetCursorPos(ImVec2(10, 520));

                if (ImGui::Button("Misc", ImVec2(80, 80)))
                    tab_intenglish = 6;

                ImGui::SetCursorPos(ImVec2(10, 600));

                if (ImGui::Button("CFG", ImVec2(80, 80)))
                    tab_intenglish = 7;

                }
                break;
}
        

}

}
}



























                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            