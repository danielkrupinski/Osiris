target_sources(OsirisOBJ PRIVATE
  Config.cpp
  ConfigStructs.cpp
  Endpoints.cpp
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

add_subdirectory(CSGO)
add_subdirectory(Hacks)
add_subdirectory(Helpers)
add_subdirectory(Hooks)
add_subdirectory(InventoryChanger)
add_subdirectory(MemorySearch)
add_subdirectory(RetSpoof)
