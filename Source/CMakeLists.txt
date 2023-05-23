if (NOT WIN32)
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wfatal-errors")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fno-rtti")
endif()

add_compile_options($<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU>,$<C_COMPILER_ID:GNU>>:-s>)

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

set_target_properties(OsirisOBJ PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_precompile_headers(OsirisOBJ PRIVATE pch.h)
target_include_directories(OsirisOBJ PUBLIC .)
target_include_directories(OsirisOBJ PUBLIC ../Dependencies)

if (NOT WIN32)
  target_link_libraries(OsirisOBJ PRIVATE ${CMAKE_DL_LIBS})
  target_link_options(OsirisOBJ PUBLIC LINKER:--no-undefined)
else()
  target_compile_definitions(OsirisOBJ PRIVATE _USE_MATH_DEFINES)
  target_compile_options(OsirisOBJ PRIVATE /utf-8)
  target_link_libraries(OsirisOBJ PRIVATE ntdll)
  add_subdirectory(Resources/Shaders)
  add_dependencies(OsirisOBJ Shaders)
endif()

add_library(Osiris SHARED Osiris.cpp)
target_link_libraries(Osiris PUBLIC OsirisOBJ)

if (WIN32)
  target_link_options(Osiris PRIVATE /ENTRY:DllEntryPoint)
endif()

target_sources(OsirisOBJ PRIVATE
  Config.cpp
  ConfigStructs.cpp
  EventListener.cpp
  GameData.cpp
  GlobalContext.cpp
  GUI.cpp
  Helpers.cpp
  Hooks.cpp
  imguiCustom.cpp
  InputUtil.cpp
  Netvars.cpp
  Texture.cpp
)

add_subdirectory(CSGO)
add_subdirectory(Hacks)
add_subdirectory(Helpers)
add_subdirectory(Hooks)
add_subdirectory(InventoryChanger)
add_subdirectory(MemorySearch)
add_subdirectory(RetSpoof)
