#include <fstream>
#include <functional>
#include <string>
#include <ShlObj.h>
#include <Windows.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_stdlib.h"
#include "imgui/imgui_impl_dx9.h"

#include "imguiCustom.h"

#include "GUI.h"
#include "Config.h"
#include "Hacks/Misc.h"
#include "Hacks/Reportbot.h"
#include "Hacks/SkinChanger.h"
#include "Hooks.h"
#include "Interfaces.h"
#include "SDK/InputSystem.h"
#include "SDK/SearchEngine.h"
#pragma comment(lib, "D3dx9") 

//Beta Branch


void CenterText() {
    std::string text = "1";
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - ImGui::CalcTextSize(text.c_str()).x
        - ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x / 2);

}

void CloseMenu() {

    gui->open = !gui->open;
    if (!gui->open) {
        // ImGui::GetIO().MouseDown[0] = false;
        interfaces->inputSystem->resetInputState();
    }

}

void SearchCheck(bool)noexcept;
constexpr auto windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar;



GUI::GUI() noexcept
{

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();

    style.FrameRounding = 10.0f;
    style.GrabRounding = 40.0f;

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    if (PWSTR pathToFonts; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Fonts, 0, nullptr, &pathToFonts))) {
        const std::filesystem::path path{ pathToFonts };
        CoTaskMemFree(pathToFonts);

        static constexpr ImWchar ranges[]{ 0x0020, 0xFFFF, 0 };
        ImFontConfig cfg;
        cfg.OversampleV = 3;

        fonts.tahoma = io.Fonts->AddFontFromFileTTF((path / "tahoma.ttf").string().c_str(), 15.0f, &cfg, ranges);
        fonts.segoeui = io.Fonts->AddFontFromFileTTF((path / "segoeui.ttf").string().c_str(), 15.0f, &cfg, ranges);
        fonts.arial = io.Fonts->AddFontFromFileTTF((path / "arial.ttf").string().c_str(), 15.0f, &cfg, ranges);
    }
}

void SearchCheck(bool changed)noexcept {
    switch (config->SkinSearch.Searchmode) {
    case 0:
        if (changed) {
            SkinChanger::search_result.clear();
            for (auto skin : SkinChanger::skinKits)
            {
                auto skin_copy = skin;

                char in_buffer[1024];
                strcpy_s<1024U>(in_buffer, skin_copy.name.c_str());

                char* out_buffer = new char[HZ2PY_OUTPUT_BUF_ARRAY_SIZE];

                memset(out_buffer, '\0', sizeof(char) * HZ2PY_OUTPUT_BUF_ARRAY_SIZE);

                if (is_utf8_string(in_buffer)) {
                    pinyin_utf8(in_buffer, out_buffer);
                }
                else {
                    pinyin_gb2312(in_buffer, out_buffer);
                }
                if (std::string p(out_buffer); p.find(SkinChanger::skin_name) != std::string::npos)
                {
                    skin_copy.name = skin_copy.name + " [ " + out_buffer + " ]";
                    SkinChanger::search_result.push_back(skin_copy);
                }
            }
        }
        break;

    case 1:
        if (changed) {
            SkinChanger::search_result_sticker.clear();
            for (auto skin : SkinChanger::stickerKits)
            {
                auto skin_copy = skin;

                char in_buffer[1024];
                strcpy_s<1024U>(in_buffer, skin_copy.name.c_str());

                char* out_buffer = new char[HZ2PY_OUTPUT_BUF_ARRAY_SIZE];

                memset(out_buffer, '\0', sizeof(char) * HZ2PY_OUTPUT_BUF_ARRAY_SIZE);

                if (is_utf8_string(in_buffer)) {
                    pinyin_utf8(in_buffer, out_buffer);
                }
                else {
                    pinyin_gb2312(in_buffer, out_buffer);
                }
                if (std::string p(out_buffer); p.find(SkinChanger::sticker_name) != std::string::npos)
                {
                    skin_copy.name = skin_copy.name + " [ " + out_buffer + " ]";
                    SkinChanger::search_result_sticker.push_back(skin_copy);
                }
            }
        }
        break;
    }
}

void GUI::render() noexcept
{
    
     //   renderGuiStyle2();
        renderPopupMenu();
        renderAimbotWindow();
        renderAntiAimWindow();
        renderTriggerbotWindow();
        renderBacktrackWindow();
        renderGlowWindow();
        renderChamsWindow();
        renderEspWindow();
        renderVisualsWindow();
        renderSkinChangerWindow();
        renderSoundWindow();
        renderStyleWindow();
        renderMiscWindow();
        renderReportbotWindow();
        renderConfigWindow();
    
}

void GUI::updateColors() const noexcept
{
    switch (config->style.menuColors) {
    case 0: ImGui::StyleColorsDark(); break;
    case 1: ImGui::StyleColorsLight(); break;
    case 2: ImGui::StyleColorsClassic(); break;
    }
}

void GUI::hotkey(int& key) noexcept
{
    key ? ImGui::Text("[ %s ]", interfaces->inputSystem->virtualKeyToString(key)) : ImGui::TextUnformatted("[ Tecla ]");

    if (!ImGui::IsItemHovered())
        return;

    ImGui::SetTooltip("Pressione uma tecla para definir");
    ImGuiIO& io = ImGui::GetIO();
    for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
        if (ImGui::IsKeyPressed(i) && i != config->misc.menuKey)
            key = i != VK_ESCAPE ? i : 0;

    for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
        if (ImGui::IsMouseDown(i) && i + (i > 1 ? 2 : 1) != config->misc.menuKey)
            key = i + (i > 1 ? 2 : 1);
}

