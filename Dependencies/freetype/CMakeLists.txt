target_sources(OsirisOBJ PRIVATE
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

target_compile_definitions(OsirisOBJ PRIVATE FT2_BUILD_LIBRARY)
target_include_directories(OsirisOBJ PRIVATE .)
