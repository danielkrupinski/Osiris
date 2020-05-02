#pragma once

enum class FrameStage;
class GameEvent;

namespace Visuals
{
    void playerModel(FrameStage stage) noexcept;
    void colorWorld() noexcept;
    void modifySmoke() noexcept;
    void thirdperson() noexcept;
    void removeVisualRecoil(FrameStage stage) noexcept;
    void removeBlur() noexcept;
    void updateBrightness() noexcept;
    void removeGrass() noexcept;
    void remove3dSky() noexcept;
    void removeShadows() noexcept;
    void applyZoom(FrameStage stage) noexcept;
    void applyScreenEffects() noexcept;
    void hitEffect(GameEvent* event = nullptr) noexcept;
    void hitMarker(GameEvent* event = nullptr) noexcept;
    void disablePostProcessing() noexcept;
    void reduceFlashEffect() noexcept;
    bool removeHands(const char* modelName) noexcept;
    bool removeSleeves(const char* modelName) noexcept;
    bool removeWeapons(const char* modelName) noexcept;
    void skybox() noexcept;
}
