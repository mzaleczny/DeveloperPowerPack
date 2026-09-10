set(TILC_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/../..")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(TILC_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/../../out/libTilcSharedd.so")
else()
    set(TILC_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/../../out/libTilcShared.so")
endif()

if(NOT TARGET Tilc::Tilc)
    add_library(Tilc::Tilc STATIC IMPORTED)
    set_target_properties(Tilc::Tilc PROPERTIES
        IMPORTED_LOCATION "${TILC_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${TILC_INCLUDE_DIR}"
    )
endif()
