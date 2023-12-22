#pragma once

#include <cstdint>

#include <CS2/Classes/FileSystem.h>
#include <GameClasses/OffsetTypes/FileSystemOffset.h>

using FileNamesOffset = FileSystemOffset<cs2::CBaseFileSystem::m_FileNames, std::int32_t>;

struct FileSystemPatterns {
    [[nodiscard]] cs2::CBaseFileSystem** fileSystem() const noexcept;
    [[nodiscard]] FileNamesOffset fileNamesOffset() const noexcept;
};
