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
  Memory.cpp
  Netvars.cpp
  Texture.cpp
)

add_subdirectory(Hacks)
add_subdirectory(Hooks)
add_subdirectory(InventoryChanger)
add_subdirectory(SDK)
