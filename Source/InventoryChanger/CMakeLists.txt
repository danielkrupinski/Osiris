target_sources(OsirisOBJ PRIVATE
  InventoryChanger.cpp
  InventoryConfig.cpp
  ItemGenerator.cpp
  StaticData.cpp
  BackendResponseHandler.cpp
  TournamentMatches.cpp
  GameIntegration/Inventory.cpp
)

add_subdirectory(Backend)
add_subdirectory(GameItems)
add_subdirectory(Inventory)
