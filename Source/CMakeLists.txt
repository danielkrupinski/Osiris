add_library(Osiris SHARED dllmain.cpp)

set_target_properties(Osiris PROPERTIES
  CXX_VISIBILITY_PRESET hidden
  C_VISIBILITY_PRESET hidden
  POSITION_INDEPENDENT_CODE ON)

target_include_directories(Osiris PRIVATE "${CMAKE_SOURCE_DIR}/Source")
target_link_libraries(Osiris PRIVATE ${CMAKE_DL_LIBS})

if(MSVC)
  string(REGEX REPLACE "/EH[a-z]+" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  target_sources(Osiris PRIVATE $<$<CONFIG:Release>:Intrinsics.cpp>)
  target_compile_options(Osiris PRIVATE /W4 $<$<CONFIG:Release>:/sdl- /GS->)
  target_link_options(Osiris PRIVATE $<$<CONFIG:Release>:/nodefaultlib /ENTRY:"DllMain">)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(NOT CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(Osiris PRIVATE -Wall)
    target_link_options(Osiris PRIVATE LINKER:--no-undefined)
  endif()

  target_compile_options(Osiris PRIVATE -Wno-missing-braces)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  if(UNIX)
    target_compile_options(Osiris PRIVATE -fno-stack-protector -fno-exceptions -fno-asynchronous-unwind-tables -fno-unwind-tables)
    target_link_options(Osiris PRIVATE -nostdlib)
    target_link_libraries(Osiris PRIVATE c)
  endif()
endif()
