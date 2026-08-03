set(BUILD_WITH_OPENGL "0")
set(FORCE_OPENGL_ES "0")
set(BUILD_WITH_MYSQL "1")
set(BUILD_WITHOUT_GRAPHICS "1")

set_property(GLOBAL PROPERTY Tilc_HEADERS_Property "")
set_property(GLOBAL PROPERTY Tilc_SOURCES_Property "")
set_property(GLOBAL PROPERTY Tilc_PLATFORM_SPECIFIC_HEADERS_Property "")
set_property(GLOBAL PROPERTY Tilc_PLATFORM_SPECIFIC_SOURCES_Property "")
set_property(GLOBAL PROPERTY BuildWithMySQL BUILD_WITH_MYSQL)
set_property(GLOBAL PROPERTY BuildWithoutGraphics BUILD_WITHOUT_GRAPHICS)

set(CURL_VERSION "curl-8.21.0_5-win64-mingw")
set(URL_CURL_PRECOMPILED_BINARIES "https://curl.se/windows/dl-8.21.0_5/${CURL_VERSION}.zip")
set(URL_CURL_EXPECTED_HASH "4c48761e9b70f447af76e65564cf2afbbf626e4ea6286008fde5cc068de237fd")

# Below is location of lib.exe file needed to generate lib file for dll file. It is used for example for genrating curl.lib for curl.dll.
set(VS_BINARIES "c:/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.51.36231/bin/Hostx64/x64")

configure_file(Tilc/configure.h.in configure.h)
