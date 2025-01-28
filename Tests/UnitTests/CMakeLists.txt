add_executable(UnitTests UnitTests.cpp)

if(MSVC)
  target_compile_options(UnitTests PRIVATE /bigobj)
endif()

target_include_directories(UnitTests PRIVATE "${CMAKE_SOURCE_DIR}/Tests" "${CMAKE_SOURCE_DIR}/Source")
target_link_libraries(UnitTests gtest_main gmock Threads::Threads)
set_target_properties(UnitTests PROPERTIES OUTPUT_NAME UnitTestsBin)
gtest_discover_tests(UnitTests DISCOVERY_TIMEOUT 60)

add_subdirectory(PlatformTests)
