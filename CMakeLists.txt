cmake_minimum_required(VERSION 3.13.0)
project(Osiris)

set(CMAKE_C_FLAGS_RELEASE "-O3 -fvisibility=hidden -flto -fno-exceptions -DNDEBUG -Wfatal-errors")

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fno-rtti")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} --no-gnu-unique")
endif()

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/Osiris/*.cpp")
list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/Osiris/imgui/GL/gl3w.c")
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/Osiris/imgui/imgui_impl_dx9.cpp") 
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/Osiris/imgui/imgui_impl_win32.cpp") 
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/Osiris/Hooks/MinHook.cpp") 

include_directories(/usr/local/include /usr/include/SDL2/)
link_directories(/usr/local/lib)

add_library(Osiris SHARED ${SOURCES})

target_link_libraries(Osiris SDL2 GL dl)
target_link_options(Osiris PRIVATE LINKER:--no-undefined)
