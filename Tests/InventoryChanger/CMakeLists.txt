target_sources(Tests PRIVATE
  StaticDataTests.cpp
  ItemGeneratorTests.cpp
)

add_subdirectory(Backend)
add_subdirectory(GameItems)
add_subdirectory(Inventory)
add_subdirectory(ItemGenerator)
