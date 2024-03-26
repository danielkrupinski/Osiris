#pragma once

#include <MemoryPatterns/ClientPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CUIEngine** ClientPatterns::uiEngine() const noexcept
{
    return clientPatternFinder("48 89 E5 48 89 3D ? ? ? ? E8"_pat).add(6).abs().as<cs2::CUIEngine**>();
}

inline cs2::CPanel2D** ClientPatterns::mainMenuPanel() const noexcept
{
    return clientPatternFinder("83 FB ? 75 ? 48 8D 05 ? ? ? ? 48 8B 00"_pat).add(8).abs().as<cs2::CPanel2D**>();
}

inline cs2::CPanel2D** ClientPatterns::hudPanel() const noexcept
{
    return clientPatternFinder("41 C6 84 24 ? ? ? ? ? 4C 89 25 ? ? ? ? E8"_pat).add(12).abs().as<cs2::CPanel2D**>();
}

inline cs2::CLoopModeGame** ClientPatterns::loopModeGame() const noexcept
{
    return clientPatternFinder("48 89 1D ? ? ? ? 48 8D 65 F0"_pat).add(3).abs().as<cs2::CLoopModeGame**>();
}

inline cs2::GlobalVars** ClientPatterns::globalVars() const noexcept
{
    return clientPatternFinder("48 89 35 ? ? ? ? 48 89 46 20"_pat).add(3).abs().as<cs2::GlobalVars**>();
}

inline cs2::CUtlVector<cs2::CPlantedC4*>* ClientPatterns::plantedC4s() const noexcept
{
    return clientPatternFinder("0F 85 ? ? ? ? 48 8D 05 ? ? ? ? 8B 10"_pat).add(9).abs().as<cs2::CUtlVector<cs2::CPlantedC4*>*>();
}

inline cs2::ClientModeCSNormal* ClientPatterns::clientMode() const noexcept
{
    return clientPatternFinder("48 8D 05 ? ? ? ? 4C 89 E7 48 89 05"_pat).add(3).abs().as<cs2::ClientModeCSNormal*>();
}

inline ReturnAddress ClientPatterns::getWorldSessionInClientMode() const noexcept
{
    return clientPatternFinder("FF 10 49 89 C5 48 85 C0 0F 84"_pat).add(2).as<ReturnAddress>();
}

inline cs2::C_CSGameRules** ClientPatterns::gameRules() const noexcept
{
    return clientPatternFinder("4C 8D 35 ? ? ? ? 49 8B 3E 48 85 FF 0F 84 ? ? ? ? 41 80 BC 24"_pat).add(3).abs().as<cs2::C_CSGameRules**>();
}

inline const void* ClientPatterns::transformTranslate3dVMT() const noexcept
{
    return clientPatternFinder("48 8D 0D ? ? ? ? 48 89 08 48 89 50 08 48 8B 53 10"_pat).add(3).abs().as<const void*>();
}

inline const void* ClientPatterns::transformScale3dVMT() const noexcept
{
    return clientPatternFinder("48 8B 53 08 48 8D 0D ? ? ? ? F3 0F 10 43"_pat).add(7).abs().as<const void*>();
}

inline cs2::VMatrix* ClientPatterns::worldToProjectionMatrix() const noexcept
{
    return clientPatternFinder("4C 8D 05 ? ? ? ? 48 8B 38 48 8D 0D"_pat).add(3).abs().as<cs2::VMatrix*>();
}

inline cs2::VMatrix* ClientPatterns::viewToProjectionMatrix() const noexcept
{
    return clientPatternFinder("48 8B 38 48 8D 0D ? ? ? ? E8"_pat).add(6).abs().as<cs2::VMatrix*>();
}

inline cs2::CViewRender** ClientPatterns::viewRender() const noexcept
{
    return clientPatternFinder("48 8D 05 ? ? ? ? 48 89 38"_pat).add(3).abs().as<cs2::CViewRender**>();
}

inline cs2::CCSPlayerController** ClientPatterns::localPlayerController() const noexcept
{
    return clientPatternFinder("48 8D 05 ? ? ? ? 48 63 FF 48 8B 04 F8 48"_pat).add(3).abs().as<cs2::CCSPlayerController**>();
}
