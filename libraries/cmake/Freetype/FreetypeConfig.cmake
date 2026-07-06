set(FREETYPE_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../SDL_ttf/external/freetype/include")
set(FREETYPE_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/../../SDL_ttf/external/freetype/out/libfreetype.a")

if(NOT TARGET Freetype::Freetype)
    add_library(Freetype::Freetype STATIC IMPORTED)
    set_target_properties(Freetype::Freetype PROPERTIES
        IMPORTED_LOCATION "${FREETYPE_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${FREETYPE_INCLUDE_DIR}"
    )
endif()
