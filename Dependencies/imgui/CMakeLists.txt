target_sources(OsirisOBJ PRIVATE
  imgui_demo.cpp
  imgui_draw.cpp
  imgui_freetype.cpp
  imgui_stdlib.cpp
  imgui_tables.cpp
  imgui_widgets.cpp
  imgui.cpp
)

if (WIN32)
  target_sources(OsirisOBJ PRIVATE imgui_impl_dx9.cpp imgui_impl_win32.cpp)
else()
  target_sources(OsirisOBJ PRIVATE imgui_impl_opengl3.cpp imgui_impl_sdl.cpp)
  find_package(Freetype REQUIRED)
  find_package(SDL2 REQUIRED)
  find_package(OpenGL REQUIRED)
  target_include_directories(OsirisOBJ PUBLIC ${SDL2_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS})
  target_link_libraries(OsirisOBJ PRIVATE SDL2 GL Freetype::Freetype)
endif()
