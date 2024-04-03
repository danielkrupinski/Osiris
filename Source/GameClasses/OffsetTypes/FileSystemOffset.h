#pragma once

#include <cstdint>

#include <CS2/Classes/FileSystem.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using FileSystemOffset = FieldOffset<cs2::CBaseFileSystem, FieldType, OffsetType>;

using FileNamesOffset = FileSystemOffset<cs2::CBaseFileSystem::m_FileNames, std::int32_t>;
