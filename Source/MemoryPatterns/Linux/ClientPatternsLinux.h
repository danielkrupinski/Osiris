#pragma once

#include <CS2/Classes/ClientModeCSNormal.h>
#include <CS2/Classes/CPlantedC4.h>
#include <CS2/Classes/CViewRender.h>
#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <CS2/Classes/GlobalVars.h>
#include <CS2/Classes/Glow.h>
#include <CS2/Classes/CUtlVector.h>
#include <CS2/Classes/VMatrix.h>
#include <MemorySearch/BytePatternLiteral.h>
#include <Utils/ReturnAddress.h>

template <typename PatternFinders>
struct ClientPatterns {
    const PatternFinders& patternFinders;

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
        return patternFinders.clientPatternFinder("8D ? ? ? ? ? 48 89 35 ? ? ? ? 48 89 ? ? C3"_pat).add(9).abs().template as<cs2::GlobalVars**>();
    }

    [[nodiscard]] cs2::ClientModeCSNormal* clientMode() const noexcept
    {
        return patternFinders.clientPatternFinder("48 8D 05 ? ? ? ? 4C 89 E7 48 89 05"_pat).add(3).abs().template as<cs2::ClientModeCSNormal*>();
    }

    [[nodiscard]] ReturnAddress getWorldSessionInClientMode() const noexcept
    {
        return patternFinders.clientPatternFinder("FF 10 49 89 C5 48 85 C0 0F 84"_pat).add(2).template as<ReturnAddress>();
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
        return patternFinders.clientPatternFinder("48 8D 05 ? ? ? ? 4C 8D 05 ? ? ? ? 48 8D 0D"_pat).add(10).abs().template as<cs2::VMatrix*>();
    }

    [[nodiscard]] cs2::VMatrix* viewToProjectionMatrix() const noexcept
    {
        return patternFinders.clientPatternFinder("48 8D 0D ? ? ? ? 48 8B 38 E8"_pat).add(3).abs().template as<cs2::VMatrix*>();
    }

    [[nodiscard]] cs2::CViewRender** viewRender() const noexcept
    {
        return patternFinders.clientPatternFinder("48 8D 05 ? ? ? ? 48 89 38 48 85"_pat).add(3).abs().template as<cs2::CViewRender**>();
    }

    [[nodiscard]] cs2::CCSPlayerController** localPlayerController() const noexcept
    {
        return patternFinders.clientPatternFinder("48 83 3D ? ? ? ? ? 0F 95 C0 C3"_pat).add(3).abs(5).template as<cs2::CCSPlayerController**>();
    }
    
    [[nodiscard]] cs2::ManageGlowSceneObject* manageGlowSceneObject() const noexcept
    {
        return patternFinders.clientPatternFinder("E8 ? ? ? ? 4C 39 F3 48"_pat).add(1).abs().template as<cs2::ManageGlowSceneObject*>();
    }
};
