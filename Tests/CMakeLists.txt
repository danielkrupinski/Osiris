if("unit" IN_LIST ENABLE_TESTS OR "functional" IN_LIST ENABLE_TESTS)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)

  include(FetchContent)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG fa8438ae6b70c57010177de47a9f13d7041a6328 # v1.17.0 with fix for Clang 21 warning -Wcharacter-conversion
    # GIT_SHALLOW TRUE # re-enable when new gtest release tag (not just a commit hash) is used above (https://gitlab.kitware.com/cmake/cmake/-/issues/23379)
  )
  FetchContent_MakeAvailable(googletest)

  include(GoogleTest)
endif()

if("unit" IN_LIST ENABLE_TESTS)
  add_subdirectory(UnitTests)
endif()

if("functional" IN_LIST ENABLE_TESTS)
  add_subdirectory(FunctionalTests)
endif()
