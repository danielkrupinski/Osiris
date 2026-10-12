#pragma once

#include <algorithm>
#include <cstddef>
#include <utility>

#include "PlayedSound.h"
#include <CS2/Classes/Vector.h>
#include <Utils/DynamicArray.h>

class WatchedSounds {
public:
    void clear() noexcept
    {
        guids.clear();
        sounds.clear();
    }

    void rememberGuid(int guid) noexcept
    {
        guids.pushBack(guid);
    }

    void addSound(int guid, const PlayedSound& sound, float lifetime = 0.85f) noexcept
    {
        if (!guids.pushBack(guid))
            return;

        if (const auto existing = findMatchingSound(sound)) {
            if (sound.source.value != cs2::INVALID_EHANDLE_INDEX) {
                // A new burst must fade in again, even if an expired entry is
                // still retained while its original channel remains active.
                if (sound.spawnTime < existing->spawnTime || sound.spawnTime - existing->spawnTime > lifetime)
                    existing->firstSpawnTime = sound.firstSpawnTime;
                existing->origin = sound.origin;
            }
            existing->spawnTime = sound.spawnTime;
        } else if (sounds.pushBack(sound)) {
            std::swap(guids.back(), guids[sounds.getSize() - 1]);
        }
    }

    [[nodiscard]] bool hasSound(int soundGuid) const noexcept
    {
        // can not use std::ranges::find() because it tries to link with __std_find_trivial_4
        for (const auto guid : guids) {
            if (guid == soundGuid)
                return true;
        }
        return false;
    }

    template <typename Predicate>
    void removeExpiredSounds(Predicate&& predicate) noexcept
    {
        for (std::size_t i = 0; i < guids.getSize();) {
            if (!tryRemoveGuidAndSound(std::forward<Predicate>(predicate), i))
                ++i;
        }
    }

    template <typename F>
    void forEach(F&& f) const noexcept
    {
        std::ranges::for_each(sounds, std::forward<F>(f));
    }

private:
    template <typename Predicate>
    [[nodiscard]] bool tryRemoveGuidAndSound(Predicate&& predicate, std::size_t index) noexcept
    {
        const auto hasSoundAssociatedWithGuid = index < sounds.getSize();
        const auto canRemoveSound = !hasSoundAssociatedWithGuid || predicate(std::as_const(sounds[index]));

        if (canRemoveSound && predicate(std::as_const(guids[index]))) {
            hasSoundAssociatedWithGuid ? removeSoundAndGuid(index) : removeGuid(index);
            return true;
        }
        return false;
    }

    [[nodiscard]] PlayedSound* findMatchingSound(const PlayedSound& sound) noexcept
    {
        for (auto& existing : sounds) {
            if (sound.source.value != cs2::INVALID_EHANDLE_INDEX) {
                if (existing.source == sound.source)
                    return &existing;
            } else if (existing.source.value == cs2::INVALID_EHANDLE_INDEX && existing.origin == sound.origin) {
                return &existing;
            }
        }
        return nullptr;
    }

    void removeSoundAndGuid(std::size_t index) noexcept
    {
        sounds.fastRemoveAt(index);
        guids[index] = guids[sounds.getSize()];
        removeGuid(sounds.getSize());
    }

    void removeGuid(std::size_t index) noexcept
    {
        guids.fastRemoveAt(index);
    }

    DynamicArray<int> guids;
    DynamicArray<PlayedSound> sounds;
};
