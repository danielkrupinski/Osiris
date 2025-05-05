add_executable(FunctionalTests Config/ConfigCompatibilityTests.cpp)

target_include_directories(FunctionalTests PRIVATE "${CMAKE_SOURCE_DIR}/Tests" "${CMAKE_SOURCE_DIR}/Source")
target_compile_definitions(FunctionalTests PRIVATE TEST_CONFIGS_PATH="${CMAKE_SOURCE_DIR}/Tests/Configs")
target_link_libraries(FunctionalTests gtest_main gmock Threads::Threads)
set_target_properties(FunctionalTests PROPERTIES OUTPUT_NAME FunctionalTestsBin)
gtest_discover_tests(FunctionalTests DISCOVERY_TIMEOUT 60 TEST_PREFIX Functional_)
