#pragma once

#include <gmock/gmock.h>
#include <Features/FeaturesStates.h>
#include <Features/Hud/BombPlantAlert/BombPlantAlertPanelFactory.h>
#include <Features/Hud/BombTimer/BombTimer.h>
#include <Features/Hud/BombTimer/BombTimerPanelFactory.h>
#include <GameClient/Panorama/PanelHandle.h>
#include <GameClient/Crosshair.h>
#include <GameClient/WorldToScreen/ViewToProjectionMatrix.h>

struct ConfigState;
struct MockConfig;
struct MockCrosshair;
struct MockCvarSystem;
struct MockGameRules;
struct MockGui;
struct MockHud;
struct MockPanelFactory;
struct MockPanelHandle;
struct MockPanoramaUiEngine;
struct MockPlayerController;
struct MockPlayerPawn;
struct MockViewToProjectionMatrix;
struct MockBombPlantAlertPanelFactory;
struct MockBombTimerPanelFactory;
struct MockBombTimerPanel;
struct MockPlantedC4;

class OsirisDirectoryPath;

struct MockHookContext {
    MOCK_METHOD(MockHud&, hud, ());
    MOCK_METHOD(MockGameRules&, gameRules, ());
    MOCK_METHOD(MockPanelFactory&, panelFactory, ());
    MOCK_METHOD(MockConfig&, config, ());
    MOCK_METHOD(ConfigState&, configState, ());
    MOCK_METHOD(OsirisDirectoryPath&, osirisDirectoryPath, ());
    MOCK_METHOD(MockGui&, gui, ());
    MOCK_METHOD(FeaturesStates&, featuresStates, ());
    MOCK_METHOD(MockPlayerController&, localPlayerController, ());
    MOCK_METHOD(MockPanelHandle&, makePanelHandle, (cs2::PanelHandle& panelHandle));
    MOCK_METHOD(MockViewToProjectionMatrix&, makeViewToProjectionMatrix, ());
    MOCK_METHOD(MockCvarSystem&, cvarSystem, ());
    MOCK_METHOD(MockPlayerPawn&, activeLocalPlayerPawn, ());
    MOCK_METHOD(MockCrosshair&, makeCrosshair, ());
    MOCK_METHOD(MockPanoramaUiEngine&, makePanoramaUiEngine, ());
    MOCK_METHOD(MockBombPlantAlertPanelFactory&, makeBombPlantAlertPanelFactory, ());
    MOCK_METHOD(MockBombTimerPanelFactory&, makeBombTimerPanelFactory, ());
    MOCK_METHOD(MockBombTimerPanel&, makeBombTimerPanel, ());
    MOCK_METHOD(Optional<float>, localPlayerBulletInaccuracy, ());
    MOCK_METHOD(MockPlantedC4&, plantedC4, ());

    template <template <typename> typename T, typename... Args>
    [[nodiscard]] decltype(auto) make(Args&&... args)
    {
        if constexpr (std::is_same_v<T<MockHookContext>, PanelHandle<MockHookContext>>) {
            return makePanelHandle(std::forward<Args>(args)...);
        } else if constexpr (std::is_same_v<T<MockHookContext>, ViewToProjectionMatrix<MockHookContext>>) {
            return makeViewToProjectionMatrix(std::forward<Args>(args)...);
        } else if constexpr (std::is_same_v<T<MockHookContext>, Crosshair<MockHookContext>>) {
            return makeCrosshair(std::forward<Args>(args)...);
        } else if constexpr (std::is_same_v<T<MockHookContext>, PanoramaUiEngine<MockHookContext>>) {
            return makePanoramaUiEngine(std::forward<Args>(args)...);
        } else if constexpr (std::is_same_v<T<MockHookContext>, BombPlantAlertPanelFactory<MockHookContext>>) {
            return makeBombPlantAlertPanelFactory(std::forward<Args>(args)...);
        } else if constexpr (std::is_same_v<T<MockHookContext>, BombTimerPanelFactory<MockHookContext>>) {
            return makeBombTimerPanelFactory(std::forward<Args>(args)...);
        } else if constexpr (std::is_same_v<T<MockHookContext>, BombTimerPanel<MockHookContext>>) {
            return makeBombTimerPanel(std::forward<Args>(args)...);
        }
    }
};
