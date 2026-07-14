#include <type_traits>

#include <gtest/gtest.h>

#include <GameClient/Panorama/PanoramaUiEngine.h>

#if defined(_WIN64)
namespace {

struct CallState {
    cs2::CUIEngine* engine{};
    int type{};
    const char* text{};
    int calls{};
    cs2::CPanoramaSymbol result{};
    bool hasFunction{true};
    bool hasStorage{true};
};

CallState callState{};
cs2::CUIEngine engine{};
cs2::CUIEngine* engineStorage{&engine};

cs2::CPanoramaSymbol makeSymbol(cs2::CUIEngine* receivedEngine, int type, const char* text) noexcept
{
    callState.engine = receivedEngine;
    callState.type = type;
    callState.text = text;
    ++callState.calls;
    return callState.result;
}

struct PatternResults {
    template <typename T>
    auto get() const noexcept
    {
        if constexpr (std::is_same_v<T, MakeSymbolFunctionPointer>)
            return callState.hasFunction ? &makeSymbol : nullptr;
        else
            return callState.hasStorage ? &engineStorage : nullptr;
    }
};

struct HookContext {
    [[nodiscard]] const PatternResults& patternSearchResults() const noexcept { return results; }
    auto uiPanel(cs2::CUIPanel*) noexcept { return nullptr; }
    PatternResults results{};
};

class PanoramaUiEngineTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        callState = {};
        callState.hasFunction = true;
        callState.hasStorage = true;
        engineStorage = &engine;
    }
};

TEST_F(PanoramaUiEngineTest, ForwardsArgumentsAndPreservesNativeSymbol)
{
    HookContext hookContext;
    PanoramaUiEngine<HookContext> uiEngine{hookContext};
    const char text[]{"test-property"};
    callState.result = static_cast<cs2::CPanoramaSymbol>(0xFFFF);

    EXPECT_EQ(uiEngine.makeSymbol(42, text), static_cast<cs2::CPanoramaSymbol>(0xFFFF));
    EXPECT_EQ(callState.calls, 1);
    EXPECT_EQ(callState.engine, &engine);
    EXPECT_EQ(callState.type, 42);
    EXPECT_EQ(callState.text, text);
}

TEST_F(PanoramaUiEngineTest, RejectsInvalidInputsWithoutCallingNativeFunction)
{
    HookContext hookContext;
    PanoramaUiEngine<HookContext> uiEngine{hookContext};

    callState.hasFunction = false;
    EXPECT_EQ(uiEngine.makeSymbol(0, "name"), -1);
    callState.hasFunction = true;
    callState.hasStorage = false;
    EXPECT_EQ(uiEngine.makeSymbol(0, "name"), -1);
    callState.hasStorage = true;
    engineStorage = nullptr;
    EXPECT_EQ(uiEngine.makeSymbol(0, "name"), -1);
    engineStorage = &engine;
    EXPECT_EQ(uiEngine.makeSymbol(0, nullptr), -1);
    EXPECT_EQ(uiEngine.makeSymbol(0, ""), -1);
    EXPECT_EQ(callState.calls, 0);
}

}
#endif
