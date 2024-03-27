#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <string_view>
#include <tuple>
#include <utility>

#include <CS2/Classes/Sound.h>
#include <GameClasses/FileSystem.h>
#include <HookDependencies/HookDependencies.h>
#include <MemoryPatterns/FileSystemPatterns.h>
#include <MemoryPatterns/SoundSystemPatterns.h>
#include "PlayedSound.h"
#include "WatchedSounds.h"
#include <Utils/BitFlags.h>
#include <Utils/DynamicArray.h>
#include <Utils/TypeBitFlags.h>
#include <Utils/TypeIndex.h>

#include "SoundExpiryChecker.h"
#include "SoundWatcherImplState.h"

template <typename Sounds>
class SoundWatcherImpl;

template <typename... Sounds>
class SoundWatcherImpl<SoundWatcherImplState<Sounds...>> {
public:
    explicit SoundWatcherImpl(SoundWatcherImplState<Sounds...>& state, HookDependencies& hookDependencies) noexcept
        : state{state}
        , hookDependencies{hookDependencies}
    {
    }

    template <typename Sound>
    void startWatching() noexcept
    {
        assert(!isWatching<Sound>());
        state.soundsToWatch.template set<Sound>();
    }

    template <typename Sound>
    void stopWatching() noexcept
    {
        assert(isWatching<Sound>());
        state.soundsToWatch.template unset<Sound>();
    }

    void update() noexcept
    {
        constexpr auto kCrucialDependencies{HookDependenciesMask{}.set<CurTime>().set<SoundChannels>()};
        if (!hookDependencies.requestDependencies(kCrucialDependencies))
            return;

        const auto curtime = hookDependencies.getDependency<CurTime>();
        auto& soundChannels = hookDependencies.getDependency<SoundChannels>();
        (removeExpiredSounds<Sounds>(soundChannels, curtime), ...);
        collectNewSounds(soundChannels, curtime);
    }

    template <typename Sound>
    [[nodiscard]] const WatchedSounds& getSoundsOfType() const noexcept
    {
        return state.watchedSounds[indexOf<Sound>()];
    }

private:
    template <typename Sound>
    void removeExpiredSounds(cs2::SoundChannels& soundChannels, float curtime) noexcept
    {
        if (isWatching<Sound>())
            getSoundsOfType<Sound>().removeExpiredSounds(SoundExpiryChecker{soundChannels.channelInfo1, curtime, Sound::kFadeAwayStart + Sound::kFadeAwayDuration});
    }

    template <typename Sound>
    [[nodiscard]] bool isWatching() const noexcept
    {
        return state.soundsToWatch.template has<Sound>();
    }

    template <typename Sound>
    [[nodiscard]] WatchedSounds& getSoundsOfType() noexcept
    {
        return state.watchedSounds[indexOf<Sound>()];
    }

    void collectNewSounds(cs2::SoundChannels& soundChannels, float curtime) noexcept
    {
        if (!state.soundsToWatch)
            return;

        if (!hookDependencies.requestDependency<FileSystem>())
            return;

        const auto fileNames = hookDependencies.getDependency<FileSystem>().fileNames();
        if (!fileNames)
            return;

        const auto& channelInfo1 = soundChannels.channelInfo1;
        const auto& channelInfo2 = soundChannels.channelInfo2;

        for (int i = 0; i < channelInfo1.size; ++i) {
            const auto& channel = channelInfo1.memory[i];
            if (!channel.sfx)
                continue;

            std::array<char, 1024> buffer;
            fileNames.getString(channel.sfx->fileNameHandle, buffer);
            buffer.back() = '\0';

            if (const auto sounds = getSoundsToAddTo(std::string_view{buffer.data()}, channel.guid))
                sounds->addSound(channel.guid, PlayedSound{ .spawnTime = curtime, .origin = correctSoundOrigin(channelInfo2.memory[i].origin) });
        }
    }

    [[nodiscard]] WatchedSounds* getSoundsToAddTo(std::string_view soundName, int guid) noexcept
    {
        WatchedSounds* result = nullptr;
        std::ignore = (getSoundsToAddTo<Sounds>(soundName, guid, result) || ...);
        return result;
    }

    template <typename Sound>
    [[nodiscard]] bool getSoundsToAddTo(std::string_view soundName, int guid, WatchedSounds*& out) noexcept
    {
        if (!isWatching<Sound>())
            return false;

        if (Sound::isSound(soundName)) {
            if (auto& sounds = getSoundsOfType<Sound>(); !sounds.hasSound(guid))
                out = &sounds;
            return true;
        }
        return false;
    }

    [[nodiscard]] static cs2::Vector correctSoundOrigin(cs2::Vector origin) noexcept
    {
        constexpr auto heightDifference = -20.0f;
        return cs2::Vector{ origin.x, origin.y, origin.z + heightDifference };
    }

    template <typename Sound>
    [[nodiscard]] static consteval std::size_t indexOf() noexcept
    {
        return utils::typeIndex<Sound, std::tuple<Sounds...>>();
    }

    SoundWatcherImplState<Sounds...>& state;
    HookDependencies& hookDependencies;
};
