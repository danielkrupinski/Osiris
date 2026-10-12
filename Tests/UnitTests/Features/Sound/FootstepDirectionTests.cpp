#include <array>
#include <new>
#include <vector>

#include <gtest/gtest.h>
#include <Features/Sound/Details/FootstepDirectionOpacity.h>
#include <Features/Sound/Details/FootstepDirectionScript.h>
#include <Features/Sound/Details/FootstepSource.h>
#include <Features/Sound/Details/WatchedSounds.h>

std::byte* MemoryAllocatorBase::allocate(std::size_t size) noexcept
{
    return new (std::nothrow) std::byte[size];
}

void MemoryAllocatorBase::deallocate(std::byte* memory, std::size_t) noexcept
{
    delete[] memory;
}

namespace {
constexpr cs2::CEntityHandle invalid{cs2::INVALID_EHANDLE_INDEX};
constexpr cs2::CEntityHandle enemy{17};
constexpr cs2::CEntityHandle other{18};

std::vector<PlayedSound> snapshot(const WatchedSounds& sounds)
{
    std::vector<PlayedSound> result;
    sounds.forEach([&](const auto& sound) { result.push_back(sound); });
    return result;
}

TEST(FootstepSource, MatchesNearbyEnemy)
{
    const std::array sources{FootstepSource{{10, 0, 20}, enemy, true}};
    EXPECT_EQ(matchEnemyFootstepSource({0, 0, 0}, sources), enemy);
}

TEST(FootstepSource, MatchesSingleEnemyNearRadiusLimit)
{
    const std::array sources{FootstepSource{{47, 0, 0}, enemy, true}};
    EXPECT_EQ(matchEnemyFootstepSource({0, 0, 0}, sources), enemy);
}

TEST(FootstepSource, RejectsNearestAllyInsteadOfChoosingEnemy)
{
    const std::array sources{FootstepSource{{0, 0, 0}, other, false}, FootstepSource{{30, 0, 0}, enemy, true}};
    EXPECT_EQ(matchEnemyFootstepSource({0, 0, 0}, sources), invalid);
}

TEST(FootstepSource, RejectsAmbiguousSources)
{
    const std::array sources{FootstepSource{{5, 0, 0}, enemy, true}, FootstepSource{{6, 0, 0}, other, true}};
    EXPECT_EQ(matchEnemyFootstepSource({0, 0, 0}, sources), invalid);
}

TEST(FootstepSource, RejectsDifferentFloorAndDistantSources)
{
    const std::array sources{FootstepSource{{0, 0, 80}, enemy, true}, FootstepSource{{49, 0, 0}, other, true}};
    EXPECT_EQ(matchEnemyFootstepSource({0, 0, 0}, sources), invalid);
}

TEST(FootstepSource, RejectsMissingSources)
{
    EXPECT_EQ(matchEnemyFootstepSource({0, 0, 0}, {}), invalid);
}

TEST(FootstepSource, IndependentOfSourceOrder)
{
    const FootstepSource near{{0, 0, 0}, enemy, true};
    const FootstepSource far{{30, 0, 0}, other, false};
    const std::array first{near, far};
    const std::array second{far, near};
    EXPECT_EQ(matchEnemyFootstepSource({0, 0, 0}, first), enemy);
    EXPECT_EQ(matchEnemyFootstepSource({0, 0, 0}, second), enemy);
}

TEST(FootstepOpacity, FadesInAndOutSmoothly)
{
    const PlayedSound sound{.spawnTime = 10, .origin = {}, .source = enemy, .firstSpawnTime = 10};
    EXPECT_FLOAT_EQ(footstepDirectionOpacity(sound, 10), 0);
    EXPECT_NEAR(footstepDirectionOpacity(sound, 10.04f), 0.5f, 0.001f);
    EXPECT_NEAR(footstepDirectionOpacity(sound, 10.08f), 1, 0.001f);
    EXPECT_NEAR(footstepDirectionOpacity(sound, 10.525f), 0.5f, 0.001f);
    EXPECT_NEAR(footstepDirectionOpacity(sound, 10.85f), 0, 0.001f);
    EXPECT_FLOAT_EQ(footstepDirectionOpacity(sound, 11), 0);
}

TEST(FootstepOpacity, HidesAfterTimeReset)
{
    const PlayedSound sound{.spawnTime = 10, .origin = {}, .source = enemy, .firstSpawnTime = 10};
    EXPECT_FLOAT_EQ(footstepDirectionOpacity(sound, 1), 0);
}

TEST(FootstepOpacity, RepeatedStepsDoNotRestartFadeIn)
{
    const PlayedSound sound{.spawnTime = 10.4f, .origin = {}, .source = enemy, .firstSpawnTime = 10};
    EXPECT_FLOAT_EQ(footstepDirectionOpacity(sound, 10.4f), 1);
}

TEST(WatchedFootsteps, MovingEnemyHasOneArrowAndLatestAudiblePosition)
{
    WatchedSounds sounds;
    sounds.addSound(1, {.spawnTime = 10, .origin = {0, 0, 0}, .source = enemy, .firstSpawnTime = 10});
    sounds.addSound(2, {.spawnTime = 10.4f, .origin = {30, 0, 0}, .source = enemy, .firstSpawnTime = 10.4f});
    const auto result = snapshot(sounds);
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].origin, (cs2::Vector{30, 0, 0}));
    EXPECT_FLOAT_EQ(result[0].spawnTime, 10.4f);
    EXPECT_FLOAT_EQ(result[0].firstSpawnTime, 10);
    EXPECT_TRUE(sounds.hasSound(1));
    EXPECT_TRUE(sounds.hasSound(2));
}

