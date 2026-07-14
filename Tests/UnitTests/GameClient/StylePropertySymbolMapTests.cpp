#include <cstdint>

#include <gtest/gtest.h>

#include <GameClient/Panorama/StylePropertySymbolMap.h>

#if defined(_WIN64)
namespace {

struct CallState {
    std::uint8_t* output{};
    const char* name{};
    std::uint8_t result{cs2::CStyleSymbol::kInvalidId};
    std::uint8_t initialOutput{};
    std::uint8_t writtenOutput{};
    bool leaveOutputUnchanged{};
    bool returnWrongPointer{};
    int calls{};
};

CallState callState{};
std::uint8_t otherOutput{};

std::uint8_t* findStylePropertySymbol(std::uint8_t* output, const char* name) noexcept
{
    callState.output = output;
    callState.name = name;
    callState.initialOutput = *output;
    ++callState.calls;
    if (!callState.leaveOutputUnchanged) {
        *output = callState.result;
        callState.writtenOutput = *output;
    }
    return callState.returnWrongPointer ? &otherOutput : output;
}

class StylePropertySymbolMapTest : public ::testing::Test {
protected:
    void SetUp() override { callState = {}; }
};

TEST_F(StylePropertySymbolMapTest, ForwardsOriginalNameAndWritableOutputAndAcceptsValidIds)
{
    StylePropertySymbolMap map{&findStylePropertySymbol};
    const char name[]{"opacity"};

    callState.result = 0x00;
    EXPECT_EQ(map.findSymbol(name).m_Id, 0x00);
    EXPECT_EQ(callState.name, name);
    ASSERT_NE(callState.output, nullptr);
    EXPECT_EQ(callState.writtenOutput, 0x00);

    callState.result = 0xFE;
    EXPECT_EQ(map.findSymbol(name).m_Id, 0xFE);
}

TEST_F(StylePropertySymbolMapTest, RejectsInvalidResultsAndInputs)
{
    StylePropertySymbolMap map{&findStylePropertySymbol};
    callState.result = 0xFF;
    EXPECT_FALSE(map.findSymbol("opacity").isValid());
    callState.leaveOutputUnchanged = true;
    EXPECT_FALSE(map.findSymbol("opacity").isValid());
    EXPECT_EQ(callState.initialOutput, cs2::CStyleSymbol::kInvalidId);
    callState.leaveOutputUnchanged = false;
    callState.result = 0x00;
    callState.returnWrongPointer = true;
    EXPECT_FALSE(map.findSymbol("opacity").isValid());

    StylePropertySymbolMap missingMap;
    EXPECT_FALSE(missingMap.findSymbol("opacity").isValid());
    const auto callsBeforeInvalidNames = callState.calls;
    EXPECT_FALSE(map.findSymbol(nullptr).isValid());
    EXPECT_FALSE(map.findSymbol("").isValid());
    EXPECT_EQ(callState.calls, callsBeforeInvalidNames);
}

}
#endif
