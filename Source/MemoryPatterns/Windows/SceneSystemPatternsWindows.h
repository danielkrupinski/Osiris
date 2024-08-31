#pragma once

#include <CS2/Classes/CSceneSystem.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct SceneSystemPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::CSceneSystem* sceneSystem() const noexcept
    {
        return patternFinders.sceneSystemPatternFinder("48 8D 0D ? ? ? ? 48 8B 54 1A"_pat).add(3).abs().template as<cs2::CSceneSystem*>();
    }

    [[nodiscard]] cs2::CSceneSystem::DeleteSceneObject* deleteSceneObject() const noexcept
    {
        return patternFinders.sceneSystemPatternFinder("48 85 D2 0F 84 ? ? ? ? 48 89 54 24 ? 53"_pat).template as<cs2::CSceneSystem::DeleteSceneObject*>();
    }
};
