cmake_minimum_required(VERSION 3.24)
project(Osiris LANGUAGES CXX ASM-ATT ASM_MASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(Source)

option(BUILD_TESTS "Build unit tests" OFF)

if (BUILD_TESTS)
  enable_testing()
  add_subdirectory(Tests)
endif()