TEST(WatchedFootsteps, DifferentEnemiesAtSamePositionRemainSeparate)
{
    WatchedSounds sounds;
    sounds.addSound(1, {.spawnTime = 10, .origin = {}, .source = enemy, .firstSpawnTime = 10});
    sounds.addSound(2, {.spawnTime = 10, .origin = {}, .source = other, .firstSpawnTime = 10});
    EXPECT_EQ(snapshot(sounds).size(), 2);
}

TEST(WatchedFootsteps, SilentGapRestartsFadeIn)
{
    WatchedSounds sounds;
    sounds.addSound(1, {.spawnTime = 10, .origin = {}, .source = enemy, .firstSpawnTime = 10});
    sounds.addSound(2, {.spawnTime = 12, .origin = {}, .source = enemy, .firstSpawnTime = 12});
    const auto result = snapshot(sounds);
    ASSERT_EQ(result.size(), 1);
    EXPECT_FLOAT_EQ(result[0].firstSpawnTime, 12);
    EXPECT_FLOAT_EQ(footstepDirectionOpacity(result[0], 12), 0);
}

TEST(WatchedFootsteps, TimeResetStartsNewBurst)
{
    WatchedSounds sounds;
    sounds.addSound(1, {.spawnTime = 100, .origin = {}, .source = enemy, .firstSpawnTime = 100});
    sounds.addSound(2, {.spawnTime = 1, .origin = {}, .source = enemy, .firstSpawnTime = 1});
    const auto result = snapshot(sounds);
    ASSERT_EQ(result.size(), 1);
    EXPECT_FLOAT_EQ(result[0].firstSpawnTime, 1);
    EXPECT_FLOAT_EQ(footstepDirectionOpacity(result[0], 1.1f), 1);
}

TEST(WatchedFootsteps, RejectedGuidIsRememberedWithoutArrow)
{
    WatchedSounds sounds;
    sounds.rememberGuid(5);
    EXPECT_TRUE(sounds.hasSound(5));
    EXPECT_TRUE(snapshot(sounds).empty());
    sounds.removeExpiredSounds([](const auto&) { return true; });
    EXPECT_FALSE(sounds.hasSound(5));
}

TEST(WatchedFootsteps, ClearingRemovesSoundsAndGuids)
{
    WatchedSounds sounds;
    sounds.addSound(1, {.spawnTime = 10, .origin = {}, .source = enemy, .firstSpawnTime = 10});
    sounds.rememberGuid(5);
    sounds.clear();
    EXPECT_TRUE(snapshot(sounds).empty());
    EXPECT_FALSE(sounds.hasSound(1));
    EXPECT_FALSE(sounds.hasSound(5));
}

TEST(WatchedFootsteps, LegacyVisualizationStillMergesOnlyExactOrigins)
{
    WatchedSounds sounds;
    sounds.addSound(1, {.spawnTime = 10, .origin = {0, 0, 0}});
    sounds.addSound(2, {.spawnTime = 11, .origin = {0, 0, 0}});
    sounds.addSound(3, {.spawnTime = 11, .origin = {30, 0, 0}});
    const auto result = snapshot(sounds);
    ASSERT_EQ(result.size(), 2);
    EXPECT_FLOAT_EQ(result[0].spawnTime, 11);
}

TEST(WatchedFootsteps, ExpiryPreservesGuidSoundAssociations)
{
    WatchedSounds sounds;
    sounds.addSound(1, {.spawnTime = 10, .origin = {}, .source = enemy, .firstSpawnTime = 10});
    sounds.addSound(2, {.spawnTime = 11, .origin = {}, .source = other, .firstSpawnTime = 11});
    sounds.addSound(3, {.spawnTime = 11, .origin = {5, 0, 0}, .source = other, .firstSpawnTime = 11});
    sounds.removeExpiredSounds([](const auto& value) {
        if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, int>)
            return value == 1;
        else
            return value.source == enemy;
    });
    const auto result = snapshot(sounds);
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].source, other);
    EXPECT_FALSE(sounds.hasSound(1));
    EXPECT_TRUE(sounds.hasSound(2));
    EXPECT_TRUE(sounds.hasSound(3));
}

TEST(FootstepScript, FitsBufferAndResetsLanesOnlyOnFirstArrow)
{
    StringBuilderStorage<2048> storage;
    auto script = storage.builder();
    buildFootstepDirectionScript(script, 100000, -100000, 100, -100, true);
    const std::string first{script.cstring()};
    EXPECT_LT(first.size(), 2047);
    EXPECT_TRUE(first.ends_with("})();"));
    EXPECT_NE(first.find("root.osirisFootstepAngles=[];"), std::string::npos);
    auto next = storage.builder();
    buildFootstepDirectionScript(next, 10, 20, 1, 0, false);
    EXPECT_EQ(std::string{next.cstring()}.find("root.osirisFootstepAngles=[];"), std::string::npos);
}
} // namespace
