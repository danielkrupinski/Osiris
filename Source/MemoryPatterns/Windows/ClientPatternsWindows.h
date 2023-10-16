#pragma once

#include <GlobalContext.h>
#include <MemoryPatterns/ClientPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CPanel2D** ClientPatterns::hudScope() noexcept
{
    return GlobalContext::instance().clientPatternFinder("48 8B 05 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 44 38 60"_pat).add(3).abs().as<cs2::CPanel2D**>();
}

inline cs2::CUIEngine** ClientPatterns::uiEngine() noexcept
{
    return GlobalContext::instance().clientPatternFinder("74 25 48 8B 0D ? ? ? ? 4C 8B C2"_pat).add(5).abs().as<cs2::CUIEngine**>();
}

inline cs2::CPanel2D* ClientPatterns::mainMenuPanel() noexcept
{
    return GlobalContext::instance().clientPatternFinder("48 89 ? ? ? ? ? 4C 89 ? ? ? ? ? 48 89 ? ? ? ? ? 48 8B"_pat).add(17).abs().deref().as<cs2::CPanel2D*>();
}

inline cs2::CPanel2D** ClientPatterns::hudPanel() noexcept
{
    return GlobalContext::instance().clientPatternFinder("89 ? ? ? ? ? C6 ? ? ? ? ? ? 48 89 ? ? ? ? ? E8 ? ? ? ? 48"_pat).add(16).abs().as<cs2::CPanel2D**>();
}

inline cs2::CLoopModeGame** ClientPatterns::loopModeGame() noexcept
{
    return GlobalContext::instance().clientPatternFinder("48 89 35 ? ? ? ? 49 8B 73"_pat).add(3).abs().as<cs2::CLoopModeGame**>();
}

inline cs2::GlobalVars** ClientPatterns::globalVars() noexcept
{
    return GlobalContext::instance().clientPatternFinder("48 8B 05 ? ? ? ? 0F 57 C0 8B 48 40"_pat).add(3).abs().as<cs2::GlobalVars**>();
}

inline cs2::CUtlVector<cs2::CPlantedC4*>* ClientPatterns::plantedC4s() noexcept
{
    return GlobalContext::instance().clientPatternFinder("0F ? ? ? ? ? 39 35 ? ? ? ? 7E ? 48 8B"_pat).add(8).abs().as<cs2::CUtlVector<cs2::CPlantedC4*>*>();
}

inline cs2::ClientModeCSNormal* ClientPatterns::clientMode() noexcept
{
    return GlobalContext::instance().clientPatternFinder("48 8D 3D ? ? ? ? 48 8D 35 ? ? ? ? 90"_pat).add(3).abs().as<cs2::ClientModeCSNormal*>();
}

inline ReturnAddress ClientPatterns::getWorldSessionInClientMode() noexcept
{
    return GlobalContext::instance().clientPatternFinder("FF 10 ? 8B F0 48 85 C0 0F 84"_pat).add(2).as<ReturnAddress>();
}

inline cs2::C_CSGameRules** ClientPatterns::gameRules() noexcept
{
    return GlobalContext::instance().clientPatternFinder("48 8B 0D ? ? ? ? 48 85 C9 0F 84 ? ? ? ? 80 BE"_pat).add(3).abs().as<cs2::C_CSGameRules**>();
}

inline const void* ClientPatterns::transformTranslate3dVMT() noexcept
{
    return GlobalContext::instance().clientPatternFinder("48 8D 05 ? ? ? ? 48 C7 42 ? 00"_pat).add(3).abs().as<const void*>();
}

inline const void* ClientPatterns::transformScale3dVMT() noexcept
{
    return GlobalContext::instance().clientPatternFinder("48 8D 0D ? ? ? ? F3 0F 10 4B ? F3 0F 10 43"_pat).add(3).abs().as<const void*>();
}

inline cs2::VMatrix* ClientPatterns::worldToProjectionMatrix() noexcept
{
    return GlobalContext::instance().clientPatternFinder("48 8D 05 ? ? ? ? 48 8B D3 4C 8D 05"_pat).add(3).abs().as<cs2::VMatrix*>();
}

inline cs2::CViewRender** ClientPatterns::viewRender() noexcept
{
    return GlobalContext::instance().clientPatternFinder("48 89 05 ? ? ? ? 48 8B C8 48 85 C0"_pat).add(3).abs().as<cs2::CViewRender**>();
}
