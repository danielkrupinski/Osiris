add_compile_definitions(FT2_BUILD_LIBRARY)

add_library(FreeType STATIC
  src/autofit/autofit.c
  src/cff/cff.c
  src/base/ftbase.c
  src/base/ftbitmap.c
  src/base/ftdebug.c
  src/base/ftglyph.c
  src/base/ftinit.c
  src/base/ftsynth.c
  src/base/ftsystem.c
  src/psaux/psaux.c
  src/pshinter/pshinter.c
  src/psnames/psnames.c
  src/raster/raster.c
  src/sfnt/sfnt.c
  src/smooth/smooth.c
  src/truetype/truetype.c)

set_target_properties(FreeType PROPERTIES POSITION_INDEPENDENT_CODE ON)

include_directories(.)
