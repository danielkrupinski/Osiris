#pragma once

#include <cassert>
#include <cstddef>
#include <limits>

template <typename Hook>
class RefCountedHook {
public:
    void incrementReferenceCount() noexcept
    {
        assert(referenceCount < (std::numeric_limits<std::size_t>::max)());
        ++referenceCount;
    }

    void decrementReferenceCount() noexcept
    {
        assert(referenceCount > 0);
        --referenceCount;
    }

    void update() noexcept
    {
        if (isHooked()) {
            if (referenceCount == 0)
                static_cast<Hook&>(*this).uninstall();
        } else if (referenceCount > 0) {
            static_cast<Hook&>(*this).install();
        }
    }

    void forceUninstall() noexcept
    {
        if (isHooked())
            static_cast<Hook&>(*this).uninstall();
    }

private:
    [[nodiscard]] bool isHooked() const noexcept
    {
        return static_cast<const Hook&>(*this).isInstalled();
    }

    RefCountedHook() = default;
    friend Hook;

    std::size_t referenceCount{0};
};
