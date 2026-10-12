#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <string_view>
#include <tuple>
#include <type_traits>

#include <CS2/Classes/Sound.h>
#include <CS2/Classes/Vector.h>
#include <GameClient/FileSystem.h>
#include <MemoryPatterns/PatternTypes/SoundSystemPatternTypes.h>
#include "PlayedSound.h"
#include "EnemyFootstepSound.h"
#include "FootstepSource.h"
#include <GameClient/Entities/PlayerPawn.h>
#include "WatchedSounds.h"
#include <Utils/TypeIndex.h>

#include "SoundExpiryChecker.h"
#include "SoundWatcherImplState.h"

template <typename HookContext, typename Sounds>
class SoundWatcherImpl;

template <typename HookContext, typename... Sounds>
class SoundWatcherImpl<HookContext, SoundWatcherImplState<Sounds...>> {
public:
    explicit SoundWatcherImpl(SoundWatcherImplState<Sounds...>& state, HookContext& hookContext) noexcept
        : state{state}
        , hookContext{hookContext}
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
        if constexpr (std::is_same_v<Sound, EnemyFootstepSound>)
            getSoundsOfType<Sound>().clear();
    }

    void update() noexcept
    {
        const auto curtime = hookContext.globalVars().curtime();
        if (!curtime.hasValue())
            return;

        auto soundChannels = hookContext.patternSearchResults().template get<SoundChannelsPointer>();
        if (!soundChannels || !*soundChannels)
            return;

        (removeExpiredSounds<Sounds>(**soundChannels, curtime.value()), ...);
        collectNewSounds(**soundChannels, curtime.value());
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

        const auto fileNames = hookContext.template make<FileSystem<HookContext>>().fileNames();
        if (!fileNames)
            return;

        const auto& channelInfo1 = soundChannels.channelInfo1;
        const auto& channelInfo2 = soundChannels.channelInfo2;
        std::array<FootstepSource, 128> sources;
        std::size_t sourceCount = 0;
        bool sourcesCollected = false;

        for (int i = 0; i < (std::min)(channelInfo1.size, channelInfo2.size); ++i) {
            const auto& channel = channelInfo1.memory[i];
            if (!channel.sfx)
                continue;

            std::array<char, 1024> buffer;
            fileNames.getString(channel.sfx->fileNameHandle, buffer);
            buffer.back() = '\0';

            if constexpr ((std::is_same_v<Sounds, EnemyFootstepSound> || ...)) {
                if (!sourcesCollected && isWatching<EnemyFootstepSound>()
                    && EnemyFootstepSound::isSound(std::string_view{buffer.data()})
                    && !getSoundsOfType<EnemyFootstepSound>().hasSound(channel.guid)) {
                    sourcesCollected = true;
                    hookContext.template make<EntitySystem>().forEachNetworkableEntityIdentity([&](const auto& identity) {
                        const auto player = hookContext.template make<BaseEntity>(static_cast<cs2::C_BaseEntity*>(identity.entity)).template cast<PlayerPawn>();
                        if (!player || !player.isTTorCT() || !player.isAlive().value_or(false))
                            return;
                        const auto playerOrigin = player.absOrigin();
                        if (playerOrigin.hasValue() && sourceCount < sources.size())
                            sources[sourceCount++] = FootstepSource{playerOrigin.value(), player.baseEntity().handle(),
                                !player.isControlledByLocalPlayer() && player.isEnemy().value_or(false)};
                    });
                }
            }
            const auto origin = correctSoundOrigin(channelInfo2.memory[i].origin);
            (collectSound<Sounds>(std::string_view{buffer.data()}, channel.guid, origin, curtime,
                std::span<const FootstepSource>{sources.data(), sourceCount}), ...);
        }
    }

    template <typename Sound>
    void collectSound(std::string_view name, int guid, const cs2::Vector& origin,
        float curtime, std::span<const FootstepSource> sources) noexcept
    {
        if (!isWatching<Sound>() || !Sound::isSound(name))
            return;
        auto& sounds = getSoundsOfType<Sound>();
        if (sounds.hasSound(guid))
            return;
        PlayedSound sound{.spawnTime = curtime, .origin = origin};
        if constexpr (std::is_same_v<Sound, EnemyFootstepSound>) {
            sound.source = matchEnemyFootstepSource(origin, sources);
            if (sound.source.value == cs2::INVALID_EHANDLE_INDEX) {
                // Never reattribute a rejected channel later as players move.
                sounds.rememberGuid(guid);
                return;
            }
            sound.firstSpawnTime = curtime;
        }
        sounds.addSound(guid, sound, Sound::kFadeAwayStart + Sound::kFadeAwayDuration);
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
    HookContext& hookContext;
};