void GUI::renderAimbotWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.aimbot)
            return;
        ImGui::SetNextWindowSize({ 260.0f, 0.0f });
        ImGui::Begin("Aimbot", &window.aimbot, windowFlags);
    }
    ImGui::TextUnformatted("Aimbot | BRCheats");
    ImGui::SameLine();
    CenterText();
    if (ImGui::Button("x")) {
        window.aimbot = !window.aimbot;
    }
    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    ImGui::Combo("", &currentCategory, "Todas\0Pistolas\0Pesadas\0SMG\0Rifles\0");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Selecione uma arma para configurar individualmente");
    }
    ImGui::PopID();
    ImGui::SameLine();
    static int currentWeapon{ 0 };
    ImGui::Checkbox("Ativo?", &config->aimbot[currentWeapon].enabled);

    ImGui::PushID(1);

    switch (currentCategory) {
    case 0:
        currentWeapon = 0;
        ImGui::NewLine();
        break;
    case 1: {
        static int currentPistol{ 0 };
        static constexpr const char* pistols[]{ "Todas", "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-Seven", "CZ-75", "Desert Eagle", "Revolver" };

        ImGui::Combo("", &currentPistol, [](void* data, int idx, const char** out_text) {
            if (config->aimbot[idx ? idx : 35].enabled) {
                static std::string name;
                name = pistols[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = pistols[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(pistols));

        currentWeapon = currentPistol ? currentPistol : 35;
        break;
    }
    case 2: {
        static int currentHeavy{ 0 };
        static constexpr const char* heavies[]{ "Todas", "Nova", "XM1014", "Cano-Curto", "MAG-7", "M249", "Negev" };

        ImGui::Combo("", &currentHeavy, [](void* data, int idx, const char** out_text) {
            if (config->aimbot[idx ? idx + 10 : 36].enabled) {
                static std::string name;
                name = heavies[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = heavies[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(heavies));

        currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
        break;
    }
    case 3: {
        static int currentSmg{ 0 };
        static constexpr const char* smgs[]{ "Todas", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon" };

        ImGui::Combo("", &currentSmg, [](void* data, int idx, const char** out_text) {
            if (config->aimbot[idx ? idx + 16 : 37].enabled) {
                static std::string name;
                name = smgs[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = smgs[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(smgs));

        currentWeapon = currentSmg ? currentSmg + 16 : 37;
        break;
    }
    case 4: {
        static int currentRifle{ 0 };
        static constexpr const char* rifles[]{ "Todas", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };

        ImGui::Combo("", &currentRifle, [](void* data, int idx, const char** out_text) {
            if (config->aimbot[idx ? idx + 23 : 38].enabled) {
                static std::string name;
                name = rifles[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = rifles[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(rifles));

        currentWeapon = currentRifle ? currentRifle + 23 : 38;
        break;
    }
    }
    ImGui::PopID();

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Ativa o Aimbot e libera configurações");
    }
    
        ImGui::Separator();
        ImGui::Checkbox("Tecla", &config->aimbot[currentWeapon].onKey);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Ativa o Aimbot somente se a tecla for pressionada");
        }
        ImGui::SameLine();
        hotkey(config->aimbot[currentWeapon].key);
        ImGui::SameLine();
        ImGui::PushID(2);
        ImGui::PushItemWidth(70.0f);
        ImGui::Combo("", &config->aimbot[currentWeapon].keyMode, "Segurar\0Ativar\0");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Segurar = Segure para ativar | Ativar = Aperte para ativar, aperte de novo para desativar");
        }
        ImGui::PopItemWidth();
        ImGui::PopID();
        ImGui::Checkbox("Sem puxar", &config->aimbot[currentWeapon].silent);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("A mira não será puxada");
        }
        ImGui::Checkbox("Fogo Amigo", &config->aimbot[currentWeapon].friendlyFire);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Ativar Aimbot em companheiros de equipe");
        }
        ImGui::Checkbox("Somente Visível", &config->aimbot[currentWeapon].visibleOnly);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Aimbot funcionará somente em inimigos visíveis");
        }
        ImGui::Checkbox("Somente Mirado", &config->aimbot[currentWeapon].scopedOnly);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Aimbot funcionará somente com scope aberto (em armas com mira)");
        }
        ImGui::Checkbox("Ignorar Flash", &config->aimbot[currentWeapon].ignoreFlash);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Aimbot funcionará mesmo cego");
        }
        ImGui::Checkbox("Ignorar Smoke", &config->aimbot[currentWeapon].ignoreSmoke);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Aimbot funcionará mesmo através de smokes");
        }
        ImGui::Checkbox("Auto Atirar", &config->aimbot[currentWeapon].autoShot);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Atirar automaticamente");
        }
        ImGui::Checkbox("Auto Mirar", &config->aimbot[currentWeapon].autoScope);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Abrir mira automaticamente (em armas com mira)");
        }
        ImGui::Checkbox("Killshots", &config->aimbot[currentWeapon].killshot);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Aimbot funcionará no último disparo");
        }
        ImGui::Checkbox("Entre disparos", &config->aimbot[currentWeapon].betweenShots);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Aimbot irá corrigir a mira entre disparos");
        }
        ImGui::Checkbox("Controle de Spray", &config->aimbot[currentWeapon].recoilbasedFov);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Controlar Spray automaticamente");
        }
        float res = 0.0;
        if (config->aimbot[currentWeapon].recoilbasedFov) {
            ImGui::PushItemWidth(150.0f);
            ImGui::SliderFloat("Horizontal", &config->aimbot[currentWeapon].recoilControlX, 0.0f, 1.0f, "%.2f");
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Nível de controle horizontal");
            }
            ImGui::PushItemWidth(150.0f);
            ImGui::SliderFloat("Vertical", &config->aimbot[currentWeapon].recoilControlY, 0.0f, 1.0f, "%.2f");
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Nível de controle vertical");
            }
            config->aimbot[currentWeapon].shotsFired = std::clamp(config->aimbot[currentWeapon].shotsFired, 0, 10);
            ImGui::Separator();
        }
        if (config->aimbot[currentWeapon].recoilbasedFov == false) {
            config->aimbot[currentWeapon].recoilControlX = res;
            config->aimbot[currentWeapon].recoilControlY = res;
        }
        ImGui::PushItemWidth(150.0f);
        ImGui::Combo("Local", &config->aimbot[currentWeapon].bone, "Proximo\0Mais Dano\0Cabeca\0Pescoco\0Peito\0Corpo\0Estomago\0Pelvis\0");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Local onde o aimbot funcionará");
        }
        ImGui::PushItemWidth(100.0f);
        ImGui::SliderFloat("Fov", &config->aimbot[currentWeapon].fov, 0.0f, 255.0f, "%.2f", 2.5f); 
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Área de atuação do Aimbot");
        }
        ImGui::SameLine();
        ImGui::Checkbox("Visível", &config->misc.drawAimbotFov);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Exibir área de atuação do Aimbot");
        }
        ImGui::PushItemWidth(150.0f);
        ImGui::SliderFloat("Suavidade", &config->aimbot[currentWeapon].smooth, 1.0f, 100.0f, "%.2f");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Suavidade com que a mira será puxada");
        }
        ImGui::PushItemWidth(150.0f);
        ImGui::SliderFloat("Imprecisão max.", &config->aimbot[currentWeapon].maxAimInaccuracy, 0.0f, 1.0f, "%.5f", 2.0f);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Imprecisão máxima para que o Aimbot seja funcione");
        }
        ImGui::PushItemWidth(100.0f);
        ImGui::InputInt("Dano Minimo", &config->aimbot[currentWeapon].minDamage);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Dano mínimo que o aimbot buscará");
        }
        config->aimbot[currentWeapon].minDamage = std::clamp(config->aimbot[currentWeapon].minDamage, 0, 250);
        ImGui::Columns(1);
    
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderAntiAimWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.antiAim)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("Anti aim", &window.antiAim, windowFlags);
        ImGui::TextUnformatted("Anti Aim | BRCheats");
        ImGui::SameLine();
        CenterText();
        if (ImGui::Button("x")) {
            window.antiAim = !window.antiAim;
        }
    }
    ImGui::Checkbox("Ligado?", &config->antiAim.general.enabled);
    ImGui::Text("Invert Key");
    ImGui::SameLine();
    hotkey(config->antiAim.general.yawInverseAngleKey);
    ImGui::SameLine();
    ImGui::PushID(1);
    ImGui::SetNextItemWidth(75.0f);
    ImGui::Combo("", &config->antiAim.general.yawInverseKeyMode, "Segurar\0Ativar\0");
    ImGui::PopID();
    ImGui::Checkbox("Fakewalk", &config->antiAim.general.fakeWalk.enabled);
    if (config->antiAim.general.fakeWalk.enabled) {
        ImGui::SameLine();
        hotkey(config->antiAim.general.fakeWalk.key);
        ImGui::SameLine();
        ImGui::PushID(2);
        ImGui::SetNextItemWidth(75.0f);
        ImGui::Combo("", &config->antiAim.general.fakeWalk.keyMode, "Segurar\0Ativar\0");
        ImGui::PopID();
        ImGui::SetNextItemWidth(240.0f);
        ImGui::SliderInt("Velocidade", &config->antiAim.general.fakeWalk.maxChoke, 3, 15);
    }
    ImGui::Text("Standing");
    ImGui::Checkbox("Standing Enabled", &config->antiAim.standing.enabled);
    if (config->antiAim.standing.enabled)
    {
        ImGui::Checkbox("Standing Pitch", &config->antiAim.standing.pitch.enabled);
        if (config->antiAim.standing.pitch.enabled) {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(240.0f);
            ImGui::SliderFloat("Standing Pitch Angle", &config->antiAim.standing.pitch.angle, -89.0f, 89.0f, "%.2f°", 1);
        }
        ImGui::Checkbox("Standing Yaw", &config->antiAim.standing.yaw.enabled);
        if (config->antiAim.standing.yaw.enabled) {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(240.0f);
            ImGui::SliderFloat("Standing Yaw Angle", &config->antiAim.standing.yaw.angle, -180.0f, 180.0f, "%.2f°", 1);
            ImGui::SetNextItemWidth(85.0f);
            ImGui::Combo("Standing Yaw Mode", &config->antiAim.standing.yaw.fake.mode, "Static\0Jitter\0");
            if (config->antiAim.standing.yaw.fake.mode == 1)
            {
                ImGui::SetNextItemWidth(240.0f);
                ImGui::SliderFloat("Standing Step", &config->antiAim.standing.yaw.fake.step, 0.0f, 180.0f, "%.2f°", 1);
                ImGui::SetNextItemWidth(240.0f);
                ImGui::SliderFloat("Standing Jitter Max", &config->antiAim.standing.yaw.fake.jitterMax, -180.0f, 180.0f, "%.2f°", 1);
                ImGui::SetNextItemWidth(240.0f);
                ImGui::SliderFloat("Standing Jitter Min", &config->antiAim.standing.yaw.fake.jitterMin, -180.0f, 180.0f, "%.2f°", 1);
            }
        }
        ImGui::Checkbox("Standing Yaw Desync", &config->antiAim.standing.yaw.desync.enabled);
        if (config->antiAim.standing.yaw.desync.enabled == true)
        {
            ImGui::SetNextItemWidth(85.0f);
            ImGui::Combo("Standing Desync Mode", &config->antiAim.standing.yaw.desync.mode, "Estático\0Jitter\0");
            if (!config->antiAim.standing.yaw.desync.mode)
            {
                ImGui::SetNextItemWidth(240.0f);
                ImGui::SliderFloat("Standing Body Lean", &config->antiAim.standing.yaw.desync.bodyLean, -100.0f, 100.0f, "%.2f", 1);
            }
            else if (config->antiAim.standing.yaw.desync.mode == 1)
            {
                ImGui::SetNextItemWidth(240.0f);
                ImGui::SliderFloat("Standing Desync Step", &config->antiAim.standing.yaw.desync.step, 0.0f, 100.0f, "%.2f°", 1);
                ImGui::SetNextItemWidth(240.0f);
                ImGui::SliderFloat("Standing Desync Jitter Max", &config->antiAim.standing.yaw.desync.jitterMax, -100.0f, 100.0f, "%.2f°", 1);
                ImGui::SetNextItemWidth(240.0f);
                ImGui::SliderFloat("Standing Desync Jitter Min", &config->antiAim.standing.yaw.desync.jitterMin, -100.0f, 100.0f, "%.2f°", 1);
            }
            ImGui::Checkbox("Standing LBY Breaker", &config->antiAim.standing.yaw.desync.LBYBreaker.enabled);
            if (config->antiAim.standing.yaw.desync.LBYBreaker.enabled)
            {
                ImGui::SliderFloat("Standing LBY Angle", &config->antiAim.standing.yaw.desync.LBYBreaker.angle, -180.0f, 180.0f, "%.2f°", 1);
            }
        }
    }
    ImGui::Text("Moving");
    ImGui::Checkbox("Moving Enabled", &config->antiAim.moving.enabled);
    if (config->antiAim.moving.enabled)
    {
        ImGui::Checkbox("Moving Pitch", &config->antiAim.moving.pitch.enabled);
        if (config->antiAim.moving.pitch.enabled) {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(240.0f);
            ImGui::SliderFloat("Moving Pitch Angle", &config->antiAim.moving.pitch.angle, -89.0f, 89.0f, "%.2f°", 1);
        }
        ImGui::Checkbox("Moving Yaw", &config->antiAim.moving.yaw.enabled);
        if (config->antiAim.moving.yaw.enabled) {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(240.0f);
            ImGui::SliderFloat("Moving Yaw Angle", &config->antiAim.moving.yaw.angle, -180.0f, 180.0f, "%.2f°", 1);
            ImGui::SetNextItemWidth(85.0f);
            ImGui::Combo("Moving Yaw Mode", &config->antiAim.moving.yaw.fake.mode, "Static\0Jitter\0");
            if (config->antiAim.moving.yaw.fake.mode == 1)
            {
                ImGui::SetNextItemWidth(240.0f);
                ImGui::SliderFloat("Moving Step", &config->antiAim.moving.yaw.fake.step, 0.0f, 180.0f, "%.2f°", 1);
                ImGui::SetNextItemWidth(240.0f);
                ImGui::SliderFloat("Moving Jitter Max", &config->antiAim.moving.yaw.fake.jitterMax, -180.0f, 180.0f, "%.2f°", 1);
                ImGui::SetNextItemWidth(240.0f);
                ImGui::SliderFloat("Moving Jitter Min", &config->antiAim.moving.yaw.fake.jitterMin, -180.0f, 180.0f, "%.2f°", 1);
            }
        }
        ImGui::Checkbox("Moving Yaw Desync", &config->antiAim.moving.yaw.desync.enabled);
        if (config->antiAim.moving.yaw.desync.enabled == true)
        {
            ImGui::SetNextItemWidth(85.0f);
            ImGui::Combo("Moving Desync Mode", &config->antiAim.moving.yaw.desync.mode, "Estático\0Jitter\0");
            if (!config->antiAim.moving.yaw.desync.mode)
            {
                ImGui::SetNextItemWidth(240.0f);
                ImGui::SliderFloat("Moving Body Lean", &config->antiAim.moving.yaw.desync.bodyLean, -100.0f, 100.0f, "%.2f", 1);
            }
            else if (config->antiAim.moving.yaw.desync.mode == 1)
            {
                ImGui::SetNextItemWidth(240.0f);
                ImGui::SliderFloat("Moving Desync Step", &config->antiAim.moving.yaw.desync.step, 0.0f, 100.0f, "%.2f°", 1);
                ImGui::SetNextItemWidth(240.0f);
                ImGui::SliderFloat("Moving Desync Jitter Max", &config->antiAim.moving.yaw.desync.jitterMax, -100.0f, 100.0f, "%.2f°", 1);
                ImGui::SetNextItemWidth(240.0f);
                ImGui::SliderFloat("Moving Desync Jitter Min", &config->antiAim.moving.yaw.desync.jitterMin, -100.0f, 100.0f, "%.2f°", 1);
            }
            ImGui::Checkbox("Moving LBY Breaker", &config->antiAim.moving.yaw.desync.LBYBreaker.enabled);
            if (config->antiAim.moving.yaw.desync.LBYBreaker.enabled)
            {
                ImGui::SliderFloat("Moving LBY Angle", &config->antiAim.moving.yaw.desync.LBYBreaker.angle, -180.0f, 180.0f, "%.2f°", 1);
            }
        }
    }
    ImGui::Text("In Air");
    ImGui::Checkbox("In Air Enabled", &config->antiAim.inAir.enabled);
    if (config->antiAim.inAir.enabled)
    {
        ImGui::Checkbox("In Air Pitch", &config->antiAim.inAir.pitch.enabled);
        if (config->antiAim.inAir.pitch.enabled) {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(240.0f);
            ImGui::SliderFloat("In Air Pitch Angle", &config->antiAim.inAir.pitch.angle, -89.0f, 89.0f, "%.2f°", 1);
        }
        ImGui::Checkbox("In Air Yaw", &config->antiAim.inAir.yaw.enabled);
        if (config->antiAim.inAir.yaw.enabled) {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(240.0f);
            ImGui::SliderFloat("In Air Yaw Angle", &config->antiAim.inAir.yaw.angle, -180.0f, 180.0f, "%.2f°", 1);
            ImGui::SetNextItemWidth(85.0f);
            ImGui::Combo("In Air Yaw Mode", &config->antiAim.inAir.yaw.fake.mode, "Estático\0Jitter\0");
            if (config->antiAim.inAir.yaw.fake.mode == 1)
            {
                ImGui::SetNextItemWidth(240.0f);
                ImGui::SliderFloat("In Air Step", &config->antiAim.inAir.yaw.fake.step, 0.0f, 180.0f, "%.2f°", 1);
                ImGui::SetNextItemWidth(240.0f);
                ImGui::SliderFloat("In Air Jitter Max", &config->antiAim.inAir.yaw.fake.jitterMax, -180.0f, 180.0f, "%.2f°", 1);
                ImGui::SetNextItemWidth(240.0f);
                ImGui::SliderFloat("In Air Jitter Min", &config->antiAim.inAir.yaw.fake.jitterMin, -180.0f, 180.0f, "%.2f°", 1);
            }
        }
        ImGui::Checkbox("In Air Yaw Desync", &config->antiAim.inAir.yaw.desync.enabled);
        if (config->antiAim.inAir.yaw.desync.enabled == true)
        {
            ImGui::SetNextItemWidth(85.0f);
            ImGui::Combo("In Air Anti-Aim Mode", &config->antiAim.inAir.yaw.desync.mode, "Estático\0Jitter\0");
            if (!config->antiAim.inAir.yaw.desync.mode)
            {
                ImGui::SetNextItemWidth(240.0f);
                ImGui::SliderFloat("In Air Body Lean", &config->antiAim.inAir.yaw.desync.bodyLean, -100.0f, 100.0f, "%.2f", 1);
            }
            else if (config->antiAim.inAir.yaw.desync.mode == 1)
            {
                ImGui::SetNextItemWidth(240.0f);
                ImGui::SliderFloat("In Desync Air Step", &config->antiAim.inAir.yaw.desync.step, 0.0f, 100.0f, "%.2f°", 1);
                ImGui::SetNextItemWidth(240.0f);
                ImGui::SliderFloat("In Air Desync Jitter Max", &config->antiAim.inAir.yaw.desync.jitterMax, -100.0f, 100.0f, "%.2f°", 1);
                ImGui::SetNextItemWidth(240.0f);
                ImGui::SliderFloat("In Air Desync Jitter Min", &config->antiAim.inAir.yaw.desync.jitterMin, -100.0f, 100.0f, "%.2f°", 1);
            }
            ImGui::Checkbox("In Air LBY Breaker", &config->antiAim.inAir.yaw.desync.LBYBreaker.enabled);
            if (config->antiAim.inAir.yaw.desync.LBYBreaker.enabled)
            {
                ImGui::SliderFloat("In Air LBY Angle", &config->antiAim.inAir.yaw.desync.LBYBreaker.angle, -180.0f, 180.0f, "%.2f°", 1);
            }
        }
    }

    
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderTriggerbotWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.triggerbot)
            return;
        ImGui::SetNextWindowSize({ 260.0f, 0.0f });
        ImGui::Begin("Triggerbot", &window.triggerbot, windowFlags);
    }
    ImGui::TextUnformatted("Autoshot | BRCheats");
    ImGui::SameLine();
    CenterText();
    if (ImGui::Button("x")) {
        window.triggerbot = !window.triggerbot;
    }
    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0); 
    ImGui::Combo("", &currentCategory, "Todas\0Pistolas\0Pesadas\0SMG\0Rifles\0Zeus x27\0");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Escolha uma arma para configurar individualmente");
    }
    ImGui::PopID();
    ImGui::SameLine();
    static int currentWeapon{ 0 };
    ImGui::Checkbox("Ligado?", &config->triggerbot[currentWeapon].enabled);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Ativa o Triggerbot e libera configurações");
    }


    ImGui::PushID(1);
    switch (currentCategory) {
    case 0:
        currentWeapon = 0;
        ImGui::NewLine();
        break;
    case 5:
        currentWeapon = 39;
        ImGui::NewLine();
        ImGui::Checkbox("Zeusbot", &config->misc.autoZeus);
        if(ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Atira com a Zeus automaticamente em 360°");
        }
        if (config->misc.autoZeus)
        {
            ImGui::SetNextItemWidth(120.0f);
            ImGui::SliderInt("Penetração Máxima da Zeus", &config->misc.autoZeusMaxPenDist, 0, 50);
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Penetração Máxima que a Zeus irá atacar");
            }
            config->misc.autoZeusMaxPenDist = std::clamp(config->misc.autoZeusMaxPenDist, 0, 50);
            ImGui::Checkbox("Zeusbot BAIM Only", &config->misc.autoZeusBaimOnly);
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("O Zeusbot só atirará se for matar o oponente");
            }
        }
        
        break;

    case 1: {
        static int currentPistol{ 0 };
        static constexpr const char* pistols[]{ "Todas", "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-Seven", "CZ-75", "Desert Eagle", "Revolver" };

        ImGui::Combo("", &currentPistol, [](void* data, int idx, const char** out_text) {
            if (config->triggerbot[idx ? idx : 35].enabled) {
                static std::string name;
                name = pistols[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = pistols[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(pistols));

        currentWeapon = currentPistol ? currentPistol : 35;
        break;
    }
    case 2: {
        static int currentHeavy{ 0 };
        static constexpr const char* heavies[]{ "Todas", "Nova", "XM1014", "Cano-Curto", "MAG-7", "M249", "Negev" };

        ImGui::Combo("", &currentHeavy, [](void* data, int idx, const char** out_text) {
            if (config->triggerbot[idx ? idx + 10 : 36].enabled) {
                static std::string name;
                name = heavies[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = heavies[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(heavies));

        currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
        break;
    }
    case 3: {
        static int currentSmg{ 0 };
        static constexpr const char* smgs[]{ "Todas", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon" };

        ImGui::Combo("", &currentSmg, [](void* data, int idx, const char** out_text) {
            if (config->triggerbot[idx ? idx + 16 : 37].enabled) {
                static std::string name;
                name = smgs[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = smgs[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(smgs));

        currentWeapon = currentSmg ? currentSmg + 16 : 37;
        break;
    }
    case 4: {
        static int currentRifle{ 0 };
        static constexpr const char* rifles[]{ "Todas", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };

        ImGui::Combo("", &currentRifle, [](void* data, int idx, const char** out_text) {
            if (config->triggerbot[idx ? idx + 23 : 38].enabled) {
                static std::string name;
                name = rifles[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = rifles[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(rifles));

        currentWeapon = currentRifle ? currentRifle + 23 : 38;
        break;
    }
    }
    ImGui::PopID();

    
        ImGui::Separator();
        ImGui::Checkbox("Tecla", &config->triggerbot[currentWeapon].onKey);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("O Triggerbot só funcionará enquanto a tecla for pressionada");
        }
        ImGui::SameLine();
        hotkey(config->triggerbot[currentWeapon].key);
        ImGui::SetNextItemWidth(85.0f);
        ImGui::Combo("Locais", &config->triggerbot[currentWeapon].hitgroup, "Todos\0Cabeca\0Peito\0Estomago\0Braco Esq.\0Braco Dir.\0Perna Esq.\0Perna Dir.\0");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("O Triggerbot só atirará nos lugares selecionados");
        }
        ImGui::Checkbox("Fogo Amigo", &config->triggerbot[currentWeapon].friendlyFire);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("O Triggerbot atacará companheiros");
        }
        ImGui::Checkbox("Somente mirado", &config->triggerbot[currentWeapon].scopedOnly);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("O Triggerbot só irá atirar se estiver mirado (em armas com mira)");
        }
        ImGui::Checkbox("Ignorar Flash", &config->triggerbot[currentWeapon].ignoreFlash);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("O Triggerbot atirará enquanto estiver cego");
        }
        ImGui::Checkbox("Ignorar Smoke", &config->triggerbot[currentWeapon].ignoreSmoke);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("O Triggerbot atirará através de smokes");
        }
        ImGui::Checkbox("Killshot", &config->triggerbot[currentWeapon].killshot);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("O Triggerbot dará tiros mortais");
        }

        ImGui::PushItemWidth(150.0f);
        ImGui::SliderInt("Delay", &config->triggerbot[currentWeapon].shotDelay, 0, 250, "%d ms");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Tempo que o Triggerbot demorará para disparar");
        }
        ImGui::InputInt("Dano minimo", &config->triggerbot[currentWeapon].minDamage);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Dano mínimo que o Triggerbot causará");
        }
        config->triggerbot[currentWeapon].minDamage = std::clamp(config->triggerbot[currentWeapon].minDamage, 0, 250);
        ImGui::PushItemWidth(150.0f);
        ImGui::SliderFloat("Tempo do tiro", &config->triggerbot[currentWeapon].burstTime, 0.0f, 0.5f, "%.3f s");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Tempo do disparo do tiro");
        }
        ImGui::PushItemWidth(150.0f);
        ImGui::SliderFloat("Imprecisao Max.", &config->triggerbot[currentWeapon].maxAimInaccuracy, 0.0f, 1.0f, "%.5f", 2.0f);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Imprecisão máxima da mira para que o Triggerbot seja funcional");
        }
    

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderBacktrackWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.backtrack)
            return;
        ImGui::SetNextWindowSize({ 260.0f, 0.0f });
        ImGui::Begin("Backtrack", &window.backtrack, windowFlags);
    }
    ImGui::TextUnformatted("Backtrack | BRCheats");
    ImGui::SameLine();
    CenterText();
    if (ImGui::Button("x")) {
        window.backtrack = !window.backtrack;
    }
    ImGui::Checkbox("Ligado?", &config->backtrack.enabled);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Ativa o Backtrack e libera configurações");
    }
    
    ImGui::Checkbox("Ignorar Smoke", &config->backtrack.ignoreSmoke);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("O Backtrack funcionará através de smokes");
    }
    ImGui::Checkbox("Baseado no FOV", &config->backtrack.recoilBasedFov);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("O Backtrack dependerá do FOV");
    }
    ImGui::PushItemWidth(150.0f);
    ImGui::SliderInt("Tempo", &config->backtrack.timeLimit, 1, 200, "%d ms");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Duração do Backtrack");
    }
    ImGui::PopItemWidth();
    
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderGlowWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.glow)
            return;
        ImGui::SetNextWindowSize({ 260.0f, 0.0f });
        ImGui::Begin("Glow", &window.glow, windowFlags);
    }
    ImGui::TextUnformatted("Contornos | BRCheats");
    ImGui::SameLine();
    CenterText();
    if (ImGui::Button("x")) {
        window.glow = !window.glow;
    }
    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    ImGui::Combo("", &currentCategory, "Aliados\0Inimigos\0Plantando\0Defusando\0Você\0Armas\0C4\0C4 Plantado\0Galinhas\0Defuse Kits\0Projeteis\0Reféns\0Cadáveres\0Ângulos Reais"); //Server Position\0Real Angles\0");
    ImGui::SameLine();
    static int currentItem{ 0 };
    ImGui::Checkbox("Ligado?", &config->glow[currentItem].enabled);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Escolha um ítem para configurar o glow");
    }
    ImGui::PopID();
   
    if (currentCategory <= 3) {
        static int currentType{ 0 };
        ImGui::PushID(1);
        ImGui::Combo("", &currentType, "Todos\0Visíveis\0Ocultos\0");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Escolha em qual momento será exibido");
        }
        ImGui::PopID();
        currentItem = currentCategory * 3 + currentType;
    } else {
        currentItem = currentCategory + 8;
    }

    
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Ativa o Glow");
    }
    ImGui::Separator();
    ImGui::Checkbox("Baseado na Vida", &config->glow[currentItem].healthBased);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("O Glow terá o tom da vida do oponente");
    }

    ImGuiCustom::colorPopup("Cor", config->glow[currentItem].color, &config->glow[currentItem].rainbow, &config->glow[currentItem].rainbowSpeed);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Selecione uma cor para o Glow");
    }

    ImGui::PushItemWidth(150.0f);
    ImGui::Combo("Estilo", &config->glow[currentItem].style, "Padrão\0 3D\0Bordas\0Bordas Pulsando\0");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Altera o Estilo do Glow");
    }
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderChamsWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.chams)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("Chams", &window.chams, windowFlags);
    }
    ImGui::TextUnformatted("Chams | BRCheats");
    ImGui::SameLine();
    CenterText();
    if (ImGui::Button("x")) {
        window.chams = !window.chams;
    }

    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    
    static int material = 1;

    if (ImGui::Combo("", &currentCategory, "Aliados\0Inimigos\0Plantando\0Defusando\0Você\0Armas\0Maos\0Backtrack\0Sleeves\0Desync\0"))
        material = 1;
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Escolha os alvos que o Chams exibirá");
    }
    static int currentItem{ 0 };
    auto& chams{ config->chams[currentCategory].materials[material - 1] };
    ImGui::SameLine();
    ImGui::Checkbox("Ligado?", &chams.enabled);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Ativa o Chams");
    }
    ImGui::PopID();

    ImGui::SameLine();

    if (material <= 1)
        ImGuiCustom::arrowButtonDisabled("##left", ImGuiDir_Left);
    else if (ImGui::ArrowButton("##left", ImGuiDir_Left))
        --material;
    ImGui::SameLine();
    ImGui::Text("%d", material);
    ImGui::SameLine();
    if (material >= int(config->chams[0].materials.size()))
        ImGuiCustom::arrowButtonDisabled("##right", ImGuiDir_Right);
    else if (ImGui::ArrowButton("##right", ImGuiDir_Right))
        ++material;


    ImGui::Separator();
    ImGui::Checkbox("Baseado na Vida", &chams.healthBased);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("A cor do chams será baseado na vida");
    }
    ImGui::Checkbox("Pulsante", &chams.blinking);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("O chams ficará pulsando");
    }
    ImGui::Combo("Material", &chams.material, "Normal\0Plano\0Animado\0Platina\0Vidro\0Cromado\0Cristal\0Prata\0Ouro\0Plastico\0Brilhante\0Perolado\0Metálico\0");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Escolha o material do chams");
    }
    ImGui::Checkbox("Wireframe", &chams.wireframe);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("O chams será exibido em Wireframe");
    }
    ImGui::Checkbox("Cover", &chams.cover);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("O chams será exibido em Cover");
    }
    ImGuiCustom::colorPopup("Cor", chams.color, &chams.rainbow, &chams.rainbowSpeed);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Escolha a cor do Chams");
    }

    if (!contentOnly) {
        ImGui::End();
    }
}

