cmake_minimum_required(VERSION 3.16.0)
project(Osiris)

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fvisibility=hidden -fno-exceptions -Wfatal-errors")

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fno-rtti")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} --no-gnu-unique")
endif()

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/Osiris/*.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/Osiris/imgui/imgui_impl_dx9.cpp") 
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/Osiris/imgui/imgui_impl_win32.cpp") 
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/Osiris/Hooks/MinHook.cpp") 
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/Osiris/pch.cpp") 
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/Osiris/Osiris.cpp")

find_package(Freetype REQUIRED)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

add_library(OsirisOBJ OBJECT ${SOURCES})
set_target_properties(OsirisOBJ PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(OsirisOBJ PRIVATE ${SDL2_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS})
target_precompile_headers(OsirisOBJ PRIVATE "${CMAKE_SOURCE_DIR}/Osiris/pch.h")
target_link_libraries(OsirisOBJ PRIVATE SDL2 GL ${CMAKE_DL_LIBS} Freetype::Freetype)
target_compile_options(OsirisOBJ PRIVATE -flto)
target_link_options(OsirisOBJ PRIVATE -flto)

add_library(Osiris SHARED $<TARGET_OBJECTS:OsirisOBJ> "${CMAKE_SOURCE_DIR}/Osiris/Osiris.cpp")
target_link_options(Osiris PRIVATE -flto)

option(BUILD_TESTS "Build unit tests" OFF)

if (BUILD_TESTS)
  enable_testing()
  add_subdirectory(Tests)
endif()
