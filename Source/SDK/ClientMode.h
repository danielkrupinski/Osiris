#pragma once

#include <cstdint>

#include "Inconstructible.h"
#include <Platform/CallingConventions.h>
#include <Platform/PlatformSpecific.h>

class HudChat {
public:
    INCONSTRUCTIBLE(HudChat)

    template <typename... Args>
    void printf(int filter, const char* fmt, Args... args) noexcept
    {
        (*reinterpret_cast<void(CDECL_CONV***)(void*, int, const char*, ...)>(this))[WIN32_LINUX(26, 29)](this, filter, fmt, args...);
    }
};

struct ClientMode {
    PAD(WIN32_LINUX(28, 48))
    HudChat* hudChat;
    PAD(WIN32_LINUX(0x47C, 0x8D0))
    float blueColorCorrection;
    PAD(WIN32_LINUX(4, 12))
    float redColorCorrection;
    PAD(WIN32_LINUX(4, 12))
    float monoColorCorrection;
    PAD(WIN32_LINUX(4, 12))
    float saturationColorCorrection;
    PAD(WIN32_LINUX(12, 28))
    float ghostColorCorrection;
    PAD(WIN32_LINUX(4, 12))
    float greenColorCorrection;
    PAD(WIN32_LINUX(4, 12))
    float yellowColorCorrection;
};
