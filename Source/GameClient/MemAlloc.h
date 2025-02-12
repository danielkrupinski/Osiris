#pragma once

#include <cstddef>

#include <MemoryPatterns/PatternTypes/MemAllocPatternTypes.h>
#include <Platform/Macros/FunctionAttributes.h>

template <typename HookContext>
class MemAlloc {
public:
    explicit MemAlloc(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }
    
    [[nodiscard]] [[NOINLINE]] void* allocate(std::size_t size) const noexcept
    {
        if (!deps().thisptr || !*deps().thisptr)
            return nullptr;

        if (const auto fn = hookContext.clientPatternSearchResults().template get<OffsetAllocVirtualMethod>().of((*deps().thisptr)->vmt).get())
            return (*fn)(*deps().thisptr, size);
        
        return nullptr;
    }

private:
    [[nodiscard]] const auto& deps() const noexcept
    {
        return hookContext.memAllocState();
    }

    HookContext& hookContext;
};
