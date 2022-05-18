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
    constexpr Key(std::string_view name, int code) : name{ name }, code{ code } {  }

    std::string_view name;
    int code;
};

// indices must match KeyBind::KeyCode enum
static constexpr auto keyMap = std::to_array<Key>({
    { "'", WIN32_LINUX(VK_OEM_7, SDL_SCANCODE_APOSTROPHE) },
    { ",", WIN32_LINUX(VK_OEM_COMMA, SDL_SCANCODE_COMMA) },
    { "-", WIN32_LINUX(VK_OEM_MINUS, SDL_SCANCODE_MINUS) },
    { ".", WIN32_LINUX(VK_OEM_PERIOD, SDL_SCANCODE_PERIOD) },
    { "/", WIN32_LINUX(VK_OEM_2, SDL_SCANCODE_SLASH) },
    { "0", WIN32_LINUX('0', SDL_SCANCODE_0) },
    { "1", WIN32_LINUX('1', SDL_SCANCODE_1) },
    { "2", WIN32_LINUX('2', SDL_SCANCODE_2) },
    { "3", WIN32_LINUX('3', SDL_SCANCODE_3) },
    { "4", WIN32_LINUX('4', SDL_SCANCODE_4) },
    { "5", WIN32_LINUX('5', SDL_SCANCODE_5) },
    { "6", WIN32_LINUX('6', SDL_SCANCODE_6) },
    { "7", WIN32_LINUX('7', SDL_SCANCODE_7) },
    { "8", WIN32_LINUX('8', SDL_SCANCODE_8) },
    { "9", WIN32_LINUX('9', SDL_SCANCODE_9) },
    { ";", WIN32_LINUX(VK_OEM_1, SDL_SCANCODE_SEMICOLON) },
    { "=", WIN32_LINUX(VK_OEM_PLUS, SDL_SCANCODE_EQUALS) },
    { "A", WIN32_LINUX('A', SDL_SCANCODE_A) },
    { "ADD", WIN32_LINUX(VK_ADD, SDL_SCANCODE_KP_PLUS) },
    { "B", WIN32_LINUX('B', SDL_SCANCODE_B) },
    { "BACKSPACE", WIN32_LINUX(VK_BACK, SDL_SCANCODE_BACKSPACE) },
    { "C", WIN32_LINUX('C', SDL_SCANCODE_C) },
    { "CAPSLOCK", WIN32_LINUX(VK_CAPITAL, SDL_SCANCODE_CAPSLOCK) },
    { "D", WIN32_LINUX('D', SDL_SCANCODE_D) },
    { "DECIMAL", WIN32_LINUX(VK_DECIMAL, SDL_SCANCODE_KP_DECIMAL) },
    { "DELETE", WIN32_LINUX(VK_DELETE, SDL_SCANCODE_DELETE) },
    { "DIVIDE", WIN32_LINUX(VK_DIVIDE, SDL_SCANCODE_KP_DIVIDE) },
    { "DOWN", WIN32_LINUX(VK_DOWN, SDL_SCANCODE_DOWN) },
    { "E", WIN32_LINUX('E', SDL_SCANCODE_E) },
    { "END", WIN32_LINUX(VK_END, SDL_SCANCODE_END) },
    { "ENTER", WIN32_LINUX(VK_RETURN, SDL_SCANCODE_RETURN) },
    { "F", WIN32_LINUX('F', SDL_SCANCODE_F) },
    { "F1", WIN32_LINUX(VK_F1, SDL_SCANCODE_F1) },
    { "F10", WIN32_LINUX(VK_F10, SDL_SCANCODE_F10) },
    { "F11", WIN32_LINUX(VK_F11, SDL_SCANCODE_F11) },
    { "F12", WIN32_LINUX(VK_F12, SDL_SCANCODE_F12) },
    { "F2", WIN32_LINUX(VK_F2, SDL_SCANCODE_F2) },
    { "F3", WIN32_LINUX(VK_F3, SDL_SCANCODE_F3) },
    { "F4", WIN32_LINUX(VK_F4, SDL_SCANCODE_F4) },
    { "F5", WIN32_LINUX(VK_F5, SDL_SCANCODE_F5) },
    { "F6", WIN32_LINUX(VK_F6, SDL_SCANCODE_F6) },
    { "F7", WIN32_LINUX(VK_F7, SDL_SCANCODE_F7) },
    { "F8", WIN32_LINUX(VK_F8, SDL_SCANCODE_F8) },
    { "F9", WIN32_LINUX(VK_F9, SDL_SCANCODE_F9) },
    { "G", WIN32_LINUX('G', SDL_SCANCODE_G) },
    { "H", WIN32_LINUX('H', SDL_SCANCODE_H) },
    { "HOME", WIN32_LINUX(VK_HOME, SDL_SCANCODE_HOME) },
    { "I", WIN32_LINUX('I', SDL_SCANCODE_I) },
    { "INSERT", WIN32_LINUX(VK_INSERT, SDL_SCANCODE_INSERT) },
    { "J", WIN32_LINUX('J', SDL_SCANCODE_J) },
    { "K", WIN32_LINUX('K', SDL_SCANCODE_K) },
    { "L", WIN32_LINUX('L', SDL_SCANCODE_L) },
    { "LALT", WIN32_LINUX(VK_LMENU, SDL_SCANCODE_LALT) },
    { "LCTRL", WIN32_LINUX(VK_LCONTROL, SDL_SCANCODE_LCTRL) },
    { "LEFT", WIN32_LINUX(VK_LEFT, SDL_SCANCODE_LEFT) },
    { "LSHIFT", WIN32_LINUX(VK_LSHIFT, SDL_SCANCODE_LSHIFT) },
    { "M", WIN32_LINUX('M', SDL_SCANCODE_M) },
    { "MOUSE1", 0 },
    { "MOUSE2", 1 },
    { "MOUSE3", 2 },
    { "MOUSE4", 3 },
    { "MOUSE5", 4 },
    { "MULTIPLY", WIN32_LINUX(VK_MULTIPLY, SDL_SCANCODE_KP_MULTIPLY) },
    { "MWHEEL_DOWN", 0 },
    { "MWHEEL_UP", 0 },
    { "N", WIN32_LINUX('N', SDL_SCANCODE_N) },
    { "NONE", 0 },
    { "NUMPAD_0", WIN32_LINUX(VK_NUMPAD0, SDL_SCANCODE_KP_0) },
    { "NUMPAD_1", WIN32_LINUX(VK_NUMPAD1, SDL_SCANCODE_KP_1) },
    { "NUMPAD_2", WIN32_LINUX(VK_NUMPAD2, SDL_SCANCODE_KP_2) },
    { "NUMPAD_3", WIN32_LINUX(VK_NUMPAD3, SDL_SCANCODE_KP_3) },
    { "NUMPAD_4", WIN32_LINUX(VK_NUMPAD4, SDL_SCANCODE_KP_4) },
    { "NUMPAD_5", WIN32_LINUX(VK_NUMPAD5, SDL_SCANCODE_KP_5) },
    { "NUMPAD_6", WIN32_LINUX(VK_NUMPAD6, SDL_SCANCODE_KP_6) },
    { "NUMPAD_7", WIN32_LINUX(VK_NUMPAD7, SDL_SCANCODE_KP_7) },
    { "NUMPAD_8", WIN32_LINUX(VK_NUMPAD8, SDL_SCANCODE_KP_8) },
    { "NUMPAD_9", WIN32_LINUX(VK_NUMPAD9, SDL_SCANCODE_KP_9) },
    { "O", WIN32_LINUX('O', SDL_SCANCODE_O) },
    { "P", WIN32_LINUX('P', SDL_SCANCODE_P) },
    { "PAGE_DOWN", WIN32_LINUX(VK_NEXT, SDL_SCANCODE_PAGEDOWN) },
    { "PAGE_UP", WIN32_LINUX(VK_PRIOR, SDL_SCANCODE_PAGEUP) },
    { "Q", WIN32_LINUX('Q', SDL_SCANCODE_Q) },
    { "R", WIN32_LINUX('R', SDL_SCANCODE_R) },
    { "RALT", WIN32_LINUX(VK_RMENU, SDL_SCANCODE_RALT) },
    { "RCTRL", WIN32_LINUX(VK_RCONTROL, SDL_SCANCODE_RCTRL) },
    { "RIGHT", WIN32_LINUX(VK_RIGHT, SDL_SCANCODE_RIGHT) },
    { "RSHIFT", WIN32_LINUX(VK_RSHIFT, SDL_SCANCODE_RSHIFT) },
    { "S", WIN32_LINUX('S', SDL_SCANCODE_S) },
    { "SPACE", WIN32_LINUX(VK_SPACE, SDL_SCANCODE_SPACE) },
    { "SUBTRACT", WIN32_LINUX(VK_SUBTRACT, SDL_SCANCODE_KP_MINUS) },
    { "T", WIN32_LINUX('T', SDL_SCANCODE_T) },
    { "TAB", WIN32_LINUX(VK_TAB, SDL_SCANCODE_TAB) },
    { "U", WIN32_LINUX('U', SDL_SCANCODE_U) },
    { "UP", WIN32_LINUX(VK_UP, SDL_SCANCODE_UP) },
    { "V", WIN32_LINUX('V', SDL_SCANCODE_V) },
    { "W", WIN32_LINUX('W', SDL_SCANCODE_W) },
    { "X", WIN32_LINUX('X', SDL_SCANCODE_X) },
    { "Y", WIN32_LINUX('Y', SDL_SCANCODE_Y) },
    { "Z", WIN32_LINUX('Z', SDL_SCANCODE_X) },
    { "[", WIN32_LINUX(VK_OEM_4, SDL_SCANCODE_LEFTBRACKET) },
    { "\\", WIN32_LINUX(VK_OEM_5, SDL_SCANCODE_BACKSLASH) },
    { "]", WIN32_LINUX(VK_OEM_6, SDL_SCANCODE_RIGHTBRACKET) },
    { "`", WIN32_LINUX(VK_OEM_3, SDL_SCANCODE_GRAVE) }
});

