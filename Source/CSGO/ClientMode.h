#pragma once

#include <cstdint>

#include <Platform/Macros/PlatformSpecific.h>
#include "VirtualMethod.h"

namespace csgo
{

struct HudChatPOD;

struct HudChat : GameClass<HudChat, HudChatPOD> {
    template <typename... Args>
    void printf(int filter, const char* fmt, Args... args) const noexcept
    {
        getInvoker().invokeCdecl<void>((*reinterpret_cast<std::uintptr_t**>(getThis()))[WIN32_LINUX(26, 29)], getThis(), filter, fmt, args...);
    }
};

struct ClientMode {
    PAD(WIN32_LINUX(28, 48));
    HudChatPOD* hudChat;
    PAD(WIN32_LINUX(0x47C, 0x8D0));
    float blueColorCorrection;
    PAD(WIN32_LINUX(4, 12));
    float redColorCorrection;
    PAD(WIN32_LINUX(4, 12));
    float monoColorCorrection;
    PAD(WIN32_LINUX(4, 12));
    float saturationColorCorrection;
    PAD(WIN32_LINUX(12, 28));
    float ghostColorCorrection;
    PAD(WIN32_LINUX(4, 12));
    float greenColorCorrection;
    PAD(WIN32_LINUX(4, 12));
    float yellowColorCorrection;
};

}
