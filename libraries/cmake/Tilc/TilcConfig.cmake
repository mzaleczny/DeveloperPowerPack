set(TILC_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/../..")
set(TILC_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/../../out/libTilcShared.so")

if(NOT TARGET Tilc::Tilc)
    add_library(Tilc::Tilc STATIC IMPORTED)
    set_target_properties(Tilc::Tilc PROPERTIES
        IMPORTED_LOCATION "${TILC_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${TILC_INCLUDE_DIR}"
    )
endif()
