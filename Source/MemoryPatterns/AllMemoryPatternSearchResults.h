#pragma once

#include <MemoryPatterns/MemoryPatterns.h>
#include <MemorySearch/PatternSearchResults.h>

struct AllMemoryPatternSearchResults {
    explicit AllMemoryPatternSearchResults(const MemoryPatterns& memoryPatterns)
        : clientPatternSearchResults{memoryPatterns.patternFinders.clientPatternFinder.findPatterns(kClientPatterns)}
        , sceneSystemPatternSearchResults{memoryPatterns.patternFinders.sceneSystemPatternFinder.findPatterns(kSceneSystemPatterns)}
        , tier0PatternSearchResults{memoryPatterns.patternFinders.tier0PatternFinder.findPatterns(kTier0Patterns)}
        , fileSystemPatternSearchResults{memoryPatterns.patternFinders.fileSystemPatternFinder.findPatterns(kFileSystemPatterns)}
        , soundSystemPatternSearchResults{memoryPatterns.patternFinders.soundSystemPatternFinder.findPatterns(kSoundSystemPatterns)}
        , panoramaPatternSearchResults{memoryPatterns.patternFinders.panoramaPatternFinder.findPatterns(kPanoramaPatterns)}
    {
    }

    template <typename PatternType>
    [[nodiscard]] auto get() const noexcept
    {
        if constexpr (decltype(kClientPatterns)::PatternPool::PatternTypes::template contains<PatternType>())
            return clientPatternSearchResults.get<PatternType>();
        else if constexpr (decltype(kSceneSystemPatterns)::PatternPool::PatternTypes::template contains<PatternType>())
            return sceneSystemPatternSearchResults.get<PatternType>();
        else if constexpr (decltype(kTier0Patterns)::PatternPool::PatternTypes::template contains<PatternType>())
            return tier0PatternSearchResults.get<PatternType>();
        else if constexpr (decltype(kFileSystemPatterns)::PatternPool::PatternTypes::template contains<PatternType>())
            return fileSystemPatternSearchResults.get<PatternType>();
        else if constexpr (decltype(kSoundSystemPatterns)::PatternPool::PatternTypes::template contains<PatternType>())
            return soundSystemPatternSearchResults.get<PatternType>();
        else if constexpr (decltype(kPanoramaPatterns)::PatternPool::PatternTypes::template contains<PatternType>())
            return panoramaPatternSearchResults.get<PatternType>();
    }

    PatternSearchResults<decltype(kClientPatterns)> clientPatternSearchResults;
    PatternSearchResults<decltype(kSceneSystemPatterns)> sceneSystemPatternSearchResults;
    PatternSearchResults<decltype(kTier0Patterns)> tier0PatternSearchResults;
    PatternSearchResults<decltype(kFileSystemPatterns)> fileSystemPatternSearchResults;
    PatternSearchResults<decltype(kSoundSystemPatterns)> soundSystemPatternSearchResults;
    PatternSearchResults<decltype(kPanoramaPatterns)> panoramaPatternSearchResults;
};
