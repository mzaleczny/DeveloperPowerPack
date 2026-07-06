@echo off

REM PrerequisitiesBeforeBuild.bat

emcmake cmake ^
    -DCMAKE_PREFIX_PATH="D:/wasmtilc" ^
    -DCMAKE_INSTALL_PREFIX=D:/wasmtilc ^
    -DCMAKE_MODULE_PATH=D:/wasmtilc/lib/cmake ^
    -DSDL3_DIR="d:\wasmtilc\lib\cmake\SDL3" ^
    -DFreetype_DIR=D:/wasmtilc/lib/cmake/Freetype ^
    -DCMAKE_BUILD_TYPE=MinSizeRel -S . -B outemscripten

REM cmake --build outemscripten -j8
REM cmake --install outemscripten

REM copy out\external\freetype-build\libfreetype.a ..\..\lib\libfreetype.a
REM copy out\external\harfbuzz-build\libharfbuzz.a ..\..\lib\libharfbuzz.a
REM copy out\external\plutosvg-guild\libplutosvg.a ..\..\lib\libplutosvg.a
REM copy out\external\plutovg-build\libplutovg.a ..\..\lib\libplutovg.a
