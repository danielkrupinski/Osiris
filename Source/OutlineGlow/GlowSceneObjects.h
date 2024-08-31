#pragma once

#include <GameClasses/SceneSystem.h>
#include "GlowSceneObject.h"
#include "GlowSceneObjectsState.h"

template <typename HookContext>
class GlowSceneObjects {
public:
    explicit GlowSceneObjects(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] decltype(auto) getGlowSceneObject(cs2::CEntityHandle entityHandle) const noexcept
    {
        return hookContext.template make<GlowSceneObject>(glowSceneObjectByIndex(findOrInsertEntityHandle(entityHandle)));
    }

    void removeUnreferencedObjects() const noexcept
    {
        auto writeIndex = findFirstUnreferencedObject();
        for (auto readIndex = writeIndex; readIndex < state().size; ++readIndex) {
            auto sceneObjectPointer = state().glowSceneObjects[readIndex];
            if (!sceneObjectPointer.isReferenced()) {
                deleteSceneObject(sceneObjectPointer.value());
            } else {
                sceneObjectPointer.clearReferenced();
                state().entityHandles[writeIndex] = state().entityHandles[readIndex];
                state().glowSceneObjects[writeIndex] = sceneObjectPointer;
                ++writeIndex;
            }
        }

        if (writeIndex < state().size)
            state().size = writeIndex;
    }

    void clearObjects() const noexcept
    {
        for (GlowSceneObjectsState::SizeType i = 0; i < state().size; ++i)
            deleteSceneObject(state().glowSceneObjects[i].value());

        state().size = 0;

        if (state().capacity > 0) {
            MemoryAllocator<cs2::CEntityHandle[]>::deallocate(state().entityHandles, state().capacity);
            MemoryAllocator<GlowSceneObjectPointer[]>::deallocate(state().glowSceneObjects, state().capacity);
            state().capacity = 0;
        }
    }

private:
    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.glowSceneObjectsState();
    }

    [[nodiscard]] GlowSceneObjectPointer* glowSceneObjectByIndex(GlowSceneObjectsState::SizeType index) const noexcept
    {
        if (index < state().size) {
            auto& glowSceneObjectPointer = state().glowSceneObjects[index];
            glowSceneObjectPointer.markReferenced();
            return &glowSceneObjectPointer;
        }
        return nullptr;
    }

    [[nodiscard]] auto findOrInsertEntityHandle(cs2::CEntityHandle handle) const noexcept
    {
        if (const auto index = findEntityHandle(handle); index != GlowSceneObjectsState::kInvalidIndex)
            return index;
        return insertEntityHandle(handle);
    }

    [[nodiscard]] auto findEntityHandle(cs2::CEntityHandle handle) const noexcept
    {
        for (GlowSceneObjectsState::SizeType i = 0; i < state().size; ++i) {
            if (state().entityHandles[i] == handle)
                return i;
        }
        return GlowSceneObjectsState::kInvalidIndex;
    }

    [[nodiscard]] auto insertEntityHandle(cs2::CEntityHandle handle) const noexcept
    {
        if (state().size < state().capacity) {
            const auto index = state().size;
            state().entityHandles[index] = handle;
            state().glowSceneObjects[index] = nullptr;
            ++state().size;
            return index;
        } else {
            const auto newCapacity = static_cast<GlowSceneObjectsState::SizeType>(state().capacity + 10);
            const auto newEntityHandles = reinterpret_cast<cs2::CEntityHandle*>(MemoryAllocator<cs2::CEntityHandle[]>::allocate(newCapacity));
            const auto newGlowSceneObjects = reinterpret_cast<GlowSceneObjectPointer*>(MemoryAllocator<GlowSceneObjectPointer[]>::allocate(newCapacity));
            if (newEntityHandles && newGlowSceneObjects) {
                if (state().size > 0) {
                    std::memcpy(newEntityHandles, state().entityHandles, state().size * sizeof(cs2::CEntityHandle));
                    std::memcpy(newGlowSceneObjects, state().glowSceneObjects, state().size * sizeof(GlowSceneObjectPointer));
                }

                if (state().capacity > 0) {
                    MemoryAllocator<cs2::CEntityHandle[]>::deallocate(state().entityHandles, state().capacity);
                    MemoryAllocator<GlowSceneObjectPointer[]>::deallocate(state().glowSceneObjects, state().capacity);
                }

                state().capacity = newCapacity;
                state().entityHandles = newEntityHandles;
                state().glowSceneObjects = newGlowSceneObjects;
                const auto index = state().size;
                state().entityHandles[index] = handle;
                state().glowSceneObjects[index] = nullptr;
                ++state().size;
                return index;
            } else if (newEntityHandles) {
                MemoryAllocator<cs2::CEntityHandle[]>::deallocate(newEntityHandles, newCapacity);
            } else if (newGlowSceneObjects) {
                MemoryAllocator<GlowSceneObjectPointer[]>::deallocate(newGlowSceneObjects, newCapacity);
            }
        }
        return GlowSceneObjectsState::kInvalidIndex;
    }

    [[nodiscard]] auto findFirstUnreferencedObject() const noexcept
    {
        for (GlowSceneObjectsState::SizeType i = 0; i < state().size; ++i) {
            if (auto& sceneObjectPointer = state().glowSceneObjects[i]; sceneObjectPointer.isReferenced())
                sceneObjectPointer.clearReferenced();
            else
                return i;
        }
        return GlowSceneObjectsState::kInvalidIndex;
    }

    void deleteSceneObject(auto&& sceneObject) const noexcept
    {
        hookContext.template make<SceneSystem>().deleteSceneObject(sceneObject);
    }

    HookContext& hookContext;
};
