#pragma once

#include <CS2/Classes/IMemAlloc.h>
#include <GameDependencies/MemAllocDeps.h>

class MemAlloc {
public:
    [[nodiscard]] static void* allocate(std::size_t size) noexcept
    {
        if (!impl().thisptr || !*impl().thisptr)
            return nullptr;

        if (const auto fn = impl().alloc.of((*impl().thisptr)->vmt).get())
            return (*fn)(*impl().thisptr, size);
        
        return nullptr;
    }

private:
    [[nodiscard]] static const MemAllocDeps& impl() noexcept
    {
        return MemAllocDeps::instance();
    }
};
