#pragma once

#include <cassert>
#include <cstdint>

#include <CS2/Classes/Glow.h>

class GlowSceneObjectPointer {
public:
    explicit(false) GlowSceneObjectPointer(cs2::CGlowHelperSceneObject* pointer) noexcept
        : pointer{pointer}
    {
        assert(tagBit(pointer) == 0);
    }

    [[nodiscard]] auto value() const noexcept
    {
        return untaggedPointer();
    }

    void setValue(cs2::CGlowHelperSceneObject* newPointer) noexcept
    {
        assert(tagBit(newPointer) == 0);
        pointer = toPointer(toAddress(newPointer) | tagBit(pointer));
    }

    [[nodiscard]] bool isReferenced() const noexcept
    {
        return tagBit(pointer) != 0;
    }

    void markReferenced() noexcept
    {
        pointer = taggedPointer();
    }

    void clearReferenced() noexcept
    {
        pointer = untaggedPointer();
    }

private:
    [[nodiscard]] cs2::CGlowHelperSceneObject* taggedPointer() const noexcept
    {
        return toPointer(toAddress(pointer) | 1);
    }

    [[nodiscard]] cs2::CGlowHelperSceneObject* untaggedPointer() const noexcept
    {
        return toPointer(toAddress(pointer) & ~1);
    }

    [[nodiscard]] static cs2::CGlowHelperSceneObject* toPointer(std::uintptr_t address) noexcept
    {
        return reinterpret_cast<cs2::CGlowHelperSceneObject*>(address);
    }

    [[nodiscard]] static std::uintptr_t toAddress(cs2::CGlowHelperSceneObject* pointer) noexcept
    {
        return reinterpret_cast<std::uintptr_t>(pointer);
    }

    [[nodiscard]] static int tagBit(cs2::CGlowHelperSceneObject* pointer) noexcept
    {
        return toAddress(pointer) & 1;
    }

    cs2::CGlowHelperSceneObject* pointer;
};
