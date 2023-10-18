#pragma once

#include <MemoryPatterns/FileSystemPatterns.h>

struct FileSystemImpl {
    [[nodiscard]] static const FileSystemImpl& instance() noexcept;

    FileNamesOffset fileNamesOffset{ FileSystemPatterns::fileNamesOffset() };
};
