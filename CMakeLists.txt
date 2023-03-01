cmake_minimum_required(VERSION 3.16.0)
project(Osiris)

if (NOT WIN32)
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wfatal-errors")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fno-rtti")
endif()

add_compile_options($<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU>,$<C_COMPILER_ID:GNU>>:-s>)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} --no-gnu-unique")
endif()

if (MSVC)
  add_compile_options(/MP)
endif()

add_library(OsirisOBJ OBJECT "")
set_target_properties(OsirisOBJ PROPERTIES CXX_VISIBILITY_PRESET hidden C_VISIBILITY_PRESET hidden)
if (WIN32)
  target_compile_definitions(OsirisOBJ PUBLIC NOMINMAX)
endif()

add_subdirectory(Dependencies)
add_subdirectory(Source)

set_target_properties(OsirisOBJ PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_precompile_headers(OsirisOBJ PRIVATE "${CMAKE_SOURCE_DIR}/Source/pch.h")
target_include_directories(OsirisOBJ PUBLIC "${CMAKE_SOURCE_DIR}/Source")
target_include_directories(OsirisOBJ PUBLIC "${CMAKE_SOURCE_DIR}/Dependencies")

if (NOT WIN32)
  target_link_libraries(OsirisOBJ PRIVATE ${CMAKE_DL_LIBS})
  target_compile_options(OsirisOBJ PRIVATE -flto)
  target_link_options(OsirisOBJ PUBLIC -flto LINKER:--no-undefined)
else()
  target_compile_definitions(OsirisOBJ PRIVATE _USE_MATH_DEFINES)
  target_compile_options(OsirisOBJ PRIVATE /utf-8)
  target_link_libraries(OsirisOBJ PRIVATE ntdll)
  add_subdirectory(Source/Resources/Shaders)
  add_dependencies(OsirisOBJ Shaders)
endif()

add_library(Osiris SHARED "${CMAKE_SOURCE_DIR}/Source/Osiris.cpp")
target_link_libraries(Osiris PUBLIC OsirisOBJ)

if (NOT WIN32)
  target_link_options(Osiris PRIVATE -flto)
else()
  target_link_options(Osiris PRIVATE /ENTRY:DllEntryPoint)
endif()

option(BUILD_TESTS "Build unit tests" OFF)

if (BUILD_TESTS)
  enable_testing()
  add_subdirectory(Tests)
endif()
