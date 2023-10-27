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

class PlayedSounds {
public:
    void removeExpiredSounds(float curtime, float lifetime) noexcept
    {
        for (std::size_t i = 0; i < numberOfSounds;) {
            auto& sound = sounds[i];
            if (!sound.isAlive(curtime, lifetime))
                removeSound(sound);
            else
                ++i;
        }
    }

    template <typename Predicate>
    void collectSounds(float curtime, Predicate&& predicate) noexcept
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

            if (!predicate(std::string_view{buffer.data()}))
                continue;

            if (std::ranges::find(sounds.begin(), sounds.begin() + numberOfSounds, channel.guid, &PlayedSound::guid) != sounds.begin() + numberOfSounds)
                continue;

            if (numberOfSounds < sounds.size())
                sounds[numberOfSounds++] = PlayedSound{ .guid = channel.guid, .spawnTime = curtime, .origin = channelInfo2.memory[i].origin };
        }
    }

    template <typename F>
    void forEach(F&& f) const noexcept
    {
        std::for_each_n(sounds.cbegin(), numberOfSounds, std::forward<F>(f));
    }

private:
    void removeSound(PlayedSound& sound) noexcept
    {
        sound = sounds[numberOfSounds - 1];
        --numberOfSounds;
    }

    cs2::SoundChannels** soundChannels{ SoundSystemPatterns::soundChannels() };
    cs2::CBaseFileSystem** fileSystem{ FileSystemPatterns::fileSystem() };
    std::array<PlayedSound, 100> sounds; // TODO: dynamically allocate
    std::size_t numberOfSounds{0};
};
