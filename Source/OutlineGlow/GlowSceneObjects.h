#pragma once

#include <cstring>

#include <CS2/Classes/CSceneObject.h>
#include <GameClient/SceneSystem/SceneSystem.h>
#include <MemoryAllocation/MemoryAllocator.h>

#include "GlowSceneObject.h"
#include "GlowSceneObjectPointer.h"
#include "GlowSceneObjectsState.h"

template <typename HookContext>
class GlowSceneObjects {
public:
    explicit GlowSceneObjects(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] decltype(auto) getGlowSceneObject(cs2::CSceneObject* sceneObject) const noexcept
    {
        return hookContext.template make<GlowSceneObject>(glowSceneObjectByIndex(findOrInsertGlowSceneObject(sceneObject)));
    }

    void removeUnreferencedObjects() const noexcept
    {
        auto writeIndex = findFirstUnreferencedObject();
        for (auto readIndex = writeIndex; readIndex < state().size; ++readIndex) {
            auto sceneObjectPointer = state().glowSceneObjects[readIndex];
            if (!sceneObjectPointer.isReferenced()) {
                deleteSceneObject(hookContext.template make<GlowSceneObject>(&sceneObjectPointer));
            } else {
                sceneObjectPointer.clearReferenced();
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
            deleteSceneObject(hookContext.template make<GlowSceneObject>(&state().glowSceneObjects[i]));

        state().size = 0;

        if (state().capacity > 0) {
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

    [[nodiscard]] auto findOrInsertGlowSceneObject(cs2::CSceneObject* sceneObject) const noexcept
    {
        if (const auto index = findAttachedSceneObject(sceneObject); index != GlowSceneObjectsState::kInvalidIndex)
            return index;
        return insertGlowSceneObject();
    }

    [[nodiscard]] auto findAttachedSceneObject(cs2::CSceneObject* sceneObject) const noexcept
    {
        for (GlowSceneObjectsState::SizeType i = 0; i < state().size; ++i) {
            if (hookContext.template make<GlowSceneObject>(&state().glowSceneObjects[i]).getAttachedSceneObject() == sceneObject)
                return i;
        }
        return GlowSceneObjectsState::kInvalidIndex;
    }

    [[nodiscard]] auto insertGlowSceneObject() const noexcept
    {
        if (state().size < state().capacity) {
            const auto index = state().size;
            state().glowSceneObjects[index] = nullptr;
            ++state().size;
            return index;
        } else {
            const auto newCapacity = static_cast<GlowSceneObjectsState::SizeType>(state().capacity + 10);
            const auto newGlowSceneObjects = reinterpret_cast<GlowSceneObjectPointer*>(MemoryAllocator<GlowSceneObjectPointer[]>::allocate(newCapacity));
            if (newGlowSceneObjects) {
                if (state().size > 0)
                    std::memcpy(newGlowSceneObjects, state().glowSceneObjects, state().size * sizeof(GlowSceneObjectPointer));

                if (state().capacity > 0)
                    MemoryAllocator<GlowSceneObjectPointer[]>::deallocate(state().glowSceneObjects, state().capacity);

                state().capacity = newCapacity;
                state().glowSceneObjects = newGlowSceneObjects;
                const auto index = state().size;
                state().glowSceneObjects[index] = nullptr;
                ++state().size;
                return index;
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

    void deleteSceneObject(auto&& glowSceneObject) const noexcept
    {
        if (glowSceneObject.isValidGlowSceneObject().valueOr(true))
            hookContext.template make<SceneSystem>().deleteSceneObject(glowSceneObject.baseSceneObject());
    }

    HookContext& hookContext;
};
