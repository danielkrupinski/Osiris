target_sources(Tests PRIVATE
  PickEmTests.cpp
  ResponseQueueTests.cpp
  Response/ItemInResponseTests.cpp
  XRayScannerTests.cpp
)

add_subdirectory(Request)
