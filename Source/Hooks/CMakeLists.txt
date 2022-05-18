target_sources(OsirisOBJ PRIVATE
  VmtSwap.cpp
)

if (WIN32)
  target_sources(OsirisOBJ PRIVATE
    MinHook.cpp
    VmtHook.cpp
  )
endif()
