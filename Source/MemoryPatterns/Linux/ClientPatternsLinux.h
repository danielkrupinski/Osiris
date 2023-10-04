#pragma once

#include <GlobalContext.h>
#include <MemoryPatterns/ClientPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CPanel2D** ClientPatterns::hudScope() noexcept
{
    return GlobalContext::instance().clientPatternFinder("75 19 48 8D 05 ? ? ? ? 48 8B 00"_pat).add(5).abs().as<cs2::CPanel2D**>();
}

inline cs2::CUIEngine* ClientPatterns::uiEngine() noexcept
{
    return GlobalContext::instance().clientPatternFinder("48 89 E5 48 89 3D ? ? ? ? E8"_pat).add(6).abs().deref().as<cs2::CUIEngine*>();
}

inline cs2::CPanel2D* ClientPatterns::mainMenuPanel() noexcept
{
    return GlobalContext::instance().clientPatternFinder("48 89 1D ? ? ? ? 48 89 83 ? ? ? ? 48 8B 07"_pat).add(3).abs().deref().as<cs2::CPanel2D*>();
}

inline cs2::CPanel2D** ClientPatterns::hudPanel() noexcept
{
    return GlobalContext::instance().clientPatternFinder("41 C6 84 24 ? ? ? ? ? 4C 89 25 ? ? ? ? E8"_pat).add(12).abs().as<cs2::CPanel2D**>();
}

inline cs2::CLoopModeGame** ClientPatterns::loopModeGame() noexcept
{
    return GlobalContext::instance().clientPatternFinder("48 89 1D ? ? ? ? 48 8D 65 F0"_pat).add(3).abs().as<cs2::CLoopModeGame**>();
}

inline cs2::GlobalVars** ClientPatterns::globalVars() noexcept
{
    return GlobalContext::instance().clientPatternFinder("48 89 35 ? ? ? ? 48 89 46 20"_pat).add(3).abs().as<cs2::GlobalVars**>();
}

inline cs2::CUtlVector<cs2::CPlantedC4*>* ClientPatterns::plantedC4s() noexcept
{
    return GlobalContext::instance().clientPatternFinder("0F 85 ? ? ? ? 48 8D 05 ? ? ? ? 8B 10"_pat).add(9).abs().as<cs2::CUtlVector<cs2::CPlantedC4*>*>();
}

inline cs2::ClientModeCSNormal* ClientPatterns::clientMode() noexcept
{
    return GlobalContext::instance().clientPatternFinder("48 8D 05 ? ? ? ? 4C 89 E7 48 89 05"_pat).add(3).abs().as<cs2::ClientModeCSNormal*>();
}

inline ReturnAddress ClientPatterns::getWorldSessionInClientMode() noexcept
{
    return GlobalContext::instance().clientPatternFinder("FF 10 49 89 C5 48 85 C0 0F 84"_pat).add(2).as<ReturnAddress>();
}

inline cs2::C_CSGameRules** ClientPatterns::gameRules() noexcept
{
    return GlobalContext::instance().clientPatternFinder("4C 8B 2D ? ? ? ? 4D 85 ED 74 5F"_pat).add(3).abs().as<cs2::C_CSGameRules**>();
}
