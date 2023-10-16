#pragma once

#include <GlobalContext.h>
#include <MemoryPatterns/ClientPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CPanel2D** ClientPatterns::hudScope() noexcept
{
    return GlobalContext::instance().clientPatternFinder("75 19 48 8D 05 ? ? ? ? 48 8B 00"_pat).add(5).abs().as<cs2::CPanel2D**>();
}

inline cs2::CUIEngine** ClientPatterns::uiEngine() noexcept
{
    return GlobalContext::instance().clientPatternFinder("48 89 E5 48 89 3D ? ? ? ? E8"_pat).add(6).abs().as<cs2::CUIEngine**>();
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
    return GlobalContext::instance().clientPatternFinder("4C 8D 35 ? ? ? ? 49 8B 3E 48 85 FF 0F 84 ? ? ? ? 41 80 BC 24"_pat).add(3).abs().as<cs2::C_CSGameRules**>();
}

inline const void* ClientPatterns::transformTranslate3dVMT() noexcept
{
    return GlobalContext::instance().clientPatternFinder("48 8D 0D ? ? ? ? 48 89 08 48 89 50 08 48 8B 53 10"_pat).add(3).abs().as<const void*>();
}

inline const void* ClientPatterns::transformScale3dVMT() noexcept
{
    return GlobalContext::instance().clientPatternFinder("48 8B 53 08 48 8D 0D ? ? ? ? F3 0F 10 43"_pat).add(7).abs().as<const void*>();
}

inline cs2::VMatrix* ClientPatterns::worldToProjectionMatrix() noexcept
{
    return GlobalContext::instance().clientPatternFinder("4C 8D 05 ? ? ? ? 48 8B 38 48 8D 0D"_pat).add(3).abs().as<cs2::VMatrix*>();
}

inline cs2::CViewRender** ClientPatterns::viewRender() noexcept
{
    return GlobalContext::instance().clientPatternFinder("48 8D 05 ? ? ? ? 48 89 38"_pat).add(3).abs().as<cs2::CViewRender**>();
}
