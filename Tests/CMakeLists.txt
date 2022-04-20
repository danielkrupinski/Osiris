set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
  URL_HASH SHA256=353571c2440176ded91c2de6d6cd88ddd41401d14692ec1f99e35d013feda55a
)

FetchContent_MakeAvailable(googletest)

add_executable(Tests HelpersTests.cpp)
add_subdirectory(InventoryChanger)
target_link_libraries(Tests gtest_main gmock Threads::Threads OsirisOBJ)

include(GoogleTest)
gtest_discover_tests(Tests)
