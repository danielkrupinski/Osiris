# Enemy footstep direction arrows

Enable **Sound > Player Sound Visualization > Enemy Footstep Direction Arrows**.
The setting is off by default and independent of the existing in-world footstep icons.

## Behavior

- A minimal white-blue chevron points toward the last audible enemy footstep.
- Directions are relative to the camera's horizontal orientation, including sounds behind the player.
- One arrow is maintained per matched enemy. Repeated steps update its last audible position and brightness without restarting its initial fade-in.
- Arrows fade in over 80 ms, then fade out completely within 850 ms of the last step.
- Similar bearings use separate radial lanes. The ring itself is never drawn.
- Arrows are hidden when disabled, when safe mode is enabled, or when the local player is dead. Spectator and third-person camera positions are not supported.
- No new sound-emitter offsets or external runtime dependencies are used.

## Source attribution limitation

The current sound-channel layout exposes positions but not emitter handles. New footstep sounds are matched to nearby living players once, at collection time. Allies and the local player participate in the match and cannot produce arrows. Ambiguous matches, unmatched sounds, and sounds on a clearly different floor are suppressed. Rejected channel GUIDs are remembered so moving players do not cause the same channel to be reattributed later.

This is a conservative spatial heuristic, not an authoritative emitter identity. It can miss steps or misattribute sounds in crowded situations. Validate against bots before relying on it. Accurate attribution requires a separately verified emitter field or sound-event API.

## Automated checks (Windows / Visual Studio 2022)

Run from a Developer PowerShell with CMake available:

```powershell
cmake -S . -B build-tests -G "Visual Studio 17 2022" -A x64 "-DENABLE_TESTS=unit;functional"
cmake --build build-tests --config Release --parallel
ctest --test-dir build-tests -C Release --output-on-failure
```

The dedicated `SoundTests` target checks attribution, ally rejection, ambiguity, floors, lifetime, repeated steps, multiple enemies, GUID retention, time resets, legacy merging, and script-buffer bounds. Existing configuration tests validate backward compatibility and the current serialized fixture.

## In-game checks

Use a local practice session with bots:

1. Keep the old in-world visualization off and enable the new arrows; then reverse the settings. They must operate independently. Enable both and verify both receive the same steps.
2. Place one moving enemy in front, to the right, behind, and to the left. Verify the corresponding bearings while turning the camera and looking up/down.
3. Stop the enemy: the arrow must fade out completely with no idle ring or glow.
4. Move two enemies in different directions; verify two arrows. Move them to similar bearings and verify radial separation when attribution is unambiguous.
5. Verify local footsteps and isolated friendly footsteps do not create arrows. Ambiguous overlapping players should not create confidently attributed arrows.
6. Check different floors, round/time resets, player death, disconnects, team changes, feature toggling, and safe mode activation while arrows are visible.
7. Check HUD scaling and multiple aspect ratios. Verify chevrons do not intercept mouse input or persist after unloading/recreating the HUD.
8. Observe frame time with several simultaneous sources: rotation currently uses one Panorama script per visible arrow per frame.
