#pragma once

#include <MemorySearch/PatternFinder.h>
#include <Helpers/PatternNotFoundLogger.h>

struct PatternFinders {
    PatternFinder<PatternNotFoundLogger> clientPatternFinder;
    PatternFinder<PatternNotFoundLogger> tier0PatternFinder;
    PatternFinder<PatternNotFoundLogger> soundSystemPatternFinder;
    PatternFinder<PatternNotFoundLogger> fileSystemPatternFinder;
    PatternFinder<PatternNotFoundLogger> panoramaPatternFinder;
    PatternFinder<PatternNotFoundLogger> sdlPatternFinder;
    PatternFinder<PatternNotFoundLogger> sceneSystemPatternFinder;
};
