#include <algorithm>
#include <array>
#include <string_view>

#ifdef _WIN32
#include <Windows.h>
#else
#include <SDL2/SDL.h>
#endif

#include "imgui/imgui.h"

#include "InputUtil.h"
#include "SDK/Platform.h"

struct Key {
    template <std::size_t N>
    constexpr Key(const char(&name)[N], int code) : name{ name }, code{ code } {  }

    std::string_view name;
    int code;
};

// must match order of KeyBind::KeyCode enum, and has to be sorted alphabetically
constexpr auto keyMap = std::to_array<Key>({
    { "A", WIN32_LINUX('A', SDL_SCANCODE_A) },
    { "B", WIN32_LINUX('B', SDL_SCANCODE_B) },
    { "C", WIN32_LINUX('C', SDL_SCANCODE_C) },
    { "INSERT", WIN32_LINUX(VK_INSERT, SDL_SCANCODE_INSERT) },
    { "MOUSE1", 0 },
    { "MOUSE2", 1 },
    { "MOUSE3", 2 },
    { "MOUSE4", 3 },
    { "MOUSE5", 4 },
    { "MOUSEWHEEL_DOWN", 0 },
    { "MOUSEWHEEL_UP", 0 },
    { "NONE", 0 }
});

KeyBind::KeyBind(KeyCode keyCode) noexcept
{
    this->keyCode = static_cast<std::size_t>(keyCode) < keyMap.size() ? keyCode : KeyCode::NONE;
}

KeyBind::KeyBind(const char* keyName) noexcept
{
    auto it = std::lower_bound(keyMap.begin(), keyMap.end(), keyName, [](const Key& key, const char* keyName) { return key.name < keyName; });
    if (it != keyMap.end() && it->name == keyName)
        keyCode = static_cast<KeyCode>(std::distance(keyMap.begin(), it));
    else
        keyCode = KeyCode::NONE;
}

const char* KeyBind::toString() const noexcept
{
    return keyMap[static_cast<std::size_t>(keyCode) < keyMap.size() ? keyCode : KeyCode::NONE].name.data();
}

bool KeyBind::isPressed() const noexcept
{
    if (keyCode == KeyCode::NONE)
        return false;

    if (keyCode == KeyCode::MOUSEWHEEL_DOWN)
        return ImGui::GetIO().MouseWheel < 0.0f;

    if (keyCode == KeyCode::MOUSEWHEEL_UP)
        return ImGui::GetIO().MouseWheel > 0.0f;

    if (keyCode >= KeyCode::MOUSE1 && keyCode <= KeyCode::MOUSE5)
        return ImGui::IsMouseClicked(keyMap[keyCode].code);

    return static_cast<std::size_t>(keyCode) < keyMap.size() && ImGui::IsKeyPressed(keyMap[keyCode].code, false);
}

bool KeyBind::isDown() const noexcept
{
    if (keyCode == KeyCode::NONE)
        return false;

    if (keyCode == KeyCode::MOUSEWHEEL_DOWN)
        return ImGui::GetIO().MouseWheel < 0.0f;

    if (keyCode == KeyCode::MOUSEWHEEL_UP)
        return ImGui::GetIO().MouseWheel > 0.0f;

    if (keyCode >= KeyCode::MOUSE1 && keyCode <= KeyCode::MOUSE5)
        return ImGui::IsMouseDown(keyMap[keyCode].code);

    return static_cast<std::size_t>(keyCode) < keyMap.size() && ImGui::IsKeyDown(keyMap[keyCode].code);
}

void KeyBind::setToPressedKey() noexcept
{
    if (ImGui::IsKeyPressed(ImGui::GetIO().KeyMap[ImGuiKey_Escape])) {
        keyCode = KeyCode::NONE;
    } else if (ImGui::GetIO().MouseWheel < 0.0f) {
        keyCode = KeyCode::MOUSEWHEEL_DOWN;
    } else if (ImGui::GetIO().MouseWheel > 0.0f) {
        keyCode = KeyCode::MOUSEWHEEL_UP;
    } else {
        for (int i = 0; i < IM_ARRAYSIZE(ImGui::GetIO().MouseDown); ++i) {
            if (ImGui::IsMouseClicked(i)) {
                keyCode = KeyCode(KeyCode::MOUSE1 + i);
                return;
            }
        }

        for (int i = 0; i < IM_ARRAYSIZE(ImGui::GetIO().KeysDown); ++i) {
            if (ImGui::IsKeyPressed(i)) {
                auto it = std::find_if(keyMap.begin(), keyMap.end(), [i](const Key& key) { return key.code == i; });
                if (it != keyMap.end()) {
                    keyCode = static_cast<KeyCode>(std::distance(keyMap.begin(), it));
                    return;
                }
            }
        }
    }
}
