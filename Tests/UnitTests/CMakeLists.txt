add_executable(UnitTests)

if(MSVC)
  target_compile_options(UnitTests PRIVATE "/MP")
endif()

target_include_directories(UnitTests PRIVATE "${CMAKE_SOURCE_DIR}/Tests" "${CMAKE_SOURCE_DIR}/Source")
target_link_libraries(UnitTests gtest_main gmock Threads::Threads)
set_target_properties(UnitTests PROPERTIES OUTPUT_NAME UnitTestsBin)
gtest_discover_tests(UnitTests DISCOVERY_TIMEOUT 60 TEST_PREFIX Unit_)

add_subdirectory(Config)
add_subdirectory(Features)
add_subdirectory(GameClient)
add_subdirectory(MemorySearch)
add_subdirectory(Platform)
add_subdirectory(Utils)
