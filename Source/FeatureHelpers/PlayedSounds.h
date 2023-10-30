#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <string_view>
#include <utility>

#include <CS2/Classes/Sound.h>
#include <GameClasses/FileSystem.h>
#include <MemoryPatterns/FileSystemPatterns.h>
#include <MemoryPatterns/SoundSystemPatterns.h>
#include "PlayedSound.h"
#include "Sound/WatchedSounds.h"
#include "Sound/WatchedSoundType.h"
#include <Utils/BitFlags.h>
#include <Utils/DynamicArray.h>

class PlayedSounds {
public:
    static constexpr auto kFootstepLifespan = 2.0f;

    template <WatchedSoundType soundType>
    void startWatching() noexcept
    {
        assert(!watchedSounds.has<soundType>());
        watchedSounds.set<soundType>();
    }

    template <WatchedSoundType soundType>
    void stopWatching() noexcept
    {
        assert(watchedSounds.has<soundType>());
        watchedSounds.unset<soundType>();
    }

    void update(float curtime) noexcept
    {
        if (watchedSounds.has<WatchedSoundType::Footsteps>()) {
            getSounds<WatchedSoundType::Footsteps>().removeExpiredSounds(curtime, kFootstepLifespan);
            collectSounds(curtime);
        }
    }

    template <typename F>
    void forEach(F&& f) const noexcept
    {
        footsteps.forEach(std::forward<F>(f));
    }

private:
    template <WatchedSoundType soundType>
    [[nodiscard]] WatchedSounds& getSounds() noexcept
    {
        if constexpr (soundType == WatchedSoundType::Footsteps) {
            return footsteps;
        } else {
            static_assert(soundType != soundType, "Unknown sound type!");
        }
    }

    void collectSounds(float curtime) noexcept
    {
        if (!watchedSounds)
            return;

        if (!soundChannels || !*soundChannels)
            return;

        if (!fileSystem || !*fileSystem)
            return;

        const auto fileNames = FileSystem{*fileSystem}.fileNames();
        if (!fileNames)
            return;

        const auto& channelInfo1 = (*soundChannels)->channelInfo1;
        const auto& channelInfo2 = (*soundChannels)->channelInfo2;

        for (int i = 0; i < channelInfo1.size; ++i) {
            const auto& channel = channelInfo1.memory[i];
            if (!channel.sfx)
                continue;

            std::array<char, 1024> buffer;
            fileNames.getString(channel.sfx->fileNameHandle, buffer);
            buffer.back() = '\0';

            if (watchedSounds.has<WatchedSoundType::Footsteps>()) {
                if (!isFootstepSound(std::string_view{buffer.data()}))
                    continue;

                if (footsteps.hasSound(channel.guid))
                    continue;

                footsteps.addSound(PlayedSound{ .guid = channel.guid, .spawnTime = curtime, .origin = channelInfo2.memory[i].origin });
            }
        }
    }

    [[nodiscard]] static bool isFootstepSound(std::string_view soundName) noexcept
    {
        if (soundName.starts_with(cs2::kPlayerFootstepSoundsPath)) {
            return !std::string_view{ soundName.data() + cs2::kPlayerFootstepSoundsPath.length(), soundName.length() - cs2::kPlayerFootstepSoundsPath.length() }.starts_with(cs2::kPlayerSuitSoundPrefix);
        }
        return false;
    }

    cs2::SoundChannels** soundChannels{ SoundSystemPatterns::soundChannels() };
    cs2::CBaseFileSystem** fileSystem{ FileSystemPatterns::fileSystem() };

    BitFlags<WatchedSoundType, std::uint8_t> watchedSounds;
    WatchedSounds footsteps;
};
