@echo off

SET OUTDIR=%CD:\=/%/out

call AddDirToGitSafeDirectory.bat "%OUTDIR%/build/x64-Debug/_deps/external_zlib-src"
call AddDirToGitSafeDirectory.bat "%OUTDIR%/build/x64-Release/_deps/external_zlib-src"

call AddDirToGitSafeDirectory.bat "%OUTDIR%/build/x64-Debug/_deps/external_libpng-src"
call AddDirToGitSafeDirectory.bat "%OUTDIR%/build/x64-Release/_deps/external_libpng-src"

call AddDirToGitSafeDirectory.bat "%OUTDIR%/build/x64-Debug/_deps/external_sdl3-src"
call AddDirToGitSafeDirectory.bat "%OUTDIR%/build/x64-Release/_deps/external_sdl3-src"

call AddDirToGitSafeDirectory.bat "%OUTDIR%/build/x64-Debug/_deps/external_sdl3_image-src"
call AddDirToGitSafeDirectory.bat "%OUTDIR%/build/x64-Release/_deps/external_sdl3_image-src"

call AddDirToGitSafeDirectory.bat "%OUTDIR%/build/x64-Debug/_deps/external_sdl3_mixer-src"
call AddDirToGitSafeDirectory.bat "%OUTDIR%/build/x64-Release/_deps/external_sdl3_mixer-src"

call AddDirToGitSafeDirectory.bat "%OUTDIR%/build/x64-Debug/_deps/external_sdl3_ttf-src"
call AddDirToGitSafeDirectory.bat "%OUTDIR%/build/x64-Release/_deps/external_sdl3_ttf-src"

call AddDirToGitSafeDirectory.bat "%OUTDIR%/build/x64-Debug/_deps/external_glm-src"
call AddDirToGitSafeDirectory.bat "%OUTDIR%/build/x64-Release/_deps/external_glm-src"

call AddDirToGitSafeDirectory.bat "%OUTDIR%/build/x64-Debug/_deps/external_assimp-src"
call AddDirToGitSafeDirectory.bat "%OUTDIR%/build/x64-Release/_deps/external_assimp-src"

call AddDirToGitSafeDirectory.bat "%OUTDIR%/build/x64-Debug/_deps/external_mariadbcpp-src"
call AddDirToGitSafeDirectory.bat "%OUTDIR%/build/x64-Release/_deps/external_mariadbcpp-src"
