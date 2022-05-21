target_sources(OsirisOBJ PRIVATE
  InventoryChanger.cpp
  InventoryConfig.cpp
  ItemGenerator/ItemGenerator.cpp
  StaticData.cpp
  TournamentMatches.cpp
  GameIntegration/Inventory.cpp
  GameIntegration/Items.cpp
)

add_subdirectory(Backend)
add_subdirectory(GameItems)
add_subdirectory(Inventory)