void GUI::renderEspWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.esp)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 270.0f });
        ImGui::Begin("Esp", &window.esp, windowFlags);
    }
    ImGui::TextUnformatted("ESP | BRCheats");
    ImGui::SameLine();
    CenterText();
    if (ImGui::Button("x")) {
        window.esp = !window.esp;
    }
    static int currentCategory = 0;
    static int currentItem = 0;

    if (ImGui::ListBoxHeader("##", { 125.0f, 230.0f })) {
        static constexpr const char* players[]{ "Todos", "Visivel", "Oculto" };

        ImGui::Text("Aliados");
        ImGui::Indent();
        ImGui::PushID("Aliados");
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
        ImGui::Text("Inimigos");
        ImGui::Indent();
        ImGui::PushID("Inimigos");
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
        if (bool isSelected = currentCategory == 2; ImGui::Selectable("Armas", isSelected))
            currentCategory = 2;

        ImGui::Text("Projeteis");
        ImGui::Indent();
        ImGui::PushID("Projeteis");
        ImGui::PushFont(fonts.segoeui);
        static constexpr const char* projectiles[]{ "Flashbang", "Granada HE", "Breach Charge", "Mina Terrestre", "Granada Decoy", "Molotov", "Granada TA", "Granada Smoke", "Bola de Neve" };

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
        static constexpr const char* dangerZone[]{ "Sentinelas", "Drones", "Dinheiro", "Maleta", "Pistolas em Caixa", "Case Leve", "Case Pesado", "Case Explosivo", "Case de Ferramentas", "Colete Full", "Colete", "Capacete", "Paraquedas", "Briefcase", "Melhorias", "ExoJump", "Munições", "Radar Jammer" };

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
    if (ImGui::BeginChild("##child", { 430.0f, 0.0f })) {
        switch (currentCategory) {
        case 0:
        case 1: {
            int selected = currentCategory * 3 + currentItem;
            ImGui::Checkbox("Ligado?", &config->esp.players[selected].enabled);
            ImGui::SameLine(0.0f, 50.0f);
            ImGui::SetNextItemWidth(85.0f);
            ImGui::InputInt("Fonte ", &config->esp.players[selected].font, 1, 294);
            config->esp.players[selected].font = std::clamp(config->esp.players[selected].font, 1, 294);

            ImGui::Separator();

            constexpr auto spacing{ 185.0f };
            ImGuiCustom::colorPicker("Linhas", config->esp.players[selected].snaplines);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Caixa", config->esp.players[selected].box);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(95.0f);
            ImGui::Combo("", &config->esp.players[selected].boxType, "2D\0""2D curvas\0""3D\0""3D curvas\0");
            ImGuiCustom::colorPicker("Mira Laser", config->esp.players[selected].eyeTraces);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Vida", config->esp.players[selected].health);
            ImGuiCustom::colorPicker("Ponto na Cabeca", config->esp.players[selected].headDot);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Barra de Vida", config->esp.players[selected].healthBar);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(80.f);
            ImGui::Combo("##Local", &config->esp.players[selected].hpside, "Esquerda\0Baixo \0Direita \0");
            ImGui::PushID("hotfix");
            ImGuiCustom::colorPicker("Nome", config->esp.players[selected].name);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Colete", config->esp.players[selected].armor);
            ImGuiCustom::colorPicker("Dinheiro", config->esp.players[selected].money);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Barra de Colete", config->esp.players[selected].armorBar);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(80.f);
            ImGui::PopID();
            ImGui::Combo("##Local", &config->esp.players[selected].armorside, "Esquerda\0Baixo\0Direita\0");
            ImGuiCustom::colorPicker("Linha externa", config->esp.players[selected].outline);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Distancia", config->esp.players[selected].distance);
            ImGuiCustom::colorPicker("Arma ativa", config->esp.players[selected].activeWeapon);
            ImGui::SameLine(spacing);
            ImGui::Checkbox("ESP Mortos", &config->esp.players[selected].deadesp);
            ImGui::SliderFloat("Distancia Max.", &config->esp.players[selected].maxDistance, 0.0f, 200.0f, "%.2fm");
            ImGui::Checkbox("Mostrar Multi Pontos", &config->esp.players[selected].drawMultiPoints);
            ImGui::Checkbox("Mostrar Multipontos da Cabeça", &config->esp.players[selected].drawMultiPointsOnlyHead);
            ImGui::TextUnformatted("Mostrar expansão dos multipontos");
            ImGui::PushID("multiPointsExpansion");
            ImGui::SliderFloat("", &config->esp.players[selected].drawMultiPointsExpansion, 0.01f, 2.0f, "%.2f", 5.0f);
            ImGui::PopID();
            break;
        }
        case 2: {
            ImGui::Checkbox("Ligado?", &config->esp.weapon.enabled);
            ImGui::SameLine(0.0f, 50.0f);
            ImGui::SetNextItemWidth(85.0f);
            ImGui::InputInt("Fonte", &config->esp.weapon.font, 1, 294);
            config->esp.weapon.font = std::clamp(config->esp.weapon.font, 1, 294);

            ImGui::Separator();

            constexpr auto spacing{ 200.0f };
            ImGuiCustom::colorPicker("Linhas", config->esp.weapon.snaplines);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Caixa", config->esp.weapon.box);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(95.0f);
            ImGui::Combo("", &config->esp.weapon.boxType, "2D\0""2D curvas\0""3D\0""3D curvas\0");
            ImGuiCustom::colorPicker("Nome", config->esp.weapon.name);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Ammo", config->esp.weapon.ammo);
            ImGuiCustom::colorPicker("Linha Externa", config->esp.weapon.outline);
            ImGuiCustom::colorPicker("Distancia", config->esp.weapon.distance);
            ImGui::SliderFloat("Distancia Max.", &config->esp.weapon.maxDistance, 0.0f, 200.0f, "%.2fm");
            break;
        }
        case 3: {
            ImGui::Checkbox("Ligado?", &config->esp.projectiles[currentItem].enabled);
            ImGui::SameLine(0.0f, 50.0f);
            ImGui::SetNextItemWidth(85.0f);
            ImGui::InputInt("Fonte", &config->esp.projectiles[currentItem].font, 1, 294);
            config->esp.projectiles[currentItem].font = std::clamp(config->esp.projectiles[currentItem].font, 1, 294);

            ImGui::Separator();

            constexpr auto spacing{ 200.0f };
            ImGuiCustom::colorPicker("Linhas", config->esp.projectiles[currentItem].snaplines);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Caixa", config->esp.projectiles[currentItem].box);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(95.0f);
            ImGui::Combo("", &config->esp.projectiles[currentItem].boxType, "2D\0""2D curvas\0""3D\0""3D curvas\0");
            ImGuiCustom::colorPicker("Nome", config->esp.projectiles[currentItem].name);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Linha Externa", config->esp.projectiles[currentItem].outline);
            ImGuiCustom::colorPicker("Distancia", config->esp.projectiles[currentItem].distance);
            ImGui::SliderFloat("Distancia Max.", &config->esp.projectiles[currentItem].maxDistance, 0.0f, 200.0f, "%.2fm");
            break;
        }
        case 4: {
            int selected = currentItem;
            ImGui::Checkbox("Ligado?", &config->esp.dangerZone[selected].enabled);
            ImGui::SameLine(0.0f, 50.0f);
            ImGui::SetNextItemWidth(85.0f);
            ImGui::InputInt("Fonte", &config->esp.dangerZone[selected].font, 1, 294);
            config->esp.dangerZone[selected].font = std::clamp(config->esp.dangerZone[selected].font, 1, 294);

            ImGui::Separator();

            constexpr auto spacing{ 200.0f };
            ImGuiCustom::colorPicker("Linhas", config->esp.dangerZone[selected].snaplines);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Caixa", config->esp.dangerZone[selected].box);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(95.0f);
            ImGui::Combo("", &config->esp.dangerZone[selected].boxType, "2D\0""2D curvas\0""3D\0""3D curvas\0");
            ImGuiCustom::colorPicker("Nome", config->esp.dangerZone[selected].name);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Linha Externa", config->esp.dangerZone[selected].outline);
            ImGuiCustom::colorPicker("Distancia", config->esp.dangerZone[selected].distance);
            ImGui::SliderFloat("Distancia Max", &config->esp.dangerZone[selected].maxDistance, 0.0f, 200.0f, "%.2fm");
            break;
        }
        }

        ImGui::EndChild();
    }

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderVisualsWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.visuals)
            return;
        ImGui::SetNextWindowSize({ 470.0f, 0.0f });
        ImGui::Begin("Visuals", &window.visuals, windowFlags);
    }
    ImGui::TextUnformatted("Visuais | BRCheats");
    ImGui::SameLine();
    CenterText();
    if (ImGui::Button("x")) {
        window.visuals = !window.visuals;
    }
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 220.0f);
    ImGui::Checkbox("Desativar Pos-processamento", &config->visuals.disablePostProcessing);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Desativa o Pós-Processamento do jogo (+ PERFORMANCE)");
    }
    ImGui::Checkbox("Inverter gravidade cadáveres", &config->visuals.inverseRagdollGravity);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Faz os cadáveres subirem ao céu");
    }
    ImGui::Checkbox("Sem névoa", &config->visuals.noFog);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Remove a névoa de alguns mapas (+ PERFORMANCE)");
    }
    ImGui::Checkbox("Sem Céu 3D", &config->visuals.no3dSky);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Remove o Céu 3D dos mapas (+ PERFORMANCE)");
    }
    ImGui::Checkbox("Sem Soco de Mira", &config->visuals.noAimPunch);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("A mira não pula ao dar um disparo");
    }
    ImGui::Checkbox("Sem Soco da Arma", &config->visuals.noViewPunch);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Remove o coice da arma ao dar um disparo");
    }
    ImGui::Checkbox("Sem mangas", &config->visuals.noHands);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Remove a manga de seu personagem");
    }
    ImGui::Checkbox("Sem bracos", &config->visuals.noSleeves);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Remove as mãos do personagem");
    }
    ImGui::Checkbox("Sem arma", &config->visuals.noWeapons);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Remove a arma de suas mãos");
    }
    ImGui::Checkbox("Sem Smoke", &config->visuals.noSmoke);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Remove as smokes (++ PERFORMANCE)");
    }
    ImGui::Checkbox("Sem Blur", &config->visuals.noBlur);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Remove o Blur do jogo (+ PERFORMANCE)");
    }
    ImGui::Checkbox("Sem Scope", &config->visuals.noScopeOverlay);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Remove o Scope das armas");
    }
    ImGui::Checkbox("Sem Grama", &config->visuals.noGrass);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Remove a grama dos mapas (++ PERFORMANCE)");
    }
    ImGui::Checkbox("Sem Sombra", &config->visuals.noShadows);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Remove a sombra dos mapas (++ PERFORMANCE)");
    }
    ImGui::Checkbox("Smoke Wireframe", &config->visuals.wireframeSmoke);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Torna as smokes em wireframe (+ PERFORMANCE)");
    }
    ImGuiCustom::colorPicker("Bullet Tracers", config->visuals.bulletTracers);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Exibe a trajetória dos disparos");
    }
    ImGui::Checkbox("Zoom", &config->visuals.zoom);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Dá um zoom ao pressionar a tecla especificada");
    }
    ImGui::SameLine();
    hotkey(config->visuals.zoomKey);
    ImGui::Checkbox("Terceira Pessoa", &config->visuals.thirdperson);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Deixa a câmera em terceira pessoa ao pressionar a tecla especificada");
    }
    ImGui::SameLine();
    hotkey(config->visuals.thirdpersonKey);
    ImGui::PushItemWidth(290.0f);
    ImGui::PushID(0);
    ImGui::SetNextItemWidth(140.0f);
    ImGui::SliderInt("", &config->visuals.thirdpersonDistance, 0, 1000, "Dist. 3ª Pessoa: %d");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Distância da Câmera em terceira pessoa");
    }
    ImGui::PopID();
    ImGui::NextColumn();
    ImGui::PushID(1);
    ImGui::SetNextItemWidth(140.0f);
    ImGui::SliderInt("", &config->visuals.viewmodelFov, -60, 60, "FOV Viewmodel: %d");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Altera o FOV da arma em primeira pessoa");
    }
    ImGui::PopID();
    ImGui::PushID(2);
    ImGui::SetNextItemWidth(140.0f);
    ImGui::SliderInt("", &config->visuals.fov, -60, 60, "FOV: %d");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Altera o FOV do jogo");
    }
    ImGui::PopID();
    ImGui::PushID(3);
    ImGui::SetNextItemWidth(140.0f);
    ImGui::SliderInt("", &config->visuals.farZ, 0, 2000, "Local Z: %d");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Define o LocalZ para alguns cheats");
    }
    ImGui::PopID();
    ImGui::PushID(4);
    ImGui::SetNextItemWidth(140.0f);
    ImGui::SliderInt("", &config->visuals.flashReduction, 0, 100, "Reducao Flash: %d%%");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Diminui a cegueira da flashbang de acordo com a porcentagem");
    }
    ImGui::PopID();
    ImGui::PushID(5);
    ImGui::SetNextItemWidth(140.0f);
    ImGui::SliderFloat("", &config->visuals.brightness, 0.0f, 1.0f, "Brilho: %.2f");
    ImGui::PopID();
    ImGui::PopItemWidth();
    ImGui::SetNextItemWidth(140.0f);
    ImGui::Combo("Skybox", &config->visuals.skybox, "Padrao\0cs_baggage_skybox_\0cs_tibet\0embassy\0italy\0jungle\0nukeblank\0office\0sky_cs15_daylight01_hdr\0sky_cs15_daylight02_hdr\0sky_cs15_daylight03_hdr\0sky_cs15_daylight04_hdr\0sky_csgo_cloudy01\0sky_csgo_night_flat\0sky_csgo_night02\0sky_day02_05_hdr\0sky_day02_05\0sky_dust\0sky_l4d_rural02_ldr\0sky_venice\0vertigo_hdr\0vertigo\0vertigoblue_hdr\0vietnam\0");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Define a Skybox para a especificada");
    }
    ImGuiCustom::colorPicker("Cor do Mundo", config->visuals.world);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Altera a cor do mapa");
    }
    ImGuiCustom::colorPicker("Cor do Ceu", config->visuals.sky);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Altera a cor do céu");
    }
    ImGui::Checkbox("Deagle Handspinner", &config->visuals.deagleSpinner);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Faz a inspeção da Desert Eagle ser sempre a Spinner");
    }
    ImGui::SetNextItemWidth(140.0f);
    ImGui::Combo("Efeito de Tela", &config->visuals.screenEffect, "Nenhum\0Drone cam\0Drone cam with noise\0Underwater\0Healthboost\0Dangerzone\0");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Define o efeito à tela");
    }
    ImGui::SetNextItemWidth(140.0f);
    ImGui::Combo("Efeito de Hit", &config->visuals.hitEffect, "Nenhum\0Drone cam\0Drone cam with noise\0Underwater\0Healthboost\0Dangerzone\0");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Define o efeito à tela quando acertar um oponente");
    }
    ImGui::SetNextItemWidth(140.0f);
    ImGui::SliderFloat("Duracao Efeito", &config->visuals.hitEffectTime, 0.1f, 1.5f, "%.2fs");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Duração do Efeito de Hit");
    }
    ImGui::SetNextItemWidth(140.0f);
    ImGui::Combo("Acerto na Mira", &config->visuals.hitMarker, "Nenhum\0Padrao (Mira)\0");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Exibe um detalhe na mira ao acertar o oponente");
    }
    ImGui::SetNextItemWidth(140.0f);
    ImGui::SliderFloat("Duracao Acerto", &config->visuals.hitMarkerTime, 0.1f, 1.5f, "%.2fs");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Duração do detalhe da mira ao acertar o oponente");
    }
    ImGui::Checkbox("Indicadores", &config->visuals.indicatorsEnabled);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Exibe as funções em funcionamento");
    }
    ImGui::SameLine();
    ImGui::PushID(6);
    ImGui::SetNextItemWidth(140.0f);
    ImGuiCustom::MultiCombo("", config->visuals.indicators, config->visuals.selectedIndicators, 4);
    ImGui::PopID();
    ImGui::Checkbox("Correcao de Cor", &config->visuals.colorCorrection.enabled);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Altera o tom das cores");
    }
    ImGui::SameLine();
    bool ccPopup = ImGui::Button("Editar");

    if (ccPopup)
        ImGui::OpenPopup("##popup");

    if (ImGui::BeginPopup("##popup")) {
        ImGui::VSliderFloat("##1", { 40.0f, 160.0f }, &config->visuals.colorCorrection.blue, 0.0f, 1.0f, "Azul\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##2", { 40.0f, 160.0f }, &config->visuals.colorCorrection.red, 0.0f, 1.0f, "Verm\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##3", { 40.0f, 160.0f }, &config->visuals.colorCorrection.mono, 0.0f, 1.0f, "Mono\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##4", { 40.0f, 160.0f }, &config->visuals.colorCorrection.saturation, 0.0f, 1.0f, "Sat\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##5", { 40.0f, 160.0f }, &config->visuals.colorCorrection.ghost, 0.0f, 1.0f, "Ghost\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##6", { 40.0f, 160.0f }, &config->visuals.colorCorrection.green, 0.0f, 1.0f, "Verde\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##7", { 40.0f, 160.0f }, &config->visuals.colorCorrection.yellow, 0.0f, 1.0f, "Amar.\n%.3f"); ImGui::SameLine();
        ImGui::EndPopup();
    }
    ImGui::Columns(1);

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderSkinChangerWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.skinChanger)
            return;
        ImGui::SetNextWindowSize({ 700.0f, 0.0f });
        ImGui::Begin("nSkinz", &window.skinChanger, windowFlags);
    }
    ImGui::TextUnformatted("SkinChanger | BRCheats");
    ImGui::SameLine();
    CenterText();
    if (ImGui::Button("x")) {
        window.skinChanger = !window.skinChanger;
    }
    if (ImGui::BeginTabBar("SkinChangers", ImGuiTabBarFlags_NoTooltip)) {
        if (ImGui::BeginTabItem("Armas")) {
            

            {
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
                    ImGui::Checkbox("Ativado?", &selected_entry.enabled);
                    ImGui::Separator();
                    ImGui::Columns(2, nullptr, false);
                    ImGui::InputInt("Seed", &selected_entry.seed);
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("Altera a Semente de algumas skins, gerando outros detalhes");
                    }
                    ImGui::InputInt("Stattrak\u2122", &selected_entry.stat_trak);
                    selected_entry.stat_trak = (std::max)(selected_entry.stat_trak, -1);
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("Exibe a quantia de kills como Stattrak");
                    }
                    ImGui::SliderFloat("Desgaste", &selected_entry.wear, FLT_MIN, 1.f, "%.10f", 5);
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("Define o desgaste da skin (float)");
                    }

                    ImGui::Combo(" ", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text) {
                        *out_text = (itemIndex == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits)[idx].name.c_str();
                        return true;
                        }, nullptr, (itemIndex == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits).size(), 10);

                    ImGui::SameLine();
                    {

                        if (itemIndex != 1) {
                            if (ImGui::Button("Busca"))
                            {
                                ImGui::OpenPopup("Search");
                            }
                        }
                        ImGui::SameLine();
                        ImGui::Text("Skin");
                        if (ImGui::BeginPopup("Search", ImGuiWindowFlags_NoResize))
                        {

                            static char skin_name[256];
                            static int select_current = 0;

                            ImGui::Text("Pesquisar Skin");

                            ImGui::SameLine();
                            CenterText();
                            if (ImGui::Button("x"))
                            {
                                ImGui::CloseCurrentPopup();
                            }
                            ImGui::Separator();
                            if (ImGui::InputText("", SkinChanger::skin_name, IM_ARRAYSIZE(SkinChanger::skin_name)))
                                SearchCheck(true);
                            ImGui::Text("Digite uma Skin");
                            if (ImGui::IsItemHovered()) {
                                ImGui::SetTooltip("Pesquisa com base no texto inserido (CaSo-SeNsItIvO)");
                            }
                            ImGui::Separator();
                            ImGui::SameLine();
                            if (ImGui::Button("Aplicar"))
                            {
                                for (int i = 0; i < SkinChanger::skinKits.size(); i++)
                                {
                                    if (SkinChanger::skinKits[i].id == SkinChanger::search_result[select_current].id)
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
                                &SkinChanger::search_result, SkinChanger::search_result.size(), 10);



                            ImGui::EndPopup();


                        }

                    }

                    ImGui::Combo("Qualidade", &selected_entry.entity_quality_vector_index, [](void* data, int idx, const char** out_text) {
                        *out_text = game_data::quality_names[idx].name;
                        return true;
                        }, nullptr, IM_ARRAYSIZE(game_data::quality_names), 5);

                    if (itemIndex == 0) {
                        ImGui::Combo("Faca", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text) {
                            *out_text = game_data::knife_names[idx].name;
                            return true;
                            }, nullptr, IM_ARRAYSIZE(game_data::knife_names), 5);
                    }
                    else if (itemIndex == 1) {
                        ImGui::Combo("Luva", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text) {
                            *out_text = game_data::glove_names[idx].name;
                            return true;
                            }, nullptr, IM_ARRAYSIZE(game_data::glove_names), 5);
                    }
                    else {
                        static auto unused_value = 0;
                        selected_entry.definition_override_vector_index = 0;
                    }

                    ImGui::InputText("Etiqueta", selected_entry.custom_name, 32);
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("Define o nome a arma");
                    }

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

                        ImGui::Combo("Sticker", &selected_sticker.kit_vector_index, [](void* data, int idx, const char** out_text) {
                            *out_text = SkinChanger::stickerKits[idx].name.c_str();
                            return true;
                            }, nullptr, SkinChanger::stickerKits.size(), 10);

                        ImGui::SliderFloat("Desgaste", &selected_sticker.wear, FLT_MIN, 1.0f, "%.10f", 5.0f);
                        if (ImGui::IsItemHovered()) {
                            ImGui::SetTooltip("Desgaste do Adesivo");
                        }
                        ImGui::SliderFloat("Tamanho", &selected_sticker.scale, 0.1f, 5.0f);
                        if (ImGui::IsItemHovered()) {
                            ImGui::SetTooltip("Tamanho do Adesivo");
                        }
                        ImGui::SliderFloat("Rotação", &selected_sticker.rotation, 0.0f, 360.0f);
                        if (ImGui::IsItemHovered()) {
                            ImGui::SetTooltip("Rotaciona os stickers");
                        }

                        ImGui::PopID();
                    }
                    selected_entry.update();

                    ImGui::Columns(1);



                    if (ImGui::Button("Aplicar", { 130.0f, 30.0f }))
                        SkinChanger::scheduleHudUpdate();

                }


            }

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Personagens")) {
            constexpr auto playerModels = "Default\0Special Agent Ava | FBI\0Operator | FBI SWAT\0Markus Delrow | FBI HRT\0Michael Syfers | FBI Sniper\0B Squadron Officer | SAS\0Seal Team 6 Soldier | NSWC SEAL\0Buckshot | NSWC SEAL\0Lt. Commander Ricksaw | NSWC SEAL\0Third Commando Company | KSK\0'Two Times' McCoy | USAF TACP\0Dragomir | Sabre\0Rezan The Ready | Sabre\0'The Doctor' Romanov | Sabre\0Maximus | Sabre\0Blackwolf | Sabre\0The Elite Mr. Muhlik | Elite Crew\0Ground Rebel | Elite Crew\0Osiris | Elite Crew\0Prof. Shahmat | Elite Crew\0Enforcer | Phoenix\0Slingshot | Phoenix\0Soldier | Phoenix\0Pirate\0Pirate Variant A\0Pirate Variant B\0Pirate Variant C\0Pirate Variant D\0Anarchist\0Anarchist Variant A\0Anarchist Variant B\0Anarchist Variant C\0Anarchist Variant D\0Balkan Variant A\0Balkan Variant B\0Balkan Variant C\0Balkan Variant D\0Balkan Variant E\0Jumpsuit Variant A\0Jumpsuit Variant B\0Jumpsuit Variant C\0";
            ImGui::Combo("Personagem TR.", &config->visuals.playerModelT, playerModels);
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Define a skin de personagem TR");
            }
            ImGui::Combo("Personagem CT", &config->visuals.playerModelCT, playerModels);
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Define a skin de personagem CT");
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }



    if (!contentOnly)
        ImGui::End();
}

