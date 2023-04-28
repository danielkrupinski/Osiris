if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  add_subdirectory(Linux)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  add_subdirectory(Windows)
endif()
