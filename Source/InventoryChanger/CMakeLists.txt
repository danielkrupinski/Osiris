target_sources(OsirisOBJ PRIVATE
  EconItemFunctions.cpp
  EconItemViewFunctions.cpp
  InventoryChanger.cpp
  InventoryConfig.cpp
  WeaponNames.cpp
)

add_subdirectory(Backend)
add_subdirectory(GameIntegration)
add_subdirectory(GameItems)
add_subdirectory(Inventory)
add_subdirectory(ItemGenerator)
