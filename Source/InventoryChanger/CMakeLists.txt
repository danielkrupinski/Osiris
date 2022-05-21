target_sources(OsirisOBJ PRIVATE
  InventoryChanger.cpp
  InventoryConfig.cpp
  ItemGenerator/ItemGenerator.cpp
  StaticData.cpp
  ItemGenerator/TournamentMatches.cpp
  GameIntegration/Inventory.cpp
  GameIntegration/Items.cpp
)

add_subdirectory(Backend)
add_subdirectory(GameItems)
add_subdirectory(Inventory)
