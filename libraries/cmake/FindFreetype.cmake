set(FREETYPE_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/freetype/../../../include")
set(FREETYPE_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/freetype/../../../lib/libfreetype.a")

include("${CMAKE_CURRENT_LIST_DIR}/freetype/freetypeConfig.cmake")

# MODULE mode variables expected by FREETYPE
set(FREETYPE_FOUND TRUE)
