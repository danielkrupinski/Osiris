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
#include "WatchedSounds.h"
#include "WatchedSoundType.h"
#include <Utils/BitFlags.h>
#include <Utils/DynamicArray.h>

class SoundWatcher {
public:
    static constexpr auto kFootstepLifespan = 2.0f;
    static constexpr auto kBombPlantLifespan = 2.5f;

    explicit SoundWatcher(GlobalVarsProvider globalVarsProvider) noexcept
        : globalVarsProvider{ globalVarsProvider }
    {
    }

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

    void update() noexcept
    {
        if (!globalVarsProvider || !globalVarsProvider.getGlobalVars())
            return;

        if (watchedSounds.has<WatchedSoundType::Footsteps>()) {
            getSounds<WatchedSoundType::Footsteps>().removeExpiredSounds(globalVarsProvider.getGlobalVars()->curtime, kFootstepLifespan);
        }

        if (watchedSounds.has<WatchedSoundType::BombPlant>()) {
            getSounds<WatchedSoundType::BombPlant>().removeExpiredSounds(globalVarsProvider.getGlobalVars()->curtime, kBombPlantLifespan);
        }

        if (watchedSounds)
            collectSounds(globalVarsProvider.getGlobalVars()->curtime);
    }

    template <WatchedSoundType soundType, typename F>
    void forEach(F&& f) const noexcept
    {
        getSounds<soundType>().forEach(std::forward<F>(f));
    }

private:
    template <WatchedSoundType soundType>
    [[nodiscard]] const WatchedSounds& getSounds() const noexcept
    {
        if constexpr (soundType == WatchedSoundType::Footsteps) {
            return footsteps;
        } else if constexpr (soundType == WatchedSoundType::BombPlant) {
            return bombPlants;
        } else {
            static_assert(soundType != soundType, "Unknown sound type!");
        }
    }

    template <WatchedSoundType soundType>
    [[nodiscard]] WatchedSounds& getSounds() noexcept
    {
        if constexpr (soundType == WatchedSoundType::Footsteps) {
            return footsteps;
        } else if constexpr (soundType == WatchedSoundType::BombPlant) {
            return bombPlants;
        } else {
            static_assert(soundType != soundType, "Unknown sound type!");
        }
    }

    void collectSounds(float curtime) noexcept
    {
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

            if (const auto sounds = getSoundsToAddTo(std::string_view{buffer.data()}, channel.guid))
                sounds->addSound(PlayedSound{ .guid = channel.guid, .spawnTime = curtime, .origin = channelInfo2.memory[i].origin });
        }
    }

    [[nodiscard]] WatchedSounds* getSoundsToAddTo(std::string_view soundName, int guid) noexcept
    {
        if (watchedSounds.has<WatchedSoundType::Footsteps>()) {
            if (isFootstepSound(soundName) && !footsteps.hasSound(guid))
                return &footsteps;
        }

        if (watchedSounds.has<WatchedSoundType::BombPlant>()) {
            if (isBombPlantSound(soundName) && !bombPlants.hasSound(guid))
                return &bombPlants;
        }

        return nullptr;
    }

    [[nodiscard]] static bool isFootstepSound(std::string_view soundName) noexcept
    {
        if (soundName.starts_with(cs2::kPlayerFootstepSoundsPath)) {
            return !std::string_view{ soundName.data() + cs2::kPlayerFootstepSoundsPath.length(), soundName.length() - cs2::kPlayerFootstepSoundsPath.length() }.starts_with(cs2::kPlayerSuitSoundPrefix);
        }
        return false;
    }

    [[nodiscard]] static bool isBombPlantSound(std::string_view soundName) noexcept
    {
        return soundName == cs2::kBombPlantSoundPath;
    }

    cs2::SoundChannels** soundChannels{ SoundSystemPatterns::soundChannels() };
    cs2::CBaseFileSystem** fileSystem{ FileSystemPatterns::fileSystem() };
    GlobalVarsProvider globalVarsProvider;

    BitFlags<WatchedSoundType, std::uint8_t> watchedSounds;
    WatchedSounds footsteps;
    WatchedSounds bombPlants;
};