void GUI::renderSoundWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.sound)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("Som", &window.sound, windowFlags);
    }
    ImGui::SliderInt("Volume da Galinha", &config->sound.chickenVolume, 0, 200, "%d%%");

    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::Combo("", &currentCategory, "Voce\0Aliados\0Inimigos\0");
    ImGui::PopItemWidth();
    ImGui::SliderInt("Volume Geral", &config->sound.players[currentCategory].masterVolume, 0, 200, "%d%%");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Volume de todos os sons do jogo");
    }
    ImGui::SliderInt("Volume Headshot", &config->sound.players[currentCategory].headshotVolume, 0, 200, "%d%%");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Volume do som de headshot");
    }
    ImGui::SliderInt("Volume da Arma", &config->sound.players[currentCategory].weaponVolume, 0, 200, "%d%%");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Volume do som do disparo das armas");
    }
    ImGui::SliderInt("Volume dos Passos", &config->sound.players[currentCategory].footstepVolume, 0, 200, "%d%%");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Volume dos passos enquanto anda");
    }

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderskinchangersWindow(bool contentOnly) noexcept
{ 
    if (!contentOnly) {
        if (!window.SkinChangers)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("SkinChangers", &window.SkinChangers, windowFlags);
    }

    if (ImGui::BeginTabBar("SkinChangers", ImGuiTabBarFlags_NoTooltip)) {
        if (ImGui::BeginTabItem("Armas")) {
            renderSkinChangerWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Personagens")) {
            
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }


    if (!contentOnly)
        ImGui::End();
}

void GUI::renderPlayerWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.playerChanger)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("Estilo", &window.playerChanger, windowFlags);
    }



    if (!contentOnly)
        ImGui::End();
}

