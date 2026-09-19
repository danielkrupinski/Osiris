#pragma once

#include <gmock/gmock.h>
#include <GameClient/Panorama/ImagePanel.h>
#include <GameClient/Panorama/PanoramaLabel.h>

struct MockHookContext;
struct MockImagePanel;
struct MockLabelPanel;
struct MockPanel;

struct MockClientPanel {
    MOCK_METHOD(MockPanel&, uiPanel, ());
    MOCK_METHOD(MockLabelPanel&, asLabel, ());
    MOCK_METHOD(MockImagePanel&, asImagePanel, ());

    template <template <typename> typename PanelType>
    [[nodiscard]] decltype(auto) as()
    {
        if constexpr (std::is_same_v<PanelType<MockHookContext>, PanoramaLabel<MockHookContext>>) {
            return asLabel();
        } else if constexpr (std::is_same_v<PanelType<MockHookContext>, ImagePanel<MockHookContext>>) {
            return asImagePanel();
        }
    }
};
