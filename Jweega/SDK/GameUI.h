#pragma once

#include "Utils.h"

class GameUI {
public:
    constexpr void messageBox(const char* title, const char* text) noexcept
    {
        callVirtualMethod<void, const char*, const char*, bool, bool, const char*, const char*, const char*, const char*, void*>(this, 20, title, text, true, false, nullptr, nullptr, nullptr, nullptr, nullptr);
    }

    void messageBox(const wchar_t* title, const wchar_t* text) noexcept
    {
        const int titleLength{ WideCharToMultiByte(CP_UTF8, 0, title, -1, nullptr, 0, nullptr, nullptr) };
        const int textLength{ WideCharToMultiByte(CP_UTF8, 0, text, -1, nullptr, 0, nullptr, nullptr) };
        if (std::string wideTitle(titleLength, 0); WideCharToMultiByte(CP_UTF8, 0, title, -1, &wideTitle[0], titleLength, nullptr, nullptr))
            if (std::string wideText(textLength, 0); WideCharToMultiByte(CP_UTF8, 0, text, -1, &wideText[0], textLength, nullptr, nullptr))
                callVirtualMethod<void, const char*, const char*, bool, bool, const char*, const char*, const char*, const char*, void*>(this, 20, wideTitle.c_str(), wideText.c_str(), true, false, nullptr, nullptr, nullptr, nullptr, nullptr);
    }
};