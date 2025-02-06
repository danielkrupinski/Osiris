#pragma once

#include <cassert>
#include <new>
#include <utility>

template <typename Partial, typename Complete>
class DeferredCompleteObject {
public:
    template <typename... Args>
    DeferredCompleteObject(Args&&... args) noexcept
        : partialObject{std::forward<Args>(args)...}
    {
    }

    DeferredCompleteObject(const DeferredCompleteObject&) = delete;
    DeferredCompleteObject(DeferredCompleteObject&&) = delete;
    DeferredCompleteObject& operator=(const DeferredCompleteObject&) = delete;
    DeferredCompleteObject& operator=(DeferredCompleteObject&&) = delete;

    [[nodiscard]] bool isComplete() const noexcept
    {
        return hasCompleteObject;
    }

    [[nodiscard]] Partial& partial() noexcept
    {
        assert(!isComplete());
        return partialObject;
    }

    [[nodiscard]] Complete& complete() noexcept
    {
        assert(isComplete());
        return completeObject;
    }

    template <typename... Args>
    void makeComplete(Args&&... args) noexcept
    {
        assert(!isComplete());
        partialObject.~Partial();
        new (&completeObject) Complete{std::forward<Args>(args)...};
        hasCompleteObject = true;
    }

    ~DeferredCompleteObject() noexcept
    {
        if (isComplete())
            completeObject.~Complete();
        else
            partialObject.~Partial();
    }

private:
    union {
        Partial partialObject;
        Complete completeObject;
    };

    bool hasCompleteObject{false};
};
