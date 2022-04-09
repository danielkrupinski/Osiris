target_sources(OsirisOBJ PRIVATE
  InventoryChanger.cpp
  InventoryConfig.cpp
  ItemGenerator.cpp
  StaticData.cpp
  GameItems/Storage.cpp
  Backend/ToolUser.cpp
  BackendResponseHandler.cpp
)

add_subdirectory(Inventory)
