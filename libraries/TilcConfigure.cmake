set(FORCE_OPENGL_ES "0")
set(BUILD_WITH_MYSQL "0")
set(BUILD_FOR_SERVER "0")

set_property(GLOBAL PROPERTY Tilc_HEADERS_Property "")
set_property(GLOBAL PROPERTY Tilc_SOURCES_Property "")
set_property(GLOBAL PROPERTY Tilc_PLATFORM_SPECIFIC_HEADERS_Property "")
set_property(GLOBAL PROPERTY Tilc_PLATFORM_SPECIFIC_SOURCES_Property "")
set_property(GLOBAL PROPERTY BuildWithMySQL BUILD_WITH_MYSQL)
set_property(GLOBAL PROPERTY BuildForServer BUILD_FOR_SERVER)


configure_file(Tilc/configure.h.in configure.h)
