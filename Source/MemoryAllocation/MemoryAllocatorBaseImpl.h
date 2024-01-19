#pragma once

#include <cstddef>

#include <GlobalContext/GlobalContext.h>
#include "FreeMemoryRegionList.h"
#include "MemoryAllocatorBase.h"

std::byte* MemoryAllocatorBase::allocate(std::size_t size) noexcept
{
    return GlobalContext::instance().freeRegionList().allocate(size);
}

void MemoryAllocatorBase::deallocate(std::byte* memory, std::size_t size) noexcept
{
    return GlobalContext::instance().freeRegionList().deallocate(memory, size);
}
