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
int tab_int = 0;


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
    ImGui::CreateContext();
    ImGui_ImplWin32_Init(FindWindowW(L"Valve001", NULL));
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

        fonts.tahoma = io.Fonts->AddFontFromFileTTF("c:/windows/fonts/汉仪铸字美心体W.TTF", 15.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
        fonts.segoeui = io.Fonts->AddFontFromFileTTF("c:/windows/fonts/汉仪铸字美心体W.TTF", 16.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());


    }
}

void GUI::render() noexcept
{
    renderMenuBar();
}



void GUI::hotkey(int& key) noexcept
{
    key ? ImGui::Text("[ %s ]", interfaces->inputSystem->virtualKeyToString(key)) : ImGui::TextUnformatted("[ 热键 ]");

    if (!ImGui::IsItemHovered())
        return;

    ImGui::SetTooltip("按下任意键修改热键");
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
    ImGui::Begin("Shadow Ware V0.7Beta", nullptr, windowFlags);
    {
        {
            ImGui::SetCursorPos(ImVec2(10, 40));

            if (ImGui::Button("自瞄&扳机", ImVec2(80, 80)))
                tab_int = 0;

            ImGui::SameLine();

            ImGui::BeginChild("Shadow Ware V0.7Beta", ImVec2(600, 680), true, 0);
            {
                ImGui::Separator();
                switch (tab_int) {

                case 0: {
                    static int currentCategory{ 0 };

                    ImGui::PushItemWidth(110.0f);
                    ImGui::PushID(0);
                    ImGui::Combo("", &currentCategory, "全部\0手枪\0重型武器\0冲锋枪\0步枪\0");
                    ImGui::PopID();
                    ImGui::SameLine();
                    static int currentWeapon{ 0 };
                    ImGui::PushID(1);

                    switch (currentCategory) {
                    case 0: {
                        currentWeapon = 0;
                        ImGui::NewLine();
                        break;
                    case 1: {
                        static int currentPistol{ 0 };
                        static constexpr const char* pistols[]{ "所有手枪", "格洛克18", "P2000", "USP-S", "双持贝瑞塔", "P250", "Tec-9", "FN57", "CZ-75", "沙漠之鹰", "R8左轮" };

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
                        static constexpr const char* heavies[]{ "所有重型武器", "新星", "XM1014", "截断霰弹枪", "MAG-7", "M249", "内格夫" };

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
                        static constexpr const char* smgs[]{ "所有冲锋枪", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP野牛" };

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
                        static constexpr const char* rifles[]{ "全部步枪", "加利尔AR", "法玛斯", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };

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

                          ImGui::PopID();
                          ImGui::SameLine();
                          ImGui::Combo("自瞄&扳机", &config->backtrack.amode, "自瞄\0扳机\0");

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

                              ImGui::Combo("自瞄位置", &config->aimbot[currentWeapon].bone, "准星附近\0最高伤害\0头部\0脖子\0胸骨\0胸部\0胃部\0屁股\0");
                              ImGui::Checkbox("启用自瞄", &config->aimbot[currentWeapon].enabled);
                              ImGui::Checkbox("强制锁定", &config->aimbot[currentWeapon].aimlock);
                              ImGui::Checkbox("静默瞄准", &config->aimbot[currentWeapon].silent);
                              ImGui::Checkbox("友军伤害", &config->aimbot[currentWeapon].friendlyFire);
                              ImGui::Checkbox("仅瞄可见", &config->aimbot[currentWeapon].visibleOnly);
                              ImGui::Checkbox("仅开镜时启用", &config->aimbot[currentWeapon].scopedOnly);
                              ImGui::Checkbox("无视闪光", &config->aimbot[currentWeapon].ignoreFlash);
                              ImGui::Checkbox("无视烟雾", &config->aimbot[currentWeapon].ignoreSmoke);

                               float res = 0.0;

                               ImGui::Checkbox("后座力控制", &config->aimbot[currentWeapon].recoilbasedFov);

                               if (config->aimbot[currentWeapon].recoilbasedFov) {
                                   ImGui::SliderFloat("垂直压枪力度", &config->aimbot[currentWeapon].recoilControlX, 0.0f, 1.0f, "%.2f");
                                   ImGui::SliderFloat("水平压枪力度", &config->aimbot[currentWeapon].recoilControlY, 0.0f, 1.0f, "%.2f");
                                   ImGui::Checkbox("随机RCS压枪", &config->aimbot[currentWeapon].standaloneRCS);
                                   ImGui::InputInt("空枪数", &config->aimbot[currentWeapon].shotsFired);
                                   config->aimbot[currentWeapon].shotsFired = std::clamp(config->aimbot[currentWeapon].shotsFired, 0, 10);
                               }

                               if (config->aimbot[currentWeapon].recoilbasedFov == false) {
                                   config->aimbot[currentWeapon].recoilControlX = res;
                                   config->aimbot[currentWeapon].recoilControlY = res;
                               }
                               



                              ImGui::NextColumn();
                              ImGui::Checkbox("按键启用", &config->aimbot[currentWeapon].onKey);
                              ImGui::SameLine();
                              ImGui::Text("按键:");
                              ImGui::SameLine();
                              hotkey(config->aimbot[currentWeapon].key);
                              ImGui::Checkbox("R8左轮预热", &config->misc.prepareRevolver);

                              ImGui::Combo("自瞄方式", &config->aimbot[currentWeapon].keyMode, "按住开启\0按下开启\0");
                              ImGui::PushItemWidth(240.0f);
                              ImGui::SliderFloat("范围", &config->aimbot[currentWeapon].fov, 0.0f, 8.0f, "%.2f", 2.5f);
                              ImGui::SliderFloat("平滑", &config->aimbot[currentWeapon].smooth, 1.0f, 100.0f, "%.2f");
                              ImGui::InputInt("最小伤害", &config->aimbot[currentWeapon].minDamage, 0, 150);
                              config->aimbot[currentWeapon].minDamage = std::clamp(config->aimbot[currentWeapon].minDamage, 0, 250);
                              ImGui::Checkbox("自适应瞄准部位", &config->aimbot[currentWeapon].killshot);
                              ImGui::Checkbox("连续射击", &config->aimbot[currentWeapon].betweenShots);
                              //ImGui::Checkbox("绘制自瞄范围", &config->misc.drawAimbotFov);
                              ImGui::PopItemWidth();
                              ImGui::Columns(1);
                              break;
                          }

                          case 1: {
                              {
                                  static int currentCategory{ 0 };
                                  ImGui::PushItemWidth(110.0f);
                                  ImGui::PushID(0);
                                  ImGui::Combo("", &currentCategory, "All\0手枪\0重型武器\0冲锋枪\0步枪\0宙斯X27\0");
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
                                      static constexpr const char* pistols[]{ "所有手枪", "格洛克18", "P2000", "USP-S", "双持贝瑞塔", "P250", "Tec-9", "FN57", "CZ-75", "沙漠之鹰", "R8左轮" };

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
                                      static constexpr const char* heavies[]{ "所有重型武器", "新星", "XM1014", "截断霰弹枪", "MAG-7", "M249", "内格夫" };

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
                                      static constexpr const char* smgs[]{ "所有冲锋枪", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP野牛" };

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
                                      static constexpr const char* rifles[]{ "全部步枪", "加利尔AR", "法玛斯", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };

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
                                  ImGui::Checkbox("启用", &config->triggerbot[currentWeapon].enabled);
                                  ImGui::Separator();
                                  ImGui::Checkbox("按键启用", &config->triggerbot[currentWeapon].onKey);
                                  ImGui::Text("按键:");
                                  ImGui::SameLine();
                                  hotkey(config->triggerbot[currentWeapon].key);
                                  ImGui::Checkbox("对友军启用", &config->triggerbot[currentWeapon].friendlyFire);
                                  ImGui::Checkbox("仅开镜时启用", &config->triggerbot[currentWeapon].scopedOnly);
                                  ImGui::Checkbox("无视闪光弹", &config->triggerbot[currentWeapon].ignoreFlash);
                                  ImGui::Checkbox("无视烟雾弹", &config->triggerbot[currentWeapon].ignoreSmoke);
                                  ImGui::SetNextItemWidth(85.0f);
                                  ImGui::Combo("扳机位置", &config->triggerbot[currentWeapon].hitgroup, "所有部位\0头部\0胸部\0胃部\0左手\0右手\0左腿\0右腿\0");
                                  ImGui::PushItemWidth(220.0f);
                                  ImGui::SliderInt("开枪延时", &config->triggerbot[currentWeapon].shotDelay, 0, 250, "%d ms");
                                  ImGui::InputInt("最小伤害", &config->triggerbot[currentWeapon].minDamage);
                                  config->triggerbot[currentWeapon].minDamage = std::clamp(config->triggerbot[currentWeapon].minDamage, 0, 250);
                                  ImGui::Checkbox("自适应瞄准部位", &config->triggerbot[currentWeapon].killshot);
                                  ImGui::SliderFloat("爆发时间", &config->triggerbot[currentWeapon].burstTime, 0.0f, 0.5f, "%.3f s");

                              }
                              break;
                          }

                          }
                          break;
                    }
                    }
                    
                }
                //Legit

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
                        ImGui::SliderInt("SlowWalkSpeed", &config->antiAim.slowwalkspeed, 0, 64, "%d");
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
                            ImGui::Text("发光透视");
                            ImGui::SameLine();
                            ImGui::PushID(0);
                            ImGui::Combo("", &currentCategory, "友军\0敌人\0正在安放炸弹的\0正在拆除炸弹的\0本地玩家\0武器\0C4炸弹\0已经安放的C4\0小鸡\0拆弹装备\0投掷物\0人质\0尸体\0");
                            ImGui::PopID();
                            static int currentItem{ 0 };
                            if (currentCategory <= 3) {
                                ImGui::SameLine();
                                static int currentType{ 0 };
                                ImGui::PushID(1);
                                ImGui::Combo("", &currentType, "所有\0可见的\0不可见的\0");
                                ImGui::PopID();
                                currentItem = currentCategory * 3 + currentType;
                            }
                            else {
                                currentItem = currentCategory + 8;
                            }
                            ImGui::SameLine();
                            ImGui::Checkbox("启用", &config->glow[currentItem].enabled);
                            ImGui::Separator();
                            ImGui::Checkbox("随血量变色", &config->glow[currentItem].healthBased);
                            ImGuiCustom::colorPicker("颜色", config->glow[currentItem].color.color, nullptr, &config->glow[currentItem].color.rainbow, &config->glow[currentItem].color.rainbowSpeed);
                            ImGui::PushItemWidth(220.0f);
                            ImGui::SliderFloat("边缘厚度", &config->glow[currentItem].thickness, 0.0f, 1.0f, "%.2f");
                            ImGui::SliderFloat("发光厚度", &config->glow[currentItem].alpha, 0.0f, 1.0f, "%.2f");
                            ImGui::SliderInt("发光风格", &config->glow[currentItem].style, 0, 3);
                            ImGui::EndChild();
                        }


                        ImGui::Separator();

                        ImGui::BeginChild("Chams", { 400.0f,150.0f });
                        {
                            static int currentCategorys{ 0 };
                            ImGui::PushItemWidth(110.0f);
                            ImGui::Text("上色透视");
                            ImGui::SameLine();
                            ImGui::PushID(0);
                            ImGui::Combo("", &currentCategorys, "友军\0敌人\0安包\0拆弹\0本地\0武器\0手部\0回溯\0袖子\0");
                            ImGui::PopID();
                            static int currentItems{ 0 };

                            if (currentCategorys <= 3) {
                                ImGui::SameLine();
                                static int currentTypes{ 0 };
                                ImGui::PushID(1);
                                ImGui::Combo("", &currentTypes, "所有\0可见的\0不可见的\0");
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

                            ImGui::Checkbox("启用", &chams.enabled);
                            ImGui::Separator();
                            ImGui::Checkbox("随血量变色", &chams.healthBased);
                            ImGui::Checkbox("闪烁 ", &chams.blinking);
                            ImGui::Combo("材质", &chams.material, "普通\0纯色\0动态\0铂金\0Glass\0Chrome\0Crystal\0Silver\0Gold\0Plastic\0Glow\0");
                            ImGui::Checkbox("线条", &chams.wireframe);
                            ImGuiCustom::colorPicker("颜色", chams.color.color, nullptr, &chams.color.rainbow, &chams.color.rainbowSpeed);
                            ImGui::SetNextItemWidth(220.0f);
                            ImGui::SliderFloat("厚度", &chams.alpha, 0.0f, 1.0f, "%.2f");
                            ImGui::EndChild();
                        }

                        ImGui::Separator();

                        ImGui::BeginChild("Esp", { 500.0f,500.0f });
                        {
                            static int currentCategory = 0;
                            static int currentItem = 0;

                            if (ImGui::ListBoxHeader("##", { 125.0f, 475.0f })) {
                                static constexpr const char* players[]{ "所有", "可见的", "不可见的" };

                                ImGui::Text("友军");
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
                                ImGui::Text("敌人");
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

                                ImGui::Text("投掷物");
                                ImGui::Indent();
                                ImGui::PushID("Projectiles");
                                ImGui::PushFont(fonts.segoeui);
                                static constexpr const char* projectiles[]{ "闪光弹", "手榴弹", "遥控炸弹", "弹射炸弹", "诱饵手雷", "燃烧瓶", "战术探测手雷", "烟雾弹", "雪球" };

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

                                ImGui::Text("头号特训");
                                ImGui::Indent();
                                ImGui::PushID("Danger Zone");
                                ImGui::PushFont(fonts.segoeui);
                                static constexpr const char* dangerZone[]{ "哨戒机枪", "无人机", "现金", "现金袋子", "手枪武器箱", "轻武器箱", "重武器箱", "易爆炸油箱", "工具箱", "全甲", "半甲", "头盔", "降落伞", "高分目标", "平板升级卡", "Exo跳跃", "弹药箱", "雷达干扰器" };

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
                                    ImGui::Checkbox("启用", &config->esp.players[selected].enabled);
                                    ImGui::SameLine();
                                    ImGui::SetNextItemWidth(85.0f);
                                    ImGui::InputInt("字体", &config->esp.players[selected].font, 1, 294);
                                    config->esp.players[selected].font = std::clamp(config->esp.players[selected].font, 1, 294);

                                    ImGui::Separator();

                                    constexpr auto spacing{ 200.0f };
                                    ImGuiCustom::colorPicker("线条", config->esp.players[selected].snaplines);
                                    ImGui::SameLine();
                                    ImGuiCustom::colorPicker("方框", config->esp.players[selected].box);
                                    ImGui::SameLine();
                                    ImGui::SetNextItemWidth(95.0f);
                                    ImGui::Combo("", &config->esp.players[selected].boxType, "2D\0""2D corners\0""3D\0""3D corners\0");
                                    ImGuiCustom::colorPicker("视线", config->esp.players[selected].eyeTraces);
                                    ImGui::SameLine();
                                    ImGuiCustom::colorPicker("生命", config->esp.players[selected].health);
                                    ImGuiCustom::colorPicker("头部白点", config->esp.players[selected].headDot);
                                    ImGui::SameLine();
                                    ImGuiCustom::colorPicker("柱状血条", config->esp.players[selected].healthBar);
                                    // ImGui::Combo("##HP side", &config->esp.players[selected].hpside, "Left\0Bottom\0Right\0");
                                    ImGui::PushID("hotfix");
                                    ImGuiCustom::colorPicker("名字", config->esp.players[selected].name);
                                    ImGui::SameLine();
                                    ImGuiCustom::colorPicker("护甲", config->esp.players[selected].armor);
                                    ImGuiCustom::colorPicker("金钱", config->esp.players[selected].money);
                                    ImGui::SameLine();
                                    ImGuiCustom::colorPicker("甲条", config->esp.players[selected].armorBar);
                                    // ImGui::Combo("##AR side", &config->esp.players[selected].armorside, "Left\0Bottom\0Right\0");
                                    ImGuiCustom::colorPicker("轮廓", config->esp.players[selected].outline);
                                    ImGui::SameLine();
                                    ImGuiCustom::colorPicker("距离", config->esp.players[selected].distance);
                                    ImGuiCustom::colorPicker("武器", config->esp.players[selected].activeWeapon);
                                    ImGui::SameLine();
                                    ImGui::Checkbox("死亡方框", &config->esp.players[selected].deadesp);
                                    ImGui::SliderFloat("最大绘制距离", &config->esp.players[selected].maxDistance, 0.0f, 200.0f, "%.2fm");
                                    break;
                                }
                                case 2: {
                                    ImGui::Checkbox("启用", &config->esp.weapon.enabled);
                                    ImGui::SameLine();
                                    ImGui::SetNextItemWidth(85.0f);
                                    ImGui::InputInt("字体", &config->esp.weapon.font, 1, 294);
                                    config->esp.weapon.font = std::clamp(config->esp.weapon.font, 1, 294);

                                    ImGui::Separator();

                                    constexpr auto spacing{ 200.0f };
                                    ImGuiCustom::colorPicker("线条", config->esp.weapon.snaplines);
                                    ImGui::SameLine();
                                    ImGuiCustom::colorPicker("方框", config->esp.weapon.box);
                                    ImGui::SameLine();
                                    ImGui::SetNextItemWidth(95.0f);
                                    ImGui::Combo("", &config->esp.weapon.boxType, "2D\0""2D corners\0""3D\0""3D corners\0");
                                    ImGuiCustom::colorPicker("弹药", config->esp.weapon.ammo);
                                    ImGuiCustom::colorPicker("名字", config->esp.weapon.name);
                                    ImGui::SameLine();
                                    ImGuiCustom::colorPicker("轮廓", config->esp.weapon.outline);
                                    ImGuiCustom::colorPicker("距离", config->esp.weapon.distance);
                                    ImGui::SliderFloat("最大绘制距离", &config->esp.weapon.maxDistance, 0.0f, 200.0f, "%.2fm");
                                    break;
                                }
                                case 3: {
                                    ImGui::Checkbox("启用", &config->esp.projectiles[currentItem].enabled);
                                    ImGui::SameLine();
                                    ImGui::SetNextItemWidth(85.0f);
                                    ImGui::InputInt("字体", &config->esp.projectiles[currentItem].font, 1, 294);
                                    config->esp.projectiles[currentItem].font = std::clamp(config->esp.projectiles[currentItem].font, 1, 294);

                                    ImGui::Separator();

                                    constexpr auto spacing{ 200.0f };
                                    ImGuiCustom::colorPicker("线条", config->esp.projectiles[currentItem].snaplines);
                                    ImGui::SameLine();
                                    ImGuiCustom::colorPicker("方框", config->esp.projectiles[currentItem].box);
                                    ImGui::SameLine();
                                    ImGui::SetNextItemWidth(95.0f);
                                    ImGui::Combo("", &config->esp.projectiles[currentItem].boxType, "2D\0""2D corners\0""3D\0""3D corners\0");
                                    ImGuiCustom::colorPicker("名字", config->esp.projectiles[currentItem].name);
                                    ImGui::SameLine();
                                    ImGuiCustom::colorPicker("轮廓", config->esp.projectiles[currentItem].outline);
                                    ImGuiCustom::colorPicker("距离", config->esp.projectiles[currentItem].distance);
                                    ImGui::SliderFloat("最大绘制距离", &config->esp.projectiles[currentItem].maxDistance, 0.0f, 200.0f, "%.2fm");
                                    break;
                                }
                                case 4: {
                                    {
                                        int selected = currentItem;
                                        ImGui::Checkbox("启用", &config->esp.dangerZone[selected].enabled);
                                        ImGui::SameLine();
                                        ImGui::SetNextItemWidth(85.0f);
                                        ImGui::InputInt("字体", &config->esp.dangerZone[selected].font, 1, 294);
                                        config->esp.dangerZone[selected].font = std::clamp(config->esp.dangerZone[selected].font, 1, 294);

                                        ImGui::Separator();

                                        constexpr auto spacing{ 200.0f };
                                        ImGuiCustom::colorPicker("线条", config->esp.dangerZone[selected].snaplines);
                                        ImGui::SameLine();
                                        ImGuiCustom::colorPicker("方框", config->esp.dangerZone[selected].box);
                                        ImGui::SameLine();
                                        ImGui::SetNextItemWidth(95.0f);
                                        ImGui::Combo("", &config->esp.dangerZone[selected].boxType, "2D\0""2D corners\0""3D\0""3D corners\0");
                                        ImGuiCustom::colorPicker("名字", config->esp.dangerZone[selected].name);
                                        ImGui::SameLine();
                                        ImGuiCustom::colorPicker("轮廓", config->esp.dangerZone[selected].outline);
                                        ImGuiCustom::colorPicker("距离", config->esp.dangerZone[selected].distance);
                                        ImGui::SliderFloat("最大绘制距离", &config->esp.dangerZone[selected].maxDistance, 0.0f, 200.0f, "%.2fm");
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

                            ImGui::Combo("天空模板", &config->visuals.skybox, "默认\0cs_baggage_skybox_\0cs_tibet\0embassy\0italy\0jungle\0nukeblank\0office\0sky_cs15_daylight01_hdr\0sky_cs15_daylight02_hdr\0sky_cs15_daylight03_hdr\0sky_cs15_daylight04_hdr\0sky_csgo_cloudy01\0sky_csgo_night_flat\0sky_csgo_night02\0sky_day02_05_hdr\0sky_day02_05\0sky_dust\0sky_l4d_rural02_ldr\0sky_venice\0vertigo_hdr\0vertigo\0vertigoblue_hdr\0vietnam\0");
                           /* if (!config->visuals.fullBright) {
                                ImGui::PushID(5);
                                ImGui::SliderFloat("", &config->visuals.brightness, 0.0f, 1.0f, "亮度: %.2f");
                                ImGui::PopID();
                            }
                            else

                            {
                                ImGui::PushID(6);
                                ImGui::SliderFloat("", &config->visuals.brightness, 0.0f, 0.0f, "禁用");
                                ImGui::PopID();
                            };*/
                            ImGuiCustom::colorPicker("世界颜色", config->visuals.world);
                            ImGuiCustom::colorPicker("天空颜色", config->visuals.sky);
                            ImGui::Checkbox("禁用后期处理", &config->visuals.disablePostProcessing);
                            //ImGui::Checkbox("FullBright", &config->visuals.fullBright);
                            ImGui::Checkbox("击杀飞天", &config->visuals.inverseRagdollGravity);
                            ImGui::Checkbox("移除雾气", &config->visuals.noFog);
                            ImGui::Checkbox("移除3D天空", &config->visuals.no3dSky);
                            ImGui::Checkbox("移除烟雾弹", &config->visuals.noSmoke);
                            ImGui::Checkbox("移除模糊", &config->visuals.noBlur);
                            ImGui::Checkbox("移除开镜黑边", &config->visuals.noScopeOverlay);
                            ImGui::Checkbox("移除草地", &config->visuals.noGrass);
                            ImGui::Checkbox("移除阴影", &config->visuals.noShadows);
                            ImGui::Checkbox("线化烟雾", &config->visuals.wireframeSmoke);



                        }
                        ImGui::NextColumn();



                        ImGui::Checkbox("移除枪械后坐力", &config->visuals.noAimPunch);
                        ImGui::Checkbox("移除视觉后坐力", &config->visuals.noViewPunch);
                        ImGui::Checkbox("移除手臂", &config->visuals.noHands);
                        ImGui::Checkbox("移除袖子", &config->visuals.noSleeves);
                        ImGui::Checkbox("移除武器", &config->visuals.noWeapons);
                        ImGui::Combo("屏幕效果", &config->visuals.screenEffect, "无\0Drone cam\0Drone cam with noise\0Underwater\0Healthboost\0Dangerzone\0");

                        ImGui::Combo("击中效果", &config->visuals.hitEffect, "无\0Drone cam\0Drone cam with noise\0Underwater\0Healthboost\0头号特训\0");
                        ImGui::SliderFloat("持续时间", &config->visuals.hitEffectTime, 0.1f, 1.5f, "%.2fs");
                        ImGui::Combo("击中效果1", &config->visuals.hitMarker, "无\0默认(穿越)\0");
                        ImGui::SliderFloat("持续时间2", &config->visuals.hitMarkerTime, 0.1f, 1.5f, "%.2fs");
                        ImGui::Checkbox("颜色矫正", &config->visuals.colorCorrection.enabled);
                        ImGui::SameLine();
                        bool ccPopup = ImGui::Button("编辑");

                        if (ccPopup)
                            ImGui::OpenPopup("##popup");

                        if (ImGui::BeginPopup("##popup")) {
                            ImGui::VSliderFloat("##1", { 40.0f, 160.0f }, &config->visuals.colorCorrection.blue, 0.0f, 1.0f, "蓝色\n%.3f"); ImGui::SameLine();
                            ImGui::VSliderFloat("##2", { 40.0f, 160.0f }, &config->visuals.colorCorrection.red, 0.0f, 1.0f, "红色\n%.3f"); ImGui::SameLine();
                            ImGui::VSliderFloat("##3", { 40.0f, 160.0f }, &config->visuals.colorCorrection.mono, 0.0f, 1.0f, "Mono\n%.3f"); ImGui::SameLine();
                            ImGui::VSliderFloat("##4", { 40.0f, 160.0f }, &config->visuals.colorCorrection.saturation, 0.0f, 1.0f, "Sat\n%.3f"); ImGui::SameLine();
                            ImGui::VSliderFloat("##5", { 40.0f, 160.0f }, &config->visuals.colorCorrection.ghost, 0.0f, 1.0f, "Ghost\n%.3f"); ImGui::SameLine();
                            ImGui::VSliderFloat("##6", { 40.0f, 160.0f }, &config->visuals.colorCorrection.green, 0.0f, 1.0f, "绿色\n%.3f"); ImGui::SameLine();
                            ImGui::VSliderFloat("##7", { 40.0f, 160.0f }, &config->visuals.colorCorrection.yellow, 0.0f, 1.0f, "黄色\n%.3f"); ImGui::SameLine();
                            ImGui::EndPopup();

                        }
                        ImGui::Checkbox("视野", &config->visuals.zoom);
                        ImGui::SameLine();
                        hotkey(config->visuals.zoomKey);
                        ImGui::Checkbox("第三人称", &config->visuals.thirdperson);
                        ImGui::SameLine();
                        hotkey(config->visuals.thirdpersonKey);
                        ImGui::SetNextItemWidth(140.0f);
                        /*ImGui::Combo("Thirdperson Angles", &config->antiAim.thirdpersonMode, "假身\0假身\0Current Tick\0");

                        if (config->visuals.thirdperson)
                        {
                            ImGui::Checkbox("死亡后启用", &config->visuals.deadThirdperson);
                        }*/


                        ImGui::PushItemWidth(290.0f);
                        ImGui::PushID(0);
                        ImGui::SliderInt("", &config->visuals.thirdpersonDistance, 0, 1000, "第三人称距离: %d");
                        ImGui::PopID();
                        //ImGui::PushID(1);
                        //ImGui::SliderInt("", &config->visuals.viewmodelFov, -60, 150, "手臂长度: %d");
                        //ImGui::PopID();
                        ImGui::PushID(2);
                        ImGui::SliderInt("", &config->visuals.fov, -60, 60, "视野大小: %d");
                        ImGui::PopID();
                        ImGui::PushID(3);
                        ImGui::SliderInt("", &config->visuals.farZ, 0, 2000, "渲染距离: %d");
                        ImGui::PopID();
                        ImGui::PushID(4);
                        ImGui::SliderInt("", &config->visuals.flashReduction, 0, 100, "闪光弹去除程度: %d%%");
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
                            ImGui::Checkbox("开启", &selected_entry.enabled);
                            ImGui::Separator();
                            ImGui::Columns(2, nullptr, false);
                            ImGui::InputInt("皮肤种子", &selected_entry.seed);
                            ImGui::InputInt("暗金计数器", &selected_entry.stat_trak);
                            ImGui::SliderFloat("磨损度", &selected_entry.wear, FLT_MIN, 1.f, "%.10f", 5);

                            ImGui::Combo("皮肤选择", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text) {
                                *out_text = (itemIndex == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits)[idx].name.c_str();
                                return true;
                                }, nullptr, (itemIndex == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits).size(), 10);


                            ImGui::Combo("品质", &selected_entry.entity_quality_vector_index, [](void* data, int idx, const char** out_text) {
                                *out_text = game_data::quality_names[idx].name;
                                return true;
                                }, nullptr, IM_ARRAYSIZE(game_data::quality_names), 5);

                            if (itemIndex == 0) {
                                ImGui::Combo("匕首", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text) {
                                    *out_text = game_data::knife_names[idx].name;
                                    return true;
                                    }, nullptr, IM_ARRAYSIZE(game_data::knife_names), 5);
                            }
                            else if (itemIndex == 1) {
                                ImGui::Combo("手套", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text) {
                                    *out_text = game_data::glove_names[idx].name;
                                    return true;
                                    }, nullptr, IM_ARRAYSIZE(game_data::glove_names), 5);
                            }
                            else {
                                static auto unused_value = 0;
                                selected_entry.definition_override_vector_index = 0;
                                ImGui::Combo("不可用的", &unused_value, "仅用于刀和手套\0");
                            }

                            ImGui::InputText("名称标签", selected_entry.custom_name, 32);

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

                                ImGui::Combo("印花选择", &selected_sticker.kit_vector_index, [](void* data, int idx, const char** out_text) {
                                    *out_text = SkinChanger::stickerKits[idx].name.c_str();
                                    return true;
                                    }, nullptr, SkinChanger::stickerKits.size(), 10);

                                ImGui::SliderFloat("磨损度", &selected_sticker.wear, FLT_MIN, 1.0f, "%.10f", 5.0f);
                                ImGui::SliderFloat("大小比例", &selected_sticker.scale, 0.1f, 5.0f);
                                ImGui::SliderFloat("旋转角度", &selected_sticker.rotation, 0.0f, 360.0f);

                                ImGui::PopID();
                            }
                            selected_entry.update();

                            ImGui::Columns(1);

                            ImGui::Separator();



                            //皮肤搜索开始
                           /* {
                                已经删除 倒卖死妈

                            }*/
                            
                            
                            
                            ImGui::Combo("CT 玩家模型", &config->visuals.playerModelCT, "默认\0Ava特工 | 联邦调查局(FBI)\0特种兵 | 联邦调查局（FBI）特警\0Markus Delrow | 联邦调查局（FBI）人质营救队\0Michael Syfers | 联邦调查局（FBI）狙击手\0B Squadron指挥官 | 英国空军特别部队\0海豹部队第六分队士兵 | 海军水面战中心海豹部队\0铅弹 | 海军水面战中心海豹部队\0陆军少尉长官Ricksaw | 海军水面战中心海豹部队\0第三特种兵连 | 德国特种部队突击队\0'两次'McCoy | 美国空军战术空中管制部队\0Dragomir | Sabre\0准备就绪的Rezan | Sabre\0’医生‘Romanov | Sabre\0Maximus | Sabre\0Blackwolf | Sabre\0精英Muhlik先生 | 精锐分子\0地面叛军 | 精锐分子\0Osiris | 精锐分子\0Shahmat教授 | 精锐分子\0执行者 | 凤凰战士\0弹弓 | 凤凰战士\0枪手 | 凤凰战士\0");
                            ImGui::Combo("T 玩家模型", &config->visuals.playerModelT, "默认\0Ava特工 | 联邦调查局(FBI)\0特种兵 | 联邦调查局（FBI）特警\0Markus Delrow | 联邦调查局（FBI）人质营救队\0Michael Syfers | 联邦调查局（FBI）狙击手\0B Squadron指挥官 | 英国空军特别部队\0海豹部队第六分队士兵 | 海军水面战中心海豹部队\0铅弹 | 海军水面战中心海豹部队\0陆军少尉长官Ricksaw | 海军水面战中心海豹部队\0第三特种兵连 | 德国特种部队突击队\0'两次'McCoy | 美国空军战术空中管制部队\0Dragomir | Sabre\0准备就绪的Rezan | Sabre\0’医生‘Romanov | Sabre\0Maximus | Sabre\0Blackwolf | Sabre\0精英Muhlik先生 | 精锐分子\0地面叛军 | 精锐分子\0Osiris | 精锐分子\0Shahmat教授 | 精锐分子\0执行者 | 凤凰战士\0弹弓 | 凤凰战士\0枪手 | 凤凰战士\0");
                            /*ImGui::SliderFloat("##Custom Viewmodel X", &config->visuals.viewmodel_x, -100, 100, "自定义手臂长度X轴: %.2f");
                            ImGui::SliderFloat("##Custom Viewmodel Y", &config->visuals.viewmodel_y, -100, 100, "自定义手臂长度Y轴: %.2f");
                            ImGui::SliderFloat("##Custom Viewmodel Z", &config->visuals.viewmodel_z, -100, 100, "自定义手臂长度Z轴: %.2f");*/
                            ImGui::Checkbox("旋转沙鹰", &config->visuals.deagleSpinner);
                            if (ImGui::Button("刷新修改", { 130.0f, 30.0f }))
                                SkinChanger::scheduleHudUpdate();
                            //Visuals::customViewmodel();
                        }

                        break;
                    }

                    case 6: {
                        ImGui::Columns(2, nullptr, false);
                        ImGui::SetColumnOffset(1, 230.0f);
                        ImGui::Checkbox("防止系统挂机自动踢出", &config->misc.antiAfkKick);
                        ImGui::Checkbox("自动连跳", &config->misc.bunnyHop);
                        /*ImGui::SliderInt("连跳击中率", &config->misc.bhopHitchance, 0, 100, "%d%");
                        ImGui::SliderInt("最小击中率", &config->misc.bhopMinHits, 0, 20, "%d%");
                        ImGui::SliderInt("最大击中率", &config->misc.bhopMaxHits, 0, 20, "%d%");*/
                        ImGui::Checkbox("自动转向", &config->misc.autoStrafe);
                        ImGui::Checkbox("快速蹲下", &config->misc.fastDuck);
                        ImGui::Checkbox("滑步不演", &config->misc.moonwalk);
                        ImGui::Checkbox("地形边缘自动跳跃", &config->misc.edgejump);
                        ImGui::SameLine();
                        hotkey(config->misc.edgejumpkey);
                        ImGui::Checkbox("慢走", &config->misc.slowwalk);
                        ImGui::SameLine();
                        hotkey(config->misc.slowwalkKey);
                        ImGui::Checkbox("狙击准心", &config->misc.sniperCrosshair);
                        ImGui::Checkbox("后坐力准心", &config->misc.recoilCrosshair);
                        ImGui::Checkbox("自动手枪 ", &config->misc.autoPistol);
                        ImGui::Checkbox("自动上弹", &config->misc.autoReload);
                        ImGui::Checkbox("自动接受", &config->misc.autoAccept);
                        ImGui::Checkbox("雷达透视", &config->misc.radarHack);
                        ImGui::Checkbox("显示段位", &config->misc.revealRanks);
                        ImGui::Checkbox("显示金钱", &config->misc.revealMoney);
                        ImGui::Checkbox("揭发嫌疑人", &config->misc.revealSuspect);
                        ImGuiCustom::colorPicker("观众名单 ", config->misc.spectatorList);
                        ImGuiCustom::colorPicker("水印", config->misc.watermark);
                        ImGui::Checkbox("修复自瞄精度", &config->misc.fixAnimationLOD);
                        ImGui::Checkbox("修复自瞄部位", &config->misc.fixBoneMatrix);
                        ImGui::Checkbox("修复物体运动", &config->misc.fixMovement);
                        ImGui::Checkbox("模型绘测", &config->misc.disableModelOcclusion);
                        ImGui::SliderFloat("纵横比", &config->misc.aspectratio, 0.0f, 5.0f, "%.2f ");
                        ImGui::NextColumn();
                        ImGui::Checkbox("界面UI透明化", &config->misc.disablePanoramablur);
                        /*ImGui::Combo("组名选择", &config->misc.clantagshadow, "ShadowWare\0自定义组名\0时钟组名\0");
                        switch (config->misc.clantagshadow) {
                        case 0: {
                            ImGui::Text("组名:ShadowWare");
                            break;
                        }
                        case 1: {
                            ImGui::Checkbox("动态组名", &config->misc.animatedClanTag);
                            ImGui::Checkbox("自定义组名", &config->misc.customClanTag);
                            ImGui::SameLine();
                            ImGui::PushItemWidth(120.0f);
                            ImGui::PushID(0);
                            if (ImGui::InputText("", &config->misc.clanTag))
                                Misc::updateClanTag(true);
                            ImGui::PopID();
                            break;
                        }
                        case 2: {
                            ImGui::Checkbox("时钟组名 ", &config->misc.clocktag);
                            break;
                        }
                        }*/

                        ImGui::PushItemWidth(120.0f);

                        ImGui::Checkbox("击杀嘲讽", &config->misc.killMessage);
                        ImGui::SameLine();
                        ImGui::PushItemWidth(120.0f);
                        ImGui::PushID(1);
                        ImGui::InputText("", &config->misc.killMessageString);
                        ImGui::PopID();
                        /*ImGui::Combo("Fake功能选项", &config->misc.nameChangeSelection, "关闭\0假封禁\0假交易(开箱)\0窃取(自定义)名字");
                        if (config->misc.nameChangeSelection == 1)
                        {
                            ImGui::PushID(3);
                            ImGui::SetNextItemWidth(100.0f);
                            ImGui::Combo("", &config->misc.banColor, "白色\0红色\0紫色\0绿色\0浅绿色\0青色\0浅白色\0灰色\0黄色\0灰色2\0浅蓝色\0紫灰色\0蓝色\0粉色\0暗橙色\0橙色\0");
                            ImGui::PopID();
                            ImGui::SameLine();
                            ImGui::PushID(4);
                            ImGui::InputText("", &config->misc.banText);
                            ImGui::PopID();
                            ImGui::SameLine();
                            if (ImGui::Button("假封禁"))
                                Misc::fakeBan(true);
                        }
                        else if (config->misc.nameChangeSelection == 2)
                        {
                            ImGui::SetNextItemWidth(200.0f);
                            ImGuiCustom::MultiCombo("Fake Item Flags", config->misc.fakeItemFlags, config->misc.selectedFakeItemFlags, 4);
                            ImGui::SetNextItemWidth(200.0f);
                            ImGui::Combo("阵营", &config->misc.fakeItemTeam, "警家\0匪家");
                            ImGui::SetNextItemWidth(200.0f);
                            ImGui::Combo("Fake信息选项", &config->misc.fakeItemMessageType, "开箱获得的\0交易获得的\0");
                            ImGui::SetNextItemWidth(200.0f);
                            ImGui::Combo("武器选择", &config->misc.fakeItemType, "AK-47\0AUG\0AWP\0Bayonet\0Bowie Knife\0Butterfly Knife\0CZ75-Auto\0Classic Knife\0Desert Eagle\0Dual Berettas\0FAMAS\0Falchion Knife\0Five-SeveN\0Flip Knife\0G3SG1\0Galil AR\0Glock-18\0Gut Knife\0Huntsman Knife\0Karambit\0M249\0M4A1-S\0M4A4\0M9 Bayonet\0MAC-10\0MAG-7\0MP5-SD\0MP7\0MP9\0Navaja Knife\0Negev\0Nomad Knife\0Nova\0P2000\0P250\0P90\0PP-Bizon\0Paracord Knife\0R8 Revolver\0SCAR-20\0SG 553\0SSG 08\0Sawed-Off\0Shadow Daggers\0Skeleton Knife\0Spectral Shiv\0Stiletto Knife\0Survival Knife\0Talon Knife\0Tec-9\0UMP-45\0USP-S\0Ursus Knife\0XM1014\0Hand Wraps\0Moto Gloves\0Specialist Gloves\0Sport Gloves\0Bloodhound Gloves\0Hydra Gloves\0Driver Gloves\0");
                            ImGui::SetNextItemWidth(200.0f);
                            ImGui::Combo("皮肤颜色(质量)", &config->misc.fakeItemRarity, "白色品质\0淡蓝色品质\0蓝色品质\0紫色品质\0粉色品质\0红色品质\0橙色|金色品质\0");
                            ImGui::Combo("玩家颜色", &config->misc.fakeItemPlayerColor, "黄色\0绿色\0蓝色\0紫色\0橙色");
                            ImGui::InputText("玩家名字", &config->misc.fakeItemPlayerName);
                            ImGui::InputText("皮肤名字", &config->misc.fakeItemName);
                            if (ImGui::Button("开启假皮肤"))
                                Misc::fakeItem(true);
                        }
                        else if (config->misc.nameChangeSelection == 3)
                        {
                            ImGui::Checkbox("名字窃取", &config->misc.nameStealer);
                            ImGui::InputText("自定义名字", &config->misc.customName);
                            if (ImGui::Button("改变昵称"))
                                Misc::setName(true);
                        }*/

                        ImGui::Checkbox("快速下包", &config->misc.fastPlant);
                        //ImGui::Checkbox("C4伤害器", &config->misc.bombDamage);
                        ImGuiCustom::colorPicker("C4计时器", config->misc.bombTimer);
                        ImGui::Checkbox("快速换弹", &config->misc.quickReload);
                        if (ImGui::Button("开启自定义击中音效"))
                            config->misc.hitSound = 5;

                        ImGui::Combo("击中音效", &config->misc.hitSound, "无\0金属\0SK\0铃铛声\0玻璃声音\0自定义\0");
                        if (config->misc.hitSound == 5) {
                            ImGui::InputText("击中音效文件名", &config->misc.customHitSound);
                            if (ImGui::IsItemHovered())
                                ImGui::SetTooltip("请在CSGO根目录csgo/sound/里面放入音效文件xxx.wav");
                        }
                        if (ImGui::Button("开启自定义击杀音效"))
                            config->misc.killSound = 5;

                        ImGui::PushID(5);
                        ImGui::Combo("击杀音效", &config->misc.killSound, "无\0金属\0SK\0铃铛声\0玻璃声音\0自定义\0");
                        if (config->misc.killSound == 5) {
                            ImGui::InputText("击杀音效文件名", &config->misc.customKillSound);
                            if (ImGui::IsItemHovered())
                                ImGui::SetTooltip("请在CSGO根目录csgo/sound/里面放入音效文件xxx.wav");
                        }
                        ImGui::PopID();
                        ImGui::SetNextItemWidth(90.0f);

                        ImGui::Text("快速打针");
                        ImGui::SameLine();
                        hotkey(config->misc.quickHealthshotKey);
                        ImGui::Checkbox("投掷物预测线", &config->misc.nadePredict);
                        ImGui::Checkbox("修复头号特训信号", &config->misc.fixTabletSignal);
                        ImGui::SetNextItemWidth(120.0f);
                        ImGui::SliderFloat("最大视角变化", &config->misc.maxAngleDelta, 0.0f, 255.0f, "%.2f");
                        ImGui::Checkbox("强开优先", &config->misc.fakePrime);
                        ImGui::Text("——————游戏声音——————");
                        static int currentCategory{ 0 };
                        ImGui::PushItemWidth(110.0f);
                        ImGui::Combo("", &currentCategory, "本人\0友军\0敌人\0");
                        ImGui::PopItemWidth();
                        ImGui::SliderInt("全局音效", &config->sound.players[currentCategory].masterVolume, 0, 200, "%d%%");
                        ImGui::SliderInt("爆头音效", &config->sound.players[currentCategory].headshotVolume, 0, 200, "%d%%");
                        ImGui::SliderInt("开枪声音", &config->sound.players[currentCategory].weaponVolume, 0, 200, "%d%%");
                        ImGui::SliderInt("脚步声音", &config->sound.players[currentCategory].footstepVolume, 0, 200, "%d%%");
                        ImGui::SliderInt("鸡叫声", &config->sound.chickenVolume, 0, 200, "%d%%");
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



                                if (ImGui::Button("刷新参数", { 160.0F, 25.0F }))
                                    //config->Config::Config("ShadowWare");// old  文件夹名字
                                    config->listConfigs(); //New 

                                if (ImGui::Button("创建参数", { 100.0f, 25.0f }))
                                    config->add(buffer.c_str());

                                if (ImGui::Button("重置参数", { 100.0f, 25.0f }))
                                    ImGui::OpenPopup("重置参数");

                                if (ImGui::BeginPopup("重置参数")) {
                                    static constexpr const char* names[]{ "确定重置" };
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
                                    if (ImGui::Button("加载参数", { 100.0f, 25.0f })) {
                                        config->load(currentConfig);
                                        SkinChanger::scheduleHudUpdate();
                                        Misc::updateClanTag(true);
                                    }

                                    if (ImGui::Button("保存参数", { 100.0f, 25.0f }))
                                        config->save(currentConfig);

                                    if (ImGui::Button("删除参数", { 100.0f, 25.0f }))
                                        config->remove(currentConfig);
                                }



                                ImGui::NextColumn();
                                ImGui::TextUnformatted("ShadowWare菜单热键");
                                ImGui::SameLine();
                                hotkey(config->misc.menuKey);

                                //ImGui::Checkbox("LUA编辑", &LUA_WINDOW);

                                //if (ImGui::Button("加载LUA", { 100.0f, 25.0f })) 
                                //c_lua::luaSettingLoad();
                                //if (ImGui::Button("保存LUA", { 100.0f, 25.0f }))


                                

                                if (ImGui::Button("退出Shadow Ware"))
                                    hooks->uninstall();



                                break;
                        }
                        //Config  & LUA

                    }
                    }
                }
                ImGui::EndChild();
            }
        }

    }



























