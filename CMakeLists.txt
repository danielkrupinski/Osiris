cmake_minimum_required(VERSION 3.24)
project(Osiris LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
  enable_language(ASM_MASM)
elseif(UNIX)
  enable_language(ASM-ATT)
endif()

set(ENABLE_TESTS CACHE STRING "List of test types to enable e.g. \"unit;functional\"")

add_subdirectory(Source)

if(ENABLE_TESTS)
  enable_testing()
  add_subdirectory(Tests)
endif()
