#pragma once

#include <MemorySearch/PatternFinder.h>
#include <MemorySearch/PatternNotFoundLogger.h>

struct PatternFinders {
    PatternFinder<PatternNotFoundLogger> clientPatternFinder;
    PatternFinder<PatternNotFoundLogger> tier0PatternFinder;
    PatternFinder<PatternNotFoundLogger> soundSystemPatternFinder;
    PatternFinder<PatternNotFoundLogger> fileSystemPatternFinder;
    PatternFinder<PatternNotFoundLogger> panoramaPatternFinder;
    PatternFinder<PatternNotFoundLogger> sdlPatternFinder;
    PatternFinder<PatternNotFoundLogger> sceneSystemPatternFinder;
};
