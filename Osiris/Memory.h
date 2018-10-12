#pragma once
#include <string>

#include "SDK/BaseEntity.h"
#include "SDK/ClientMode.h"
#include "SDK/GlobalVars.h"

class C_LocalPlayer
{
    friend bool operator==(const C_LocalPlayer& lhs, void* rhs);
public:
    C_LocalPlayer() : m_local(nullptr) {}

    operator bool() const { return *m_local != nullptr; }
    operator BaseEntity*() const { return *m_local; }

    BaseEntity* operator->() { return *m_local; }

private:
    BaseEntity** m_local;
};

class Memory final {
public:
    Memory();
    std::uintptr_t findPattern(std::string, std::string);

    std::uintptr_t present;
    std::uintptr_t reset;

    GlobalVars* globalVars;
    BaseEntity** localPlayer;
    ClientMode* clientMode;
};

extern Memory memory;