void GUI::renderStyleWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.style)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("Estilo", &window.style, windowFlags);
    }

    ImGui::PushItemWidth(150.0f);
    if (ImGui::Combo("Cores do Menu", &config->style.menuColors, "Nether\0Claro\0Classico\0Custom\0"))
        updateColors();
    ImGui::PopItemWidth();

    if (config->style.menuColors == 3) {
        ImGuiStyle& style = ImGui::GetStyle();
        for (int i = 0; i < ImGuiCol_COUNT; i++) {
            if (i && i & 3) ImGui::SameLine(220.0f * (i & 3));

            ImGuiCustom::colorPicker(ImGui::GetStyleColorName(i), (float*)&style.Colors[i]);
        }
    }

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderMiscWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.misc)
            return;
        ImGui::SetNextWindowSize({ 600.0f, 0.0f });
        ImGui::Begin("Outros", &window.misc, windowFlags);
    }
    ImGui::TextUnformatted("Outros | BRCheats");
    ImGui::SameLine();
    CenterText();
    if (ImGui::Button("x")) {
        window.misc = !window.misc;
    }
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 250.0f);
    ImGui::TextUnformatted("Tecla do Menu");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Mude a tecla que abre o menu");
    }
    ImGui::SameLine();
    hotkey(config->misc.menuKey);
    ImGui::Text("Healthshot Rapido");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Usa o Healthshot rapidamente ao pressionar a tecla");
    }
    ImGui::SameLine();
    hotkey(config->misc.quickHealthshotKey);    
    ImGui::Checkbox("Fast plant", &config->misc.fastPlant);
    ImGuiCustom::colorPicker("Bomb timer", config->misc.bombTimer);
    ImGui::Checkbox("Quick reload", &config->misc.quickReload);
    ImGui::Checkbox("Prepare revolver", &config->misc.prepareRevolver);
    ImGui::SameLine();
    hotkey(config->misc.prepareRevolverKey);
    ImGui::Checkbox("Anti AFK kick", &config->misc.antiAfkKick);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Você não será expulso automaticamente por ficar inativo");
    }
    ImGui::Checkbox("Auto strafe", &config->misc.autoStrafe);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Dá stafe automaticamente (o personagem vai para frente ao pular sozinho, resultando em melhor movimentação)");
    }
    if (config->misc.bunnyStyle == 2){
        config->misc.bunnyHuman = true;
        ImGui::SliderInt("BHop Chance", &config->misc.bhopHitchance, 0, 100, "%d%");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Probabilidade de sucesso do bhop");
        }
        ImGui::SliderInt("Min Pulos", &config->misc.bhopMinHits, 0, 20, "%d%");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Mínimo de pulos que o bhop dará");
        }
        ImGui::SliderInt("Max Pulos", &config->misc.bhopMaxHits, 0, 20, "%d%");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Máximo de pulos que o bhop dará");
        }
    }
    else {
        config->misc.bunnyHuman = false;
    }
    if (config->misc.bunnyStyle == 1) {
        config->misc.bunnyHop = true;
    }
    
    ImGui::Checkbox("Agachar Rapido", &config->misc.fastDuck);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Seu personagem não cansará ao ficar se agachando");
    }
    ImGui::Checkbox("Pernas Surfistas", &config->misc.moonwalk);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Seu personagem \"Surfará\" ao andar");
    }
    ImGui::Checkbox("Pulo Lateral", &config->misc.edgejump);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Desincroniza o pulo do personagem (EdgeJump)");
    }
    ImGui::SameLine();
    hotkey(config->misc.edgejumpkey);
    ImGui::Checkbox("Slowwalk", &config->misc.slowwalk);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Reduz sua velocidade de movimento propositalmente para confundir resolvedores de Aimbot");
    }
    ImGui::SameLine();
    hotkey(config->misc.slowwalkKey);
    ImGui::Checkbox("Mira em Sniper", &config->misc.sniperCrosshair);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("A mira não sumirá ao equipar uma sniper");
    }
    ImGui::Checkbox("Recuo com Mira", &config->misc.recoilCrosshair);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("A mira acompanhará o recuo dos tiros");
    }
    ImGui::Checkbox("Auto pistol", &config->misc.autoPistol);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Atira o mais rápido possível com a pistola (resulta em perda de precisão)");
    }
    ImGui::Checkbox("Auto Recarregar", &config->misc.autoReload);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Recarrega automaticamente a arma ao acabar as munições");
    }
    ImGui::Checkbox("Auto Aceitar", &config->misc.autoAccept);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Aceita automaticamente partidas encontradas (pode resultar em bugs)");
    }
    ImGui::Checkbox("Radar Hack", &config->misc.radarHack);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Exibe os inimigos no radar");
    }
    ImGui::Checkbox("Mostrar Ranks", &config->misc.revealRanks);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Mostra o rank de todos da partida no scoreboard (TAB)");
    }
    ImGui::Checkbox("Mostrar Dinheiro", &config->misc.revealMoney);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Mostra o dinheiro da equipe inimiga no scoreboard (TAB)");
    }
    ImGui::Checkbox("Mostrar Suspeito", &config->misc.revealSuspect);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Revela a identidade do suspeito em fiscalizações");
    }
    ImGui::Checkbox("Mostrar Dano-Amigo", &config->misc.showTeamDamage);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Exibe o dano causado à companheiros de equipe (Se ultrapassar 500, você é expulso)");
    }
    if (config->misc.showTeamDamage) {
        ImGui::SameLine();
        if (ImGui::Button("Reset")) {
            Misc::teamKills = 0;
            Misc::teamDamage = 0;
        }
    }
    ImGui::Checkbox("Desativar Oclusao de Personagens", &config->misc.disableModelOcclusion);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Corrige o problema de ver jogadores através da smoke em alguns casos");
    }
    ImGui::Checkbox("Desativar Blur do HUD", &config->misc.disablePanoramablur);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Desativa o Blur da interface");
    }
    ImGuiCustom::colorPicker("Espectadores", config->misc.spectatorList);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Exibe os jogadores que estão te espectando");
    }
    ImGuiCustom::colorPicker("Marca D'agua", config->misc.watermark);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Exibe o nome do cheat c/ informações de FPS e ping");
    }
    ImGui::Checkbox("Clan Animado", &config->misc.animatedClanTag);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Anima o clan atual");
    }
    ImGui::Checkbox("Clan Relogio", &config->misc.clocktag);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Transforma seu clan em um relógio");
    }
    ImGui::Checkbox("Clan Customizado", &config->misc.customClanTag);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Permite definir um clan customizado");
    }
    ImGui::SameLine();
    ImGui::PushItemWidth(120.0f);
    ImGui::PushID(0);
    if (ImGui::InputText("", config->misc.clanTag, sizeof(config->misc.clanTag)))
        Misc::updateClanTag(true);
    ImGui::PopID();

    ImGui::Checkbox("Mensagem de Kill", &config->misc.killMessage);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Envia a mensagem no chat ao eliminar um oponente");
    }
    ImGui::SameLine();
    ImGui::PushItemWidth(120.0f);
    ImGui::PushID(1);
    ImGui::InputText("", &config->misc.killMessageString);
    ImGui::PopID();
    ImGui::Combo("Bunny hop", &config->misc.bunnyStyle, "Desligado\0Normal\0Humano\0");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Normal = Bunnyhop perfeito | Humano = Permite configurar a chance de sucesso do bhop além dos pulos mínimos e máximos");
    }
    ImGui::NextColumn();
    ImGui::Combo("Nick Exploit", &config->misc.nameChangeSelection, "Desligado\0Fake Ban\0Fake Item\0Nick Custom");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Algumas funções para enganar (ataque um companheiro de equipe para exibir a mensagem)");
    }
    if (config->misc.nameChangeSelection == 1)
    {
        ImGui::PushID(3);
        ImGui::SetNextItemWidth(100.0f);
        ImGui::Combo("", &config->misc.banColor, "Branco\0Vermelho\0Roxo\0Verde\0Verde Claro\0Turquesa\0Vermelho Claro\0Cinza\0Amarelo\0Cinza 2\0Azul Claro\0Cinza Aroxoado\0Azul\0Rosa\0Laranja Escuro\0Laranja\0");
        ImGui::PopID();
        ImGui::SameLine();
        ImGui::PushID(4);
        ImGui::InputText("", &config->misc.banText);
        ImGui::PopID();
        if (ImGui::Button("Instalar Fake Ban"))
            Misc::fakeBan(true);
    }
    else if (config->misc.nameChangeSelection == 2)
    {
        ImGui::SetNextItemWidth(200.0f);
        ImGuiCustom::MultiCombo("Fake Item Flags", config->misc.fakeItemFlags, config->misc.selectedFakeItemFlags, 4);
        ImGui::SetNextItemWidth(200.0f);
        ImGui::Combo("Time", &config->misc.fakeItemTeam, "Contra-Terroristas\0Terroristas\0");
        ImGui::SetNextItemWidth(200.0f);
        ImGui::Combo("Mensagem de", &config->misc.fakeItemMessageType, "Caixa\0Troca\0");
        ImGui::SetNextItemWidth(200.0f);
        ImGui::Combo("Item", &config->misc.fakeItemType, "AK-47\0AUG\0AWP\0Bayonet\0Bowie Knife\0Butterfly Knife\0CZ75-Auto\0Faca Classica\0Desert Eagle\0Dual Berettas\0FAMAS\0Falchion Knife\0Five-SeveN\0Flip Knife\0G3SG1\0Galil AR\0Glock-18\0Gut Knife\0Huntsman Knife\0Karambit\0M249\0M4A1-S\0M4A4\0M9 Bayonet\0MAC-10\0MAG-7\0MP5-SD\0MP7\0MP9\0Navaja Knife\0Negev\0Nomad Knife\0Nova\0P2000\0P250\0P90\0PP-Bizon\0Paracord Knife\0R8 Revolver\0SCAR-20\0SG 553\0SSG 08\0Sawed-Off\0Shadow Daggers\0Skeleton Knife\0Spectral Shiv\0Stiletto Knife\0Survival Knife\0Talon Knife\0Tec-9\0UMP-45\0USP-S\0Ursus Knife\0XM1014\0Hand Wraps\0Moto Gloves\0Specialist Gloves\0Sport Gloves\0Bloodhound Gloves\0Hydra Gloves\0Driver Gloves\0");
        ImGui::SetNextItemWidth(200.0f);
        ImGui::Combo("Cor/Raridade", &config->misc.fakeItemRarity, "Nivel Consumidor (Branco)\0Nivel Industrial (Azul Claro)\0Mil-Spec (Azul)\0Restrita (Roxo)\0Classificada (Rosa)\0Secreto (Vermelho)\0Contrabando(Dourado)\0");
        ImGui::Combo("Cor do Nick", &config->misc.fakeItemPlayerColor, "Amarelo\0Verde\0Azul\0Roxo\0Laranja\0");
        ImGui::InputText("Nick do Player", &config->misc.fakeItemPlayerName);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Deverá ser definido para não aparecer como ?empty");
        }
        ImGui::InputText("Nome da Skin", &config->misc.fakeItemName);
        if (ImGui::Button("Aplicar"))
            Misc::fakeItem(true);
    }
    else if (config->misc.nameChangeSelection == 3)
    {
        ImGui::Checkbox("Roubar Nicks", &config->misc.nameStealer);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Rouba os nicks dos usuários da partida");
        }
        ImGui::InputText("Nick Custom", &config->misc.customName);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Define um nick customizado");
        }
        if (ImGui::Button("Aplicar"))
            Misc::setName(true);
    }
   
    ImGui::Combo("Som de Hit", &config->misc.hitSound, "Nenhum\0Metal\0Gamesense\0Sino\0Vidro\0Custom\0");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Faz o som ao acertar o oponente");
    }
    if (config->misc.hitSound == 5) {
        ImGui::InputText("Nome do Arquivo", &config->misc.customHitSound);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Coloque o arquivo na pasta csgo/sound/");
    }

    ImGui::PushID(5);
    ImGui::Combo("Kill Sound", &config->misc.killSound, "Nenhum\0Metal\0Gamesense\0Sino\0Vidro\0Custom\0");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Faz o som ao eliminar o oponente");
    }
    if (config->misc.killSound == 5) {
        ImGui::InputText("Kill Sound filename", &config->misc.customKillSound);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Coloque o arquivo na pasta csgo/sound/");
    }
    ImGui::PopID();

    ImGui::SetNextItemWidth(90.0f);
    ImGui::Combo("Fake Lag", &config->misc.fakeLagMode, "Desligado\0Normal\0Adaptivo\0Aleatorio\0Alternado");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Define o modo do FakeLag");
    }
    ImGui::SameLine();
    hotkey(config->misc.fakeLagKey);
    if (!(config->misc.fakeLagMode == 0))
    {
        ImGuiCustom::MultiCombo("Pontos", config->misc.fakeLagFlags, config->misc.fakeLagSelectedFlags, 4);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Momentos em que o fakelag estará ativado");
        }
        if (config->misc.fakeLagMode == 3)
        {
            ImGui::SetNextItemWidth(120.0f);
            ImGui::SliderInt("Ticks Minimos", &config->misc.fakeLagTicks, 1, 16);
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Quantia mínima de ticks afetadas pelo fakelag");
            }
            config->misc.fakeLagTicks = std::clamp(config->misc.fakeLagTicks, 1, 64);
        }
        else if (!(config->misc.fakeLagMode == 4))
        {
            ImGui::SetNextItemWidth(120.0f);
            ImGui::SliderInt("Quantia de Ticks", &config->misc.fakeLagTicks, 1, 16);
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Quantia de ticks afetadas no geral");
            }
            config->misc.fakeLagTicks = std::clamp(config->misc.fakeLagTicks, 1, 64);
        }
    }
    ImGui::Checkbox("Plantar Rapido", &config->misc.fastPlant);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Faz uma troca rápida ao plantar a bomba");
    }
    ImGuiCustom::colorPicker("Tempo da C4", config->misc.bombTimer);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Exibe o tempo de explosão da C4, junto com o tempo de defuse e se é possível defusar ou não");
    }
    ImGui::Checkbox("Recarregar Rapido", &config->misc.quickReload);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Faz uma troca rápida ao recarregar a arma");
    }
    ImGui::Checkbox("Preparar Revolver", &config->misc.prepareRevolver);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Deixa o R8 Revolver no ponto ao pressionar a tecla");
    }
    ImGui::SameLine();
    hotkey(config->misc.prepareRevolverKey);
    ImGui::Checkbox("Trajetoria da Granada", &config->misc.nadePredict);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Exibe a trajetória da granada");
    }
    ImGui::Checkbox("Tablet com Sinal", &config->misc.fixTabletSignal);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Corrige locais onde o tablet não funcionaria no DangerZone");
    }
    ImGui::Checkbox("Prime Falso", &config->misc.fakePrime);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Adiciona o ícone prime ao ser perfil");
    }
    ImGui::Checkbox("Fakeduck", &config->misc.fakeDuck);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Faz você aparecer agachado, mesmo não estando");
    }
    ImGui::SameLine();
    hotkey(config->misc.fakeDuckKey);
    ImGui::Checkbox("Lista de Compras", &config->misc.purchaseList.enabled);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Exibe uma lista da compra dos players da partida");
    }
    ImGui::SameLine();

    ImGui::PushID("Lista de Compras");
    if (ImGui::Button("..."))
        ImGui::OpenPopup("");

    if (ImGui::BeginPopup("")) {
        ImGui::SetNextItemWidth(75.0f);
        ImGui::Combo("Modo", &config->misc.purchaseList.mode, "Detalhes\0Simples\0");
        ImGui::Checkbox("Somente durante compras", &config->misc.purchaseList.onlyDuringFreezeTime);
        ImGui::Checkbox("Mostrar preços", &config->misc.purchaseList.showPrices);
        ImGui::Checkbox("Sem título", &config->misc.purchaseList.noTitleBar);
        ImGui::EndPopup();
    }
    ImGui::PopID();
    ImGui::SliderFloat("Proporcao", &config->misc.aspectratio, 0.0f, 5.0f, "%.2f");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Permite alterar a proporção do jogo (podendo até jogar em 4:3 em resoluções 16:9)");
    }
    if (ImGui::Button("Sair"))
        hooks->uninstall();

    ImGui::Columns(1);
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderReportbotWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.reportbot)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("Reportbot", &window.reportbot, windowFlags);
    }
    ImGui::Checkbox("Ligado?", &config->reportbot.enabled);
    ImGui::SameLine(0.0f, 50.0f);
    if (ImGui::Button("Resetar"))
        Reportbot::reset();
    ImGui::Separator();
    ImGui::PushItemWidth(80.0f);
    ImGui::Combo("Alvo", &config->reportbot.target, "Inimigos\0Aliados\0Todos\0");
    ImGui::InputInt("Delay (s)", &config->reportbot.delay);
    config->reportbot.delay = (std::max)(config->reportbot.delay, 1);
    ImGui::InputInt("Rounds", &config->reportbot.rounds);
    config->reportbot.rounds = (std::max)(config->reportbot.rounds, 1);
    ImGui::PopItemWidth();
    ImGui::Checkbox("Comunicacao Abusiva", &config->reportbot.textAbuse);
    ImGui::Checkbox("Antijogo", &config->reportbot.griefing);
    ImGui::Checkbox("Auxilio Visual", &config->reportbot.wallhack);
    ImGui::Checkbox("Auxilio de Mira", &config->reportbot.aimbot);
    ImGui::Checkbox("Outros Hacks", &config->reportbot.other);

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderConfigWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.config)
            return;
        ImGui::SetNextWindowSize(ImVec2(580.0f, 0.0f));
        ImGui::Begin("Config", &window.config, windowFlags);
    }
    ImGui::TextUnformatted("Config | BRCheats");
    ImGui::SameLine();
    CenterText();
    if (ImGui::Button("x")) {
        window.config = !window.config;
    }
    if (ImGui::BeginTabBar("Configs", ImGuiTabBarFlags_NoTooltip)) {
        if (ImGui::BeginTabItem("Config")) {
            ImGui::Columns(2, nullptr, false);
            ImGui::SetColumnOffset(1, 170.0f);

            ImGui::PushItemWidth(160.0f);

            auto& configItems = config->getConfigs();
            static int currentConfig = -1;

            if (static_cast<std::size_t>(currentConfig) >= configItems.size())
                currentConfig = -1;

            static std::string buffer;

            if (ImGui::Button("Recarregar Lista", { 100.0F, 25.0F }))
                config->Config::Config("BRCheats");

            ImGui::SameLine();
            if (ImGui::Button("Nova", { 50.0f, 25.0f }))
                config->add(buffer.c_str());
            if (ImGui::ListBox("", &currentConfig, [](void* data, int idx, const char** out_text) {
                auto& vector = *static_cast<std::vector<std::string>*>(data);
                *out_text = vector[idx].c_str();
                return true;
                }, &configItems, configItems.size(), 5) && currentConfig != -1)
                buffer = configItems[currentConfig];

                ImGui::PushID(0);
                if (ImGui::InputTextWithHint("", "Nome da Config", &buffer, ImGuiInputTextFlags_EnterReturnsTrue)) {
                    if (currentConfig != -1)
                        config->rename(currentConfig, buffer.c_str());
                }
                ImGui::PopID();
                ImGui::NextColumn();
                ImGui::PushItemWidth(60.0f);

                if (ImGui::Button("Resetar", { 60.0f, 25.0f }))
                    ImGui::OpenPopup("Qual config?");




                if (ImGui::BeginPopup("Qual config?")) {
                    static constexpr const char* names[]{ "Tudo", "Aimbot", "Triggerbot", "Backtrack", "Anti aim", "Glow", "Chams", "Esp", "Visuais", "Skin Changer", "Sound", "Style", "Outros", "Reportbot" };
                    for (int i = 0; i < IM_ARRAYSIZE(names); i++) {
                        if (i == 1) ImGui::Separator();

                        if (ImGui::Selectable(names[i])) {
                            switch (i) {
                            case 0: config->reset(); updateColors(); Misc::updateClanTag(true); SkinChanger::scheduleHudUpdate(); break;
                            case 1: config->aimbot = { }; break;
                            case 2: config->triggerbot = { }; break;
                            case 3: config->backtrack = { }; break;
                            case 4: config->antiAim = { }; break;
                            case 5: config->glow = { }; break;
                            case 6: config->chams = { }; break;
                            case 7: config->esp = { }; break;
                            case 8: config->visuals = { }; break;
                            case 9: config->skinChanger = { }; SkinChanger::scheduleHudUpdate(); break;
                            case 10: config->sound = { }; break;
                            case 11: config->style = { }; updateColors(); break;
                            case 12: config->misc = { };  Misc::updateClanTag(true); break;
                            case 13: config->reportbot = { }; break;
                            }
                        }
                    }
                    ImGui::EndPopup();
                }
                if (currentConfig != -1) {
                    if (ImGui::Button("Carregar", { 60.0f, 25.0f })) {
                        config->load(currentConfig);
                        updateColors();
                        SkinChanger::scheduleHudUpdate();
                        Misc::updateClanTag(true);
                    }
                    if (ImGui::Button("Salvar", { 60.0f, 25.0f }))
                        config->save(currentConfig);
                    if (ImGui::Button("Deletar", { 60.0f, 25.0f })) {
                        config->remove(currentConfig);
                        currentConfig = -1;
                        buffer.clear();

                    }
                }
                ImGui::Columns(1);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Auto Config")) {
            static int currentCategory{ 0 };
            static int currentWeapon{ 0 };



            if (ImGui::Button("Legit Aimbot")) {

                //Desativando conflitos
                config->aimbot[currentWeapon].onKey = false;
                config->aimbot[currentWeapon].enabled = false;
                config->aimbot[currentWeapon].fov = 0.0f;
                config->aimbot[currentWeapon].smooth = 1.0f;
                config->aimbot[currentWeapon].aimlock = false;
                config->aimbot[currentWeapon].scopedOnly = false;
                config->aimbot[currentWeapon].autoShot = false;
                config->aimbot[currentWeapon].silent = false;
                config->aimbot[currentWeapon].visibleOnly = false;
                config->aimbot[currentWeapon].ignoreFlash = false;
                config->aimbot[currentWeapon].ignoreSmoke = false;
                config->aimbot[currentWeapon].minDamage = 0;
                config->aimbot[currentWeapon].killshot = false;
                config->aimbot[currentWeapon].bone = 0;
                config->aimbot[currentWeapon].killshot = false;
                config->aimbot[currentWeapon].scopedOnly = false;
                config->aimbot[currentWeapon].autoScope = false;
                config->aimbot[currentWeapon].maxShotInaccuracy = 1;
                config->aimbot[currentWeapon].maxAimInaccuracy = 1;
                config->aimbot[currentWeapon].recoilbasedFov = false;


                //Ativando Legit
                config->aimbot[currentWeapon].onKey = true;
                config->aimbot[currentWeapon].key = 1;
                config->aimbot[currentWeapon].enabled = true;
                config->aimbot[currentWeapon].fov = 6.8f;
                config->aimbot[currentWeapon].smooth = 28.0f;
                config->aimbot[currentWeapon].aimlock = true;
                config->aimbot[currentWeapon].scopedOnly = false;
                config->aimbot[currentWeapon].visibleOnly = true;
                config->aimbot[currentWeapon].recoilbasedFov = true;
                config->aimbot[currentWeapon].recoilControlX = 0.25f;
                config->aimbot[currentWeapon].recoilControlY = 0.25f;
                config->aimbot[currentWeapon].standaloneRCS = false;
            };
            ImGui::SameLine();
            ImGui::Text("Aimbot disfarcado, bem fraco");
            ImGui::Separator();
            if (ImGui::Button("Rage Aimbot")) {

                //Ativando Rage
                config->aimbot[currentWeapon].onKey = false;
                config->aimbot[currentWeapon].enabled = true;
                config->aimbot[currentWeapon].fov = 255.0f;
                config->aimbot[currentWeapon].smooth = 1.0f;
                config->aimbot[currentWeapon].aimlock = false;
                config->aimbot[currentWeapon].scopedOnly = false;
                config->aimbot[currentWeapon].autoShot = true;
                config->aimbot[currentWeapon].silent = true;
                config->aimbot[currentWeapon].visibleOnly = false;
                config->aimbot[currentWeapon].ignoreFlash = true;
                config->aimbot[currentWeapon].ignoreSmoke = true;
                config->aimbot[currentWeapon].minDamage = 20;
                config->aimbot[currentWeapon].killshot = true;
                config->aimbot[currentWeapon].bone = 2;
                config->aimbot[currentWeapon].killshot = true;
                config->aimbot[currentWeapon].scopedOnly = true;
                config->aimbot[currentWeapon].autoScope = true;
                config->aimbot[currentWeapon].maxShotInaccuracy = 0.015350000001490116;
                config->aimbot[currentWeapon].maxAimInaccuracy = 0.014270000159740448;
                config->aimbot[currentWeapon].recoilbasedFov = true;
                config->aimbot[currentWeapon].standaloneRCS = true;
                config->aimbot[currentWeapon].recoilControlX = 1.0f;
                config->aimbot[currentWeapon].recoilControlY = 1.0f;

            };
            ImGui::SameLine();
            ImGui::Text("Aimbot maximo, vara paredes, atira automaticamente e normalmente sempre headshot");




            ImGui::Separator();

            if (ImGui::Button("Simple Wall")) {
                static int currentCategory = 1;
                static int currentType = 0;

                static int currentItem = currentCategory * 3 + currentType;
                config->glow[currentItem].enabled = true;
                config->glow[currentItem].healthBased = true;
            }; ImGui::SameLine();    ImGui::Text("Wallhack simples, contornando o inimigo");

            ImGui::Separator();
            if (ImGui::Button("Advanced Visual")) {
                config->glow[3].enabled = true;
                config->glow[3].healthBased = true;
                config->glow[6].enabled = true;
                config->glow[6].healthBased = true;
                config->glow[13].enabled = true;
                config->glow[14].enabled = true;
                config->glow[15].enabled = true;
                config->glow[16].enabled = true;
                config->glow[17].enabled = true;
                config->glow[18].enabled = true;
                config->glow[19].enabled = true;
                config->misc.bombTimer.enabled = true;
                config->misc.disableModelOcclusion = true;
                config->visuals.disablePostProcessing = true;
                config->visuals.noFog = true;
                config->visuals.noBlur = true;
                config->visuals.noGrass = true;
                config->visuals.noShadows = true;
                config->visuals.flashReduction = 30;


            };
            ImGui::SameLine();
            ImGui::Text("Wallhack completo + auxiliares de visualizacao.");

            ImGui::Separator();
            if (ImGui::Button("Utilities")) {

                config->misc.revealRanks = true;
                config->misc.antiAfkKick = true;
                config->misc.fastDuck = true;
                config->misc.sniperCrosshair = true;
                config->misc.autoReload = true;
                config->misc.autoAccept = true;
                config->misc.revealRanks = true;
                config->misc.revealMoney = true;
                config->misc.revealSuspect = true;
                config->misc.quickReload = true;
                config->misc.fastPlant = true;
                config->misc.quickHealthshotKey = true;
                config->misc.nadePredict = true;
                config->misc.fixTabletSignal = true;



            };
            ImGui::SameLine();
            ImGui::Text("Alguns utilitarios, como ver patentes, dinheiro e algumas outras opcoes interessantes.");

            ImGui::Separator();
            if (ImGui::Button("BunnyHopping")) {

                config->misc.bunnyHop = true;
                config->misc.autoStrafe = true;

            };
            ImGui::SameLine();
            ImGui::Text("Ativa o Auto BunnyHopping (Segure Espaco).");

            ImGui::Separator();
            if (ImGui::Button("Resetar")) {

                config->reset(); Misc::updateClanTag(true); SkinChanger::scheduleHudUpdate();


            };
            ImGui::SameLine();
            ImGui::Text("Redefine todas as configuracoes do cheat.");



            ImGui::TextUnformatted("AutoConfig by Caillou");    ImGui::SameLine(); ImGui::SameLine();   if (ImGui::Button("Info"))
                ImGui::OpenPopup("Informacoes");

            bool open = true;
            if (ImGui::BeginPopupModal("Informacoes", &open))
            {
                ImGui::Text("Este é um facilitador de configuracoes.");
                ImGui::Text("Aprenda a fazer sua propria configuracao em http://bit.ly/BRCheatsCfg.");
                ImGui::Text(" ");
                ImGui::Text("By: Caillou (http://bit.ly/CaillouProfile) ");
                ImGui::Text("Apoie nosso projeto acessando https://cheaters.pro");
                if (ImGui::Button("Fechar"))
                    ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

   
        if (!contentOnly)
            ImGui::End();
}

void GUI::renderAutoConfigWindow(bool contentOnly) noexcept
{

    if (!contentOnly) {
        if (!window.autoconfig)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("AutoConfig", &window.autoconfig, windowFlags);
    }
    static int currentCategory{ 0 };
    static int currentWeapon{ 0 };



    if (ImGui::Button("Legit Aimbot")) {

        //Desativando conflitos
        config->aimbot[currentWeapon].onKey = false;
        config->aimbot[currentWeapon].enabled = false;
        config->aimbot[currentWeapon].fov = 0.0f;
        config->aimbot[currentWeapon].smooth = 1.0f;
        config->aimbot[currentWeapon].aimlock = false;
        config->aimbot[currentWeapon].scopedOnly = false;
        config->aimbot[currentWeapon].autoShot = false;
        config->aimbot[currentWeapon].silent = false;
        config->aimbot[currentWeapon].visibleOnly = false;
        config->aimbot[currentWeapon].ignoreFlash = false;
        config->aimbot[currentWeapon].ignoreSmoke = false;
        config->aimbot[currentWeapon].minDamage = 0;
        config->aimbot[currentWeapon].killshot = false;
        config->aimbot[currentWeapon].bone = 0;
        config->aimbot[currentWeapon].killshot = false;
        config->aimbot[currentWeapon].scopedOnly = false;
        config->aimbot[currentWeapon].autoScope = false;
        config->aimbot[currentWeapon].maxShotInaccuracy = 1;
        config->aimbot[currentWeapon].maxAimInaccuracy = 1;
        config->aimbot[currentWeapon].recoilbasedFov = false;


        //Ativando Legit
        config->aimbot[currentWeapon].onKey = true;
        config->aimbot[currentWeapon].key = 1;
        config->aimbot[currentWeapon].enabled = true;
        config->aimbot[currentWeapon].fov = 6.8f;
        config->aimbot[currentWeapon].smooth = 28.0f;
        config->aimbot[currentWeapon].aimlock = true;
        config->aimbot[currentWeapon].scopedOnly = false;
        config->aimbot[currentWeapon].visibleOnly = true;
        config->aimbot[currentWeapon].recoilbasedFov = true;
        config->aimbot[currentWeapon].recoilControlX = 0.25f;
        config->aimbot[currentWeapon].recoilControlY = 0.25f;
        config->aimbot[currentWeapon].standaloneRCS = false;
    };
    ImGui::SameLine();
    ImGui::Text("Aimbot disfarcado, bem fraco");
    ImGui::Separator();
    if (ImGui::Button("Rage Aimbot")) {

        //Ativando Rage
        config->aimbot[currentWeapon].onKey = false;
        config->aimbot[currentWeapon].enabled = true;
        config->aimbot[currentWeapon].fov = 255.0f;
        config->aimbot[currentWeapon].smooth = 1.0f;
        config->aimbot[currentWeapon].aimlock = false;
        config->aimbot[currentWeapon].scopedOnly = false;
        config->aimbot[currentWeapon].autoShot = true;
        config->aimbot[currentWeapon].silent = true;
        config->aimbot[currentWeapon].visibleOnly = false;
        config->aimbot[currentWeapon].ignoreFlash = true;
        config->aimbot[currentWeapon].ignoreSmoke = true;
        config->aimbot[currentWeapon].minDamage = 20;
        config->aimbot[currentWeapon].killshot = true;
        config->aimbot[currentWeapon].bone = 2;
        config->aimbot[currentWeapon].killshot = true;
        config->aimbot[currentWeapon].scopedOnly = true;
        config->aimbot[currentWeapon].autoScope = true;
        config->aimbot[currentWeapon].maxShotInaccuracy = 0.015350000001490116;
        config->aimbot[currentWeapon].maxAimInaccuracy = 0.014270000159740448;
        config->aimbot[currentWeapon].recoilbasedFov = true;
        config->aimbot[currentWeapon].standaloneRCS = true;
        config->aimbot[currentWeapon].recoilControlX = 1.0f;
        config->aimbot[currentWeapon].recoilControlY = 1.0f;

    };
    ImGui::SameLine();
    ImGui::Text("Aimbot maximo, vara paredes, atira automaticamente e normalmente sempre headshot");




    ImGui::Separator();

    if (ImGui::Button("Simple Wall")) {
        static int currentCategory = 1;
        static int currentType = 0;

        static int currentItem = currentCategory * 3 + currentType;
        config->glow[currentItem].enabled = true;
        config->glow[currentItem].healthBased = true;
    }; ImGui::SameLine();    ImGui::Text("Wallhack simples, contornando o inimigo");

    ImGui::Separator();
    if (ImGui::Button("Advanced Visual")) {
        config->glow[3].enabled = true;
        config->glow[3].healthBased = true;
        config->glow[6].enabled = true;
        config->glow[6].healthBased = true;
        config->glow[13].enabled = true;
        config->glow[14].enabled = true;
        config->glow[15].enabled = true;
        config->glow[16].enabled = true;
        config->glow[17].enabled = true;
        config->glow[18].enabled = true;
        config->glow[19].enabled = true;
        config->misc.bombTimer.enabled = true;
        config->misc.disableModelOcclusion = true;
        config->visuals.disablePostProcessing = true;
        config->visuals.noFog = true;
        config->visuals.noBlur = true;
        config->visuals.noGrass = true;
        config->visuals.noShadows = true;
        config->visuals.flashReduction = 30;


    };
    ImGui::SameLine();
    ImGui::Text("Wallhack completo + auxiliares de visualizacao.");

    ImGui::Separator();
    if (ImGui::Button("Utilities")) {

        config->misc.revealRanks = true;
        config->misc.antiAfkKick = true;
        config->misc.fastDuck = true;
        config->misc.sniperCrosshair = true;
        config->misc.autoReload = true;
        config->misc.autoAccept = true;
        config->misc.revealRanks = true;
        config->misc.revealMoney = true;
        config->misc.revealSuspect = true;
        config->misc.quickReload = true;
        config->misc.fastPlant = true;
        config->misc.quickHealthshotKey = true;
        config->misc.nadePredict = true;
        config->misc.fixTabletSignal = true;
      


    };
    ImGui::SameLine();
    ImGui::Text("Alguns utilitarios, como ver patentes, dinheiro e algumas outras opcoes interessantes.");

    ImGui::Separator();
    if (ImGui::Button("BunnyHopping")) {

        config->misc.bunnyHop = true;
        config->misc.autoStrafe = true;

    };
    ImGui::SameLine();
    ImGui::Text("Ativa o Auto BunnyHopping (Segure Espaco).");

    ImGui::Separator();
    if (ImGui::Button("Resetar")) {

        config->reset(); Misc::updateClanTag(true); SkinChanger::scheduleHudUpdate();


    };
    ImGui::SameLine();
    ImGui::Text("Redefine todas as configuracoes do cheat.");



    ImGui::TextUnformatted("AutoConfig by Caillou");    ImGui::SameLine(); ImGui::SameLine();   if (ImGui::Button("Info"))
        ImGui::OpenPopup("Informacoes");

    bool open = true;
    if (ImGui::BeginPopupModal("Informacoes", &open))
    {
        ImGui::Text("Este é um facilitador de configuracoes.");
        ImGui::Text("Aprenda a fazer sua propria configuracao em http://bit.ly/BRCheatsCfg.");
        ImGui::Text(" ");
        ImGui::Text("By: Caillou (http://bit.ly/CaillouProfile) ");
        ImGui::Text("Apoie nosso projeto acessando https://cheaters.pro");
        if (ImGui::Button("Fechar"))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderwallhacksWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.wallhacks)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("Wallhacks", &window.wallhacks, windowFlags);
    }

    if (ImGui::BeginTabBar("Wallhacks", ImGuiTabBarFlags_NoTooltip)) {
        if (ImGui::BeginTabItem("ESP")) {
            renderEspWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Glow")) {
            renderGlowWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Chams")) {
            renderChamsWindow(true);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }


    if (!contentOnly)
        ImGui::End();
}



void GUI::renderotherhacksWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.other)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("Outros", &window.other, windowFlags);
    }

    if (ImGui::BeginTabBar("Outros", ImGuiTabBarFlags_NoTooltip)) {
        if (ImGui::BeginTabItem("Derivados")) {
            renderMiscWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("AntiAim")) {
            renderAntiAimWindow(true);
            ImGui::EndTabItem();
        }
        

        ImGui::EndTabBar();
    }


    if (!contentOnly)
        ImGui::End();
}


