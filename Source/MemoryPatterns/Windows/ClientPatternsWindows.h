#pragma once

#include <CS2/Classes/ClientModeCSNormal.h>
#include <CS2/Classes/CPlantedC4.h>
#include <CS2/Classes/CViewRender.h>
#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <CS2/Classes/GlobalVars.h>
#include <CS2/Classes/Panorama.h>
#include <CS2/Classes/CUtlVector.h>
#include <CS2/Classes/VMatrix.h>
#include <MemorySearch/BytePatternLiteral.h>
#include <Utils/ReturnAddress.h>

template <typename PatternFinders>
struct ClientPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::CUIEngine** uiEngine() const noexcept
    {
        return patternFinders.clientPatternFinder("48 89 78 ? 48 89 0D ? ? ? ?"_pat).add(7).abs().template as<cs2::CUIEngine**>();
    }

    [[nodiscard]] cs2::CPanel2D** mainMenuPanel() const noexcept
    {
        return patternFinders.clientPatternFinder("48 83 EC ? 48 8B 05 ? ? ? ? 48 8D 15"_pat).add(7).abs().template as<cs2::CPanel2D**>();
    }

    [[nodiscard]] cs2::CPanel2D** hudPanel() const noexcept
    {
        return patternFinders.clientPatternFinder("89 ? ? ? ? ? C6 ? ? ? ? ? ? 48 89 ? ? ? ? ? E8 ? ? ? ? 48"_pat).add(16).abs().template as<cs2::CPanel2D**>();
    }

    [[nodiscard]] cs2::CLoopModeGame** loopModeGame() const noexcept
    {
        return patternFinders.clientPatternFinder("48 89 35 ? ? ? ? 49 8B 73"_pat).add(3).abs().template as<cs2::CLoopModeGame**>();
    }

    [[nodiscard]] cs2::GlobalVars** globalVars() const noexcept
    {
        return patternFinders.clientPatternFinder("48 8B 05 ? ? ? ? 0F 57 C0 8B 48 40"_pat).add(3).abs().template as<cs2::GlobalVars**>();
    }

    [[nodiscard]] cs2::CUtlVector<cs2::CPlantedC4*>* plantedC4s() const noexcept
    {
        return patternFinders.clientPatternFinder("0F ? ? ? ? ? 39 35 ? ? ? ? 7E ? 48 8B"_pat).add(8).abs().template as<cs2::CUtlVector<cs2::CPlantedC4*>*>();
    }

    [[nodiscard]] cs2::ClientModeCSNormal* clientMode() const noexcept
    {
        return patternFinders.clientPatternFinder("48 8D 3D ? ? ? ? 48 8D 35 ? ? ? ? 90"_pat).add(3).abs().template as<cs2::ClientModeCSNormal*>();
    }

    [[nodiscard]] ReturnAddress getWorldSessionInClientMode() const noexcept
    {
        return patternFinders.clientPatternFinder("FF 10 ? 8B F0 48 85 C0 0F 84"_pat).add(2).template as<ReturnAddress>();
    }

    [[nodiscard]] cs2::C_CSGameRules** gameRules() const noexcept
    {
        return patternFinders.clientPatternFinder("48 8B 0D ? ? ? ? 48 85 C9 0F 84 ? ? ? ? 80 BE"_pat).add(3).abs().template as<cs2::C_CSGameRules**>();
    }

    [[nodiscard]] const void* transformTranslate3dVMT() const noexcept
    {
        return patternFinders.clientPatternFinder("48 8D 05 ? ? ? ? 48 C7 42 ? 00"_pat).add(3).abs().template as<const void*>();
    }

    [[nodiscard]] const void* transformScale3dVMT() const noexcept
    {
        return patternFinders.clientPatternFinder("48 8D 0D ? ? ? ? F3 0F 10 4B ? F3 0F 10 43"_pat).add(3).abs().template as<const void*>();
    }

    [[nodiscard]] cs2::VMatrix* worldToProjectionMatrix() const noexcept
    {
        return patternFinders.clientPatternFinder("48 8D 05 ? ? ? ? 48 8B D3 4C 8D 05"_pat).add(3).abs().template as<cs2::VMatrix*>();
    }

    [[nodiscard]] cs2::VMatrix* viewToProjectionMatrix() const noexcept
    {
        return patternFinders.clientPatternFinder("4C 8D 0D ? ? ? ? 48 8D 05 ? ? ? ? 48 8B D3"_pat).add(3).abs().template as<cs2::VMatrix*>();
    }

    [[nodiscard]] cs2::CViewRender** viewRender() const noexcept
    {
        return patternFinders.clientPatternFinder("48 89 05 ? ? ? ? 48 8B C8 48 85 C0"_pat).add(3).abs().template as<cs2::CViewRender**>();
    }

    [[nodiscard]] cs2::CCSPlayerController** localPlayerController() const noexcept
    {
        return patternFinders.clientPatternFinder("48 63 C1 48 8D 0D ? ? ? ? 48 8B 0C"_pat).add(6).abs().template as<cs2::CCSPlayerController**>();
    }
};
