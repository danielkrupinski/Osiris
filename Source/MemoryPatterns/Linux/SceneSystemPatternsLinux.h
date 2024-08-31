#pragma once

#include <CS2/Classes/CSceneSystem.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct SceneSystemPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::CSceneSystem* sceneSystem() const noexcept
    {
        return patternFinders.sceneSystemPatternFinder("? ? ? ? F6 84 02 ? ? ? ? ? 75"_pat).abs().template as<cs2::CSceneSystem*>();
    }

    [[nodiscard]] cs2::CSceneSystem::DeleteSceneObject* deleteSceneObject() const noexcept
    {
        return patternFinders.sceneSystemPatternFinder("48 85 F6 0F 84 ? ? ? ? 55 48 89 E5 41 56 49"_pat).template as<cs2::CSceneSystem::DeleteSceneObject*>();
    }
};
