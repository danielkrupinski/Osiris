#pragma once

#include <gmock/gmock.h>
#include <GameClient/Panorama/PanoramaLabel.h>

struct MockHookContext;
struct MockLabelPanel;
struct MockPanel;

struct MockClientPanel {
    MOCK_METHOD(MockPanel&, uiPanel, ());
    MOCK_METHOD(MockLabelPanel&, asLabel, ());
    
    template <template <typename> typename PanelType>
    [[nodiscard]] decltype(auto) as()
    {
        if constexpr (std::is_same_v<PanelType<MockHookContext>, PanoramaLabel<MockHookContext>>) {
            return asLabel();
        }
    }
};