void GUI::renderaimhacksWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.aimhacks)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("AimHacks", &window.aimhacks, windowFlags);
    }

    if (ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_NoTooltip)) {
        if (ImGui::BeginTabItem("Aimbot")) {
            renderAimbotWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("TriggerBot")) {
            renderTriggerbotWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Backtrack")) {
            renderBacktrackWindow(true);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderPopupMenu(bool contentOnly) noexcept
{

     
        ImGui::SetNextWindowSize({ 100.0f, 0.0f });
        ImGui::Begin("BRCheats", nullptr, windowFlags);
        ImGui::TextUnformatted("BRCheats");
        ImGui::SameLine();
        CenterText();
        if (ImGui::Button("x")) {
            if (config->style.closeMsg == false) {
                ImGui::OpenPopup("Aviso");
            }
            else {
                CloseMenu();
            }
        }
        ImGui::PushItemWidth(80.0f);
        if (ImGui::Button("Aimbot", { 80.0f, 20.0f })) {
            window.aimbot = !window.aimbot;
        }
        if (ImGui::Button("Autoshot", { 80.0f, 20.0f })) {
            window.triggerbot = !window.triggerbot;
        }
        if (ImGui::Button("Backtrack", { 80.0f, 20.0f })) {
            window.backtrack = !window.backtrack;
        }
        if (ImGui::Button("Contornos", { 80.0f, 20.0f })) {
            window.glow = !window.glow;
        }
        if (ImGui::Button("ESP", { 80.0f, 20.0f })) {
            window.esp = !window.esp;
        }
        if (ImGui::Button("Chams", { 80.0f, 20.0f })) {
            window.chams = !window.chams;
        }
        if (ImGui::Button("Visuais", { 80.0f, 20.0f })) {
            window.visuals = !window.visuals;
        }
        if (ImGui::Button("SkinChanger", { 80.0f, 20.0f })) {
            window.skinChanger = !window.skinChanger;
        }
        if (ImGui::Button("Outros", { 80.0f, 20.0f })) {
            window.misc = !window.misc;
        }
      //  if (ImGui::Button(" Anti Aim  ")) {
      //      window.antiAim = !window.antiAim;
     //   }
        if (ImGui::Button("Config", { 80.0f, 20.0f })) {
            window.config = !window.config;
        }
        ImGui::PopItemWidth();
}

void GUI::renderGuiStyle2() noexcept
{
    ImGui::SetNextWindowSize({ 620.0f, 0.0f });
    ImGui::Begin("BRCheats.org Counter-Strike Multihack", nullptr, windowFlags);
    ImGui::TextUnformatted("BRCheats.org Counter-Strike Multihack");
    ImGui::SameLine();
    CenterText();
    if (ImGui::Button("x")) {
        if (config->style.closeMsg == false) {
            ImGui::OpenPopup("Aviso");
        }
        else {
            CloseMenu();
            }
    }
    if (ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_NoTooltip)) {
        if (ImGui::BeginTabItem("AimHacks")) {
            renderaimhacksWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Wallhacks")) {
            renderwallhacksWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Visuais")) {
            renderVisualsWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("SkinChanger")) {
            renderSkinChangerWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Outros")) {
            renderotherhacksWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Reportbot")) {
            renderReportbotWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Estilo")) {
            renderStyleWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Config")) {
            renderConfigWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("AutoConfig")) {
            renderAutoConfigWindow(true);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    bool open = true;
    if (ImGui::BeginPopupModal("Aviso", &open, windowFlags))
    {
        auto menuKey = interfaces->inputSystem->virtualKeyToString(config->misc.menuKey);
        ImGui::Text("Para abrir o menu novamente, pressione INSERT");

        if (ImGui::Button("Fechar")) {
            config->style.closeMsg = true;
            gui->open = !gui->open;
            if (!gui->open) {
                // ImGui::GetIO().MouseDown[0] = false;
                interfaces->inputSystem->resetInputState();
                ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }
        }

    }

    ImGui::End();
}
