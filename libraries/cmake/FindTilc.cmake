set(TILC_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/Tilc/../../../include")
set(TILC_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/Tilc/../../../lib/libTilcShared.so")

include("${CMAKE_CURRENT_LIST_DIR}/Tilc/TilcConfig.cmake")

# MODULE mode variables expected by FREETYPE
set(TILC_FOUND TRUE)