static_assert(keyMap.size() == KeyBind::MAX);
static_assert(std::ranges::is_sorted(keyMap, {}, &Key::name));

KeyBind::KeyBind(KeyCode keyCode) noexcept : keyCode{ static_cast<std::size_t>(keyCode) < keyMap.size() ? keyCode : KeyCode::NONE } {}

KeyBind::KeyBind(const char* keyName) noexcept
{
    if (const auto it = std::ranges::lower_bound(keyMap, keyName, {}, &Key::name); it != keyMap.end() && it->name == keyName)
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

bool KeyBind::setToPressedKey() noexcept
{
    if (ImGui::IsKeyPressed(ImGui::GetIO().KeyMap[ImGuiKey_Escape])) {
        keyCode = KeyCode::NONE;
        return true;
    } else if (ImGui::GetIO().MouseWheel < 0.0f) {
        keyCode = KeyCode::MOUSEWHEEL_DOWN;
        return true;
    } else if (ImGui::GetIO().MouseWheel > 0.0f) {
        keyCode = KeyCode::MOUSEWHEEL_UP;
        return true;
    } 
    
    for (int i = 0; i < IM_ARRAYSIZE(ImGui::GetIO().MouseDown); ++i) {
        if (ImGui::IsMouseClicked(i)) {
            keyCode = KeyCode(KeyCode::MOUSE1 + i);
            return true;
        }
    }

    for (int i = 0; i < IM_ARRAYSIZE(ImGui::GetIO().KeysDown); ++i) {
        if (!ImGui::IsKeyPressed(i))
            continue;

        if (const auto it = std::ranges::find(keyMap, i, &Key::code); it != keyMap.end()) {
            keyCode = static_cast<KeyCode>(std::distance(keyMap.begin(), it));
            // Treat AltGr as RALT
            if (keyCode == KeyCode::LCTRL && ImGui::IsKeyPressed(keyMap[KeyCode::RALT].code))
                keyCode = KeyCode::RALT;
            return true;
        }
    }
    return false;
}

void KeyBindToggle::handleToggle() noexcept
{
    if (isPressed())
        toggledOn = !toggledOn;
}
