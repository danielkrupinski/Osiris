#pragma once

#include <GlobalContext.h>
#include <MemoryPatterns/FileSystemPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CBaseFileSystem** FileSystemPatterns::fileSystem() noexcept
{
    return GlobalContext::instance().soundSystemPatternFinder("22 74 ? 48 8B 0D ? ? ? ? 4C 8D 44"_pat).add(6).abs().as<cs2::CBaseFileSystem**>();
}

inline FileNamesOffset FileSystemPatterns::fileNamesOffset() noexcept
{
    return FileNamesOffset{
        GlobalContext::instance().fileSystemPatternFinder("E8 ? ? ? ? 48 8D 8B ? ? ? ? FF 15"_pat).add(8).as<std::int32_t*>()
    };
}
