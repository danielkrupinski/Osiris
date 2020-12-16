#pragma once

enum class FrameStage;
class GameEvent;
struct ImDrawList;

namespace Visuals
{
    void playerModel(FrameStage stage) noexcept;
    void colorWorld() noexcept;
    void modifySmoke(FrameStage stage) noexcept;
    void thirdperson() noexcept;
    void removeVisualRecoil(FrameStage stage) noexcept;
    void removeBlur(FrameStage stage) noexcept;
    void updateBrightness() noexcept;
    void removeGrass(FrameStage stage) noexcept;
    void remove3dSky() noexcept;
    void removeShadows() noexcept;
    void applyZoom(FrameStage stage) noexcept;
    void applyScreenEffects() noexcept;
    void hitEffect(GameEvent* event = nullptr) noexcept;
    void hitMarker(GameEvent* event, ImDrawList* drawList = nullptr) noexcept;
    void disablePostProcessing(FrameStage stage) noexcept;
    void reduceFlashEffect() noexcept;
    bool removeHands(const char* modelName) noexcept;
    bool removeSleeves(const char* modelName) noexcept;
    bool removeWeapons(const char* modelName) noexcept;
    void skybox(FrameStage stage) noexcept;
}
