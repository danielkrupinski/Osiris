#pragma once
#include <string>
#include <vector>

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

class GUI final {
public:
    void render() noexcept;
    bool isOpen{ false };
private:
    static void hotkey(int&) noexcept;
    void renderMenuBar() noexcept;
    void renderAimbotWindow() noexcept;
    void renderTriggerbotWindow() noexcept;
    void renderGlowWindow() noexcept;
    void renderChamsWindow() noexcept;
    void renderVisualsWindow() noexcept;
    void renderKnifeChangerWindow() noexcept;
    void renderMiscWindow() noexcept;
    void renderConfigWindow() noexcept;
    struct {
        bool aimbot{ false };
        bool triggerbot{ false };
        bool glow{ false };
        bool chams{ false };
        bool visuals{ false };
        bool knifeChanger{ false };
        bool misc{ false };
        bool config{ false };
    } window;

    std::string GetNewConfigName(std::string name = "New Config", int i = 0) noexcept;
};

extern GUI gui;


namespace ImGui {
    static auto vector_getter = [](void* vec, int idx, const char** out_text) {
        auto& vector = *static_cast<std::vector<std::string>*>(vec);
        if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
        *out_text = vector.at(idx).c_str();
        return true;
    };

    inline bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values, int height_in_items = -1) {
        if (values.empty()) { return false; }
        return ListBox(label, currIndex, vector_getter,
            static_cast<void*>(&values), values.size(), height_in_items);
    }

    static void ButtonDisabled(const char* label) {
        SetCursorPosX(ImGui::GetCursorPosX() + 9);
        SetCursorPosY(ImGui::GetCursorPosY() + 4);
        TextDisabled(label);
        SetCursorPosY(ImGui::GetCursorPosY() + 5);
    }

    static void InputTextDisabled(const char* label) {
        SetCursorPosX(ImGui::GetCursorPosX() + 4);
        SetCursorPosY(ImGui::GetCursorPosY() + 3);
        TextDisabled(label);
        SetCursorPosY(ImGui::GetCursorPosY() + 4);
    }
}