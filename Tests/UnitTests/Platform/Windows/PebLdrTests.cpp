#include <string>

#include <gtest/gtest.h>

#include "PebLdrBuilder.h"

namespace
{

class TestPebLdr : public testing::Test {
protected:
    void addDummyModule1()
    {
        builder.addModule((void*)0x1234123412341234, L"dllone");
    }

    void addDummyModule2()
    {
        builder.addModule((void*)0x0BADC0DE0BADC0DE, L"another.dll");
    }

    PebLdrBuilder builder;
};

TEST_F(TestPebLdr, NullHandleIsReturnedWhenModuleIsNotFound) {
    addDummyModule1();
    addDummyModule2();
    EXPECT_EQ(builder.pebLdr().getModuleHandle("client.dll"), nullptr);
}

class TestPebLdr_Handle_Name
    : public TestPebLdr, public testing::WithParamInterface<std::tuple<void*, std::string>> {};

TEST_P(TestPebLdr_Handle_Name, FirstModuleInListCanBeFound) {
    const auto [handle, dllName] = GetParam();
    const std::wstring dllNameUnicode{ dllName.begin(), dllName.end() };
    builder.addModule(handle, dllNameUnicode.c_str());

    addDummyModule1();
    addDummyModule2();

    EXPECT_EQ(builder.pebLdr().getModuleHandle(dllName.c_str()), handle);
}

TEST_P(TestPebLdr_Handle_Name, LastModuleInListCanBeFound) {
    addDummyModule1();
    addDummyModule2();

    const auto [handle, dllName] = GetParam();
    const std::wstring dllNameUnicode{ dllName.begin(), dllName.end() };
    builder.addModule(handle, dllNameUnicode.c_str());

    EXPECT_EQ(builder.pebLdr().getModuleHandle(dllName.c_str()), handle);
}

TEST_P(TestPebLdr_Handle_Name, ModuleInTheCenterOfListCanBeFound) {
    addDummyModule1();

    const auto [handle, dllName] = GetParam();
    const std::wstring dllNameUnicode{ dllName.begin(), dllName.end() };
    builder.addModule(handle, dllNameUnicode.c_str());

    addDummyModule2();

    EXPECT_EQ(builder.pebLdr().getModuleHandle(dllName.c_str()), handle);
}

INSTANTIATE_TEST_SUITE_P(, TestPebLdr_Handle_Name, testing::Combine(
    testing::Values((void*)0xABABABABABABABAB, (void*)0xCDCDCDCDCDCDCDCD),
    testing::Values("client.dll", "engine.dll")
));

}
