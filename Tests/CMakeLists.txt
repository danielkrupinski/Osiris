set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG f8d7d77c06936315286eb55f8de22cd23c188571 # v1.14.0
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(googletest)

add_executable(Tests "")
add_subdirectory(HelpersTests)
add_subdirectory(MemoryAllocation)
add_subdirectory(MemorySearchTests)
add_subdirectory(Platform)
add_subdirectory(UtilsTests)
target_link_libraries(Tests gtest_main gmock Threads::Threads)
target_include_directories(Tests PRIVATE . "${CMAKE_SOURCE_DIR}/Source")

include(GoogleTest)
gtest_discover_tests(Tests DISCOVERY_TIMEOUT 60)
