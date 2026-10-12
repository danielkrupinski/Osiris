#pragma once

#include <Features/Common/InWorldPanels.h>
#include <Features/Diagnostics/DiagnosticsConfigVariables.h>
#include <GameClient/Entities/PlayerController.h>
#include <GameClient/Panorama/PanoramaUiEngine.h>
#include <MemoryPatterns/PatternTypes/ClientPatternTypes.h>
#include <Utils/StringBuilder.h>

#include "Details/EnemyFootstepSound.h"
#include "Details/FootstepDirectionOpacity.h"
#include "Details/FootstepDirectionScript.h"
#include "Details/SoundVisualizationPanelProperties.h"
#include "SoundVisualizationConfigVariables.h"

struct EnemyFootstepDirectionPanels {
    [[nodiscard]] static SoundVisualizationPanelProperties soundVisualizationPanelProperties() noexcept
    {
        return {.svgImagePath = nullptr, .position = SoundVisualizationPosition::DirectionArrow};
    }
};

template <typename HookContext>
class EnemyFootstepDirection {
public:
    explicit EnemyFootstepDirection(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void onDisable() const noexcept
    {
        hookContext.template make<InWorldPanels>().template hideSoundVisualizationPanels<EnemyFootstepDirectionPanels>();
    }

    void run() const noexcept
    {
        if (!GET_CONFIG_VAR(EnemyFootstepDirectionEnabled) || GET_CONFIG_VAR(diagnostics_vars::SafeModeEnabled))
            return;
        const auto localPawn = hookContext.localPlayerController().pawn().template cast<PlayerPawn>();
        if (!localPawn || !localPawn.isAlive().value_or(false))
            return;
        const auto localOrigin = localPawn.absOrigin();
        const auto curtime = hookContext.globalVars().curtime();
        const auto matrix = hookContext.patternSearchResults().template get<WorldToProjectionMatrixPointer>();
        if (!localOrigin.hasValue() || !curtime.hasValue() || !matrix)
            return;
        const auto rx = matrix->m[0][0];
        const auto ry = matrix->m[0][1];
        if (!(rx * rx + ry * ry > 0.000001f) || rx < -100.0f || rx > 100.0f || ry < -100.0f || ry > 100.0f)
            return;

        std::size_t count = 0;
        const auto watcher = hookContext.soundWatcher();
        const auto& sounds = watcher.template getSoundsOfType<EnemyFootstepSound>();
        sounds.forEach([&](const PlayedSound& sound) {
            if (count >= 64)
                return;
            const auto opacity = footstepDirectionOpacity(sound, curtime.value());
            if (!(opacity > 0.0f))
                return;
            const auto player = hookContext.template make<EntitySystem>().getEntityFromHandle2(sound.source).template cast<PlayerPawn>();
            if (!player || !player.isAlive().value_or(false) || !player.isEnemy().value_or(false))
                return;
            // Keep the last audible position, not the enemy's current position.
            const auto delta = sound.origin - localOrigin.value();
            if (!(delta.x * delta.x + delta.y * delta.y > 1.0f)
                || delta.x < -100000.0f || delta.x > 100000.0f || delta.y < -100000.0f || delta.y > 100000.0f)
                return;
            auto&& panel = hookContext.template make<InWorldPanels>().template getNextSoundVisualizationPanel<EnemyFootstepDirectionPanels>();
            panel.setOpacity(opacity);
            StringBuilderStorage<2048> storage;
            auto script = storage.builder();
            buildFootstepDirectionScript(script, delta.x, delta.y, rx, ry, count == 0);
            hookContext.template make<PanoramaUiEngine>().runScript(panel, script.cstring());
            ++count;
        });
    }

private:
    HookContext& hookContext;
};
