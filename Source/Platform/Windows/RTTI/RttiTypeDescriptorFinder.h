#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

#include <MemorySearch/HybridPatternFinder.h>
#include "RttiTypeDescriptor.h"
#include <Utils/MemorySection.h>

class RttiTypeDescriptorFinder {
public:
    RttiTypeDescriptorFinder(MemorySection dataSection) noexcept
        : dataSection{dataSection}
    {
    }

    [[nodiscard]] const RttiTypeDescriptor* findTypeDescriptor(std::string_view mangledTypeName) const noexcept
    {
        if (const auto typeDescriptorName{findTypeDescriptorName(mangledTypeName)}; canBePartOfTypeDescriptor(typeDescriptorName))
            return toTypeDescriptorPointer(typeDescriptorName);
        return nullptr;
    }

private:
    [[nodiscard]] const std::byte* findTypeDescriptorName(std::string_view typeDescriptorName) const noexcept
    {
        return HybridPatternFinder{dataSection.raw(), BytePattern{typeDescriptorName}}.findNextOccurrence();
    }

    [[nodiscard]] bool canBePartOfTypeDescriptor(const std::byte* typeDescriptorName) const noexcept
    {
        return typeDescriptorName && dataSection.offsetOf(reinterpret_cast<std::uintptr_t>(typeDescriptorName)) >= RttiTypeDescriptor::kOffsetOfName;
    }

    [[nodiscard]] static const RttiTypeDescriptor* toTypeDescriptorPointer(const std::byte* typeDescriptorName) noexcept
    {
        return reinterpret_cast<const RttiTypeDescriptor*>(typeDescriptorName - RttiTypeDescriptor::kOffsetOfName);
    }

    MemorySection dataSection;
};
