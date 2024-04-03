#pragma once

#include <MemoryPatterns/ClientPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CUIEngine** ClientPatterns::uiEngine() const noexcept
{
    return clientPatternFinder("48 89 78 ? 48 89 0D ? ? ? ?"_pat).add(7).abs().as<cs2::CUIEngine**>();
}

inline cs2::CPanel2D** ClientPatterns::mainMenuPanel() const noexcept
{
    return clientPatternFinder("48 83 EC ? 48 8B 05 ? ? ? ? 48 8D 15"_pat).add(7).abs().as<cs2::CPanel2D**>();
}

inline cs2::CPanel2D** ClientPatterns::hudPanel() const noexcept
{
    return clientPatternFinder("89 ? ? ? ? ? C6 ? ? ? ? ? ? 48 89 ? ? ? ? ? E8 ? ? ? ? 48"_pat).add(16).abs().as<cs2::CPanel2D**>();
}

inline cs2::CLoopModeGame** ClientPatterns::loopModeGame() const noexcept
{
    return clientPatternFinder("48 89 35 ? ? ? ? 49 8B 73"_pat).add(3).abs().as<cs2::CLoopModeGame**>();
}

inline cs2::GlobalVars** ClientPatterns::globalVars() const noexcept
{
    return clientPatternFinder("48 8B 05 ? ? ? ? 0F 57 C0 8B 48 40"_pat).add(3).abs().as<cs2::GlobalVars**>();
}

inline cs2::CUtlVector<cs2::CPlantedC4*>* ClientPatterns::plantedC4s() const noexcept
{
    return clientPatternFinder("0F ? ? ? ? ? 39 35 ? ? ? ? 7E ? 48 8B"_pat).add(8).abs().as<cs2::CUtlVector<cs2::CPlantedC4*>*>();
}

inline cs2::ClientModeCSNormal* ClientPatterns::clientMode() const noexcept
{
    return clientPatternFinder("48 8D 3D ? ? ? ? 48 8D 35 ? ? ? ? 90"_pat).add(3).abs().as<cs2::ClientModeCSNormal*>();
}

inline ReturnAddress ClientPatterns::getWorldSessionInClientMode() const noexcept
{
    return clientPatternFinder("FF 10 ? 8B F0 48 85 C0 0F 84"_pat).add(2).as<ReturnAddress>();
}

inline cs2::C_CSGameRules** ClientPatterns::gameRules() const noexcept
{
    return clientPatternFinder("48 8B 0D ? ? ? ? 48 85 C9 0F 84 ? ? ? ? 80 BE"_pat).add(3).abs().as<cs2::C_CSGameRules**>();
}

inline const void* ClientPatterns::transformTranslate3dVMT() const noexcept
{
    return clientPatternFinder("48 8D 05 ? ? ? ? 48 C7 42 ? 00"_pat).add(3).abs().as<const void*>();
}

inline const void* ClientPatterns::transformScale3dVMT() const noexcept
{
    return clientPatternFinder("48 8D 0D ? ? ? ? F3 0F 10 4B ? F3 0F 10 43"_pat).add(3).abs().as<const void*>();
}

inline cs2::VMatrix* ClientPatterns::worldToProjectionMatrix() const noexcept
{
    return clientPatternFinder("48 8D 05 ? ? ? ? 48 8B D3 4C 8D 05"_pat).add(3).abs().as<cs2::VMatrix*>();
}

inline cs2::VMatrix* ClientPatterns::viewToProjectionMatrix() const noexcept
{
    return clientPatternFinder("4C 8D 0D ? ? ? ? 48 8D 05 ? ? ? ? 48 8B D3"_pat).add(3).abs().as<cs2::VMatrix*>();
}

inline cs2::CViewRender** ClientPatterns::viewRender() const noexcept
{
    return clientPatternFinder("48 89 05 ? ? ? ? 48 8B C8 48 85 C0"_pat).add(3).abs().as<cs2::CViewRender**>();
}

inline cs2::CCSPlayerController** ClientPatterns::localPlayerController() const noexcept
{
    return clientPatternFinder("48 63 C1 48 8D 0D ? ? ? ? 48 8B 0C"_pat).add(6).abs().as<cs2::CCSPlayerController**>();
}
