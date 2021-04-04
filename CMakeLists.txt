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

# FreeType sources
list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/Osiris/freetype/src/autofit/autofit.c")
list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/Osiris/freetype/src/cff/cff.c")
list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/Osiris/freetype/src/base/ftbase.c")
list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/Osiris/freetype/src/base/ftbitmap.c")
list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/Osiris/freetype/src/base/ftdebug.c")
list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/Osiris/freetype/src/base/ftglyph.c")
list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/Osiris/freetype/src/base/ftinit.c")
list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/Osiris/freetype/src/base/ftsynth.c")
list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/Osiris/freetype/src/base/ftsystem.c")
list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/Osiris/freetype/src/psaux/psaux.c")
list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/Osiris/freetype/src/pshinter/pshinter.c")
list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/Osiris/freetype/src/psnames/psnames.c")
list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/Osiris/freetype/src/raster/raster.c")
list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/Osiris/freetype/src/sfnt/sfnt.c")
list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/Osiris/freetype/src/smooth/smooth.c")
list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/Osiris/freetype/src/truetype/truetype.c")

add_compile_definitions(FT2_BUILD_LIBRARY)

include_directories(/usr/local/include /usr/include/SDL2/ "${CMAKE_SOURCE_DIR}/Osiris/freetype/")
link_directories(/usr/local/lib)

add_library(Osiris SHARED ${SOURCES})

target_link_libraries(Osiris SDL2 GL dl)
target_link_options(Osiris PRIVATE LINKER:--no-undefined)
