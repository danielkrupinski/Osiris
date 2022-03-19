add_library(imgui STATIC
  imgui_demo.cpp
  imgui_draw.cpp
  imgui_freetype.cpp
  imgui_stdlib.cpp
  imgui_tables.cpp
  imgui_widgets.cpp
  imgui.cpp
)

if (WIN32)
  target_sources(imgui PRIVATE imgui_impl_dx9.cpp)
  target_sources(imgui PRIVATE imgui_impl_win32.cpp)
else()
  target_sources(imgui PRIVATE imgui_impl_opengl3.cpp)
  target_sources(imgui PRIVATE imgui_impl_sdl.cpp)

  find_package(Freetype REQUIRED)
  find_package(SDL2 REQUIRED)
  find_package(OpenGL REQUIRED)
  target_include_directories(imgui PUBLIC ${SDL2_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS})
  target_link_libraries(imgui PRIVATE SDL2 GL Freetype::Freetype)
endif()

set_target_properties(imgui PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(imgui PUBLIC FreeType)
