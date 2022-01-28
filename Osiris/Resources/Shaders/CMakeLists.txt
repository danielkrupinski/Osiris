add_custom_target(Shaders
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Build/default_vs.h
)

set(FXC "$ENV{ProgramFiles\(x86\)}\\Windows Kits\\10\\bin\\${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}\\x86\\fxc.exe")

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/Build/default_vs.h
  COMMAND FXC ${CMAKE_CURRENT_SOURCE_DIR}/default_vs.hlsl /Vn default_vs /T vs_2_0 /Fh ${CMAKE_CURRENT_SOURCE_DIR}/Build/default_vs.h
  DEPENDS default_vs.hlsl
  VERBATIM)
