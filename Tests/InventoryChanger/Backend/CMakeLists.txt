target_sources(Tests PRIVATE
  PickEmTests.cpp
  ResponseQueueTests.cpp
  Response/ItemInResponseTests.cpp
  StorageUnitManagerTests.cpp
  XRayScannerTests.cpp
)

add_subdirectory(Request)
