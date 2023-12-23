#pragma once

#include <cstdint>

#include <CS2/Classes/FileSystem.h>
#include <GameClasses/OffsetTypes/FileSystemOffset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

using FileNamesOffset = FileSystemOffset<cs2::CBaseFileSystem::m_FileNames, std::int32_t>;

struct FileSystemPatterns {
    [[nodiscard]] cs2::CBaseFileSystem** fileSystem() const noexcept;
    [[nodiscard]] FileNamesOffset fileNamesOffset() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& soundSystemPatternFinder;
    const PatternFinder<PatternNotFoundLogger>& fileSystemPatternFinder;
};
