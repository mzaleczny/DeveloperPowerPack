cl -MD -EHsc -std:c++latest -I f:/libs/google/googletest/googletest/include/ -I .. -I ../.cache/assimp/contrib/googletest/googletest/include f:/libs/google/googletest/out/build/x64-Release/lib/gtest.lib f:/libs/google/googletest/out/build/x64-Release/lib/gtest_main.lib ../out/build/x64-Debug/TilcSharedd.lib "%1" main.cpp
del *.obj

copy ..\out\build\x64-Debug\TilcSharedd.dll TilcSharedd.dll
copy ..\out\build\x64-Debug\external_assimp-build\bin\assimpd.dll assimpd.dll
copy ..\out\build\x64-Debug\external_sdl3_image-build\SDL3_image.dll SDL3_image.dll
copy ..\out\build\x64-Debug\external_sdl3_ttf-build\SDL3_ttf.dll SDL3_ttf.dll
copy ..\out\build\x64-Debug\external_sdl3_mixer-build\SDL3_mixer.dll SDL3_mixer.dll
copy ..\out\build\x64-Debug\external_sdl3-build\SDL3.dll SDL3.dll
