target_sources(OsirisOBJ PRIVATE
  InventoryChanger.cpp
  InventoryConfig.cpp
  StaticData.cpp
  GameIntegration/Inventory.cpp
  GameIntegration/Items.cpp
)

add_subdirectory(Backend)
add_subdirectory(GameItems)
add_subdirectory(Inventory)
add_subdirectory(ItemGenerator)
