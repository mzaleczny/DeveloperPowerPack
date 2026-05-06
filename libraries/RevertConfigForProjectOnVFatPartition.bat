@echo off

SET OUTDIR=%CD:\=/%/out

git config --global --unset safe.directory "%OUTDIR%/build/x64-Debug/_deps/external_zlib-src"
git config --global --unset safe.directory "%OUTDIR%/build/x64-Release/_deps/external_zlib-src"

git config --global --unset safe.directory "%OUTDIR%/build/x64-Debug/_deps/external_libpng-src"
git config --global --unset safe.directory "%OUTDIR%/build/x64-Release/_deps/external_libpng-src"

git config --global --unset safe.directory "%OUTDIR%/build/x64-Debug/_deps/external_sdl3-src"
git config --global --unset safe.directory "%OUTDIR%/build/x64-Release/_deps/external_sdl3-src"

git config --global --unset safe.directory "%OUTDIR%/build/x64-Debug/_deps/external_sdl3_image-src"
git config --global --unset safe.directory "%OUTDIR%/build/x64-Release/_deps/external_sdl3_image-src"

git config --global --unset safe.directory "%OUTDIR%/build/x64-Debug/_deps/external_sdl3_mixer-src"
git config --global --unset safe.directory "%OUTDIR%/build/x64-Release/_deps/external_sdl3_mixer-src"

git config --global --unset safe.directory "%OUTDIR%/build/x64-Debug/_deps/external_sdl3_ttf-src"
git config --global --unset safe.directory "%OUTDIR%/build/x64-Release/_deps/external_sdl3_ttf-src"

git config --global --unset safe.directory "%OUTDIR%/build/x64-Debug/_deps/external_glm-src"
git config --global --unset safe.directory "%OUTDIR%/build/x64-Release/_deps/external_glm-src"

git config --global --unset safe.directory "%OUTDIR%/build/x64-Debug/_deps/external_assimp-src"
git config --global --unset safe.directory "%OUTDIR%/build/x64-Release/_deps/external_assimp-src"

git config --global --unset safe.directory "%OUTDIR%/build/x64-Debug/_deps/external_mariadbcpp-src"
git config --global --unset safe.directory "%OUTDIR%/build/x64-Release/_deps/external_mariadbcpp-src"
