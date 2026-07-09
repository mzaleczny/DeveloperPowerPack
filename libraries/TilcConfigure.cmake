set(FORCE_OPENGL_ES "1")
set(BUILD_WITH_MYSQL "1")

set_property(GLOBAL PROPERTY Tilc_HEADERS_Property "")
set_property(GLOBAL PROPERTY Tilc_SOURCES_Property "")
set_property(GLOBAL PROPERTY Tilc_PLATFORM_SPECIFIC_HEADERS_Property "")
set_property(GLOBAL PROPERTY Tilc_PLATFORM_SPECIFIC_SOURCES_Property "")
set_property(GLOBAL PROPERTY BuildWithMySQL BUILD_WITH_MYSQL)


configure_file(Tilc/configure.h.in configure.h)
