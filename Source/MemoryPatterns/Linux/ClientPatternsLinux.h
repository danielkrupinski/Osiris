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
        return patternFinders.clientPatternFinder("48 89 E5 48 89 3D ? ? ? ? E8"_pat).add(6).abs().template as<cs2::CUIEngine**>();
    }

    [[nodiscard]] cs2::CPanel2D** mainMenuPanel() const noexcept
    {
        return patternFinders.clientPatternFinder("83 FB ? 75 ? 48 8D 05 ? ? ? ? 48 8B 00"_pat).add(8).abs().template as<cs2::CPanel2D**>();
    }

    [[nodiscard]] cs2::CPanel2D** hudPanel() const noexcept
    {
        return patternFinders.clientPatternFinder("41 C6 84 24 ? ? ? ? ? 4C 89 25 ? ? ? ? E8"_pat).add(12).abs().template as<cs2::CPanel2D**>();
    }

    [[nodiscard]] cs2::CLoopModeGame** loopModeGame() const noexcept
    {
        return patternFinders.clientPatternFinder("48 89 1D ? ? ? ? 48 8D 65 F0"_pat).add(3).abs().template as<cs2::CLoopModeGame**>();
    }

    [[nodiscard]] cs2::GlobalVars** globalVars() const noexcept
    {
        return patternFinders.clientPatternFinder("48 89 35 ? ? ? ? 48 89 46 20"_pat).add(3).abs().template as<cs2::GlobalVars**>();
    }

    [[nodiscard]] cs2::CUtlVector<cs2::CPlantedC4*>* plantedC4s() const noexcept
    {
        return patternFinders.clientPatternFinder("0F 85 ? ? ? ? 48 8D 05 ? ? ? ? 8B 10"_pat).add(9).abs().template as<cs2::CUtlVector<cs2::CPlantedC4*>*>();
    }

    [[nodiscard]] cs2::ClientModeCSNormal* clientMode() const noexcept
    {
        return patternFinders.clientPatternFinder("48 8D 05 ? ? ? ? 4C 89 E7 48 89 05"_pat).add(3).abs().template as<cs2::ClientModeCSNormal*>();
    }

    [[nodiscard]] ReturnAddress getWorldSessionInClientMode() const noexcept
    {
        return patternFinders.clientPatternFinder("FF 10 49 89 C5 48 85 C0 0F 84"_pat).add(2).template as<ReturnAddress>();
    }

    [[nodiscard]] cs2::C_CSGameRules** gameRules() const noexcept
    {
        return patternFinders.clientPatternFinder("4C 8D 35 ? ? ? ? 49 8B 3E 48 85 FF 0F 84 ? ? ? ? 41 80 BC 24"_pat).add(3).abs().template as<cs2::C_CSGameRules**>();
    }

    [[nodiscard]] const void* transformTranslate3dVMT() const noexcept
    {
        return patternFinders.clientPatternFinder("48 8D 0D ? ? ? ? 48 89 08 48 89 50 08 48 8B 53 10"_pat).add(3).abs().template as<const void*>();
    }

    [[nodiscard]] const void* transformScale3dVMT() const noexcept
    {
        return patternFinders.clientPatternFinder("48 8B 53 08 48 8D 0D ? ? ? ? F3 0F 10 43"_pat).add(7).abs().template as<const void*>();
    }

    [[nodiscard]] cs2::VMatrix* worldToProjectionMatrix() const noexcept
    {
        return patternFinders.clientPatternFinder("4C 8D 05 ? ? ? ? 48 8B 38 48 8D 0D"_pat).add(3).abs().template as<cs2::VMatrix*>();
    }

    [[nodiscard]] cs2::VMatrix* viewToProjectionMatrix() const noexcept
    {
        return patternFinders.clientPatternFinder("48 8B 38 48 8D 0D ? ? ? ? E8"_pat).add(6).abs().template as<cs2::VMatrix*>();
    }

    [[nodiscard]] cs2::CViewRender** viewRender() const noexcept
    {
        return patternFinders.clientPatternFinder("48 8D 05 ? ? ? ? 48 89 38"_pat).add(3).abs().template as<cs2::CViewRender**>();
    }

    [[nodiscard]] cs2::CCSPlayerController** localPlayerController() const noexcept
    {
        return patternFinders.clientPatternFinder("48 8D 05 ? ? ? ? 48 63 FF 48 8B 04 F8 48"_pat).add(3).abs().template as<cs2::CCSPlayerController**>();
    }
};
