target_sources(OsirisOBJ PRIVATE
  InventoryChanger.cpp
  InventoryConfig.cpp
  ItemGenerator.cpp
  StaticData.cpp
  GameItems/Storage.cpp
  BackendResponseHandler.cpp
)

add_subdirectory(Backend)
add_subdirectory(Inventory)
