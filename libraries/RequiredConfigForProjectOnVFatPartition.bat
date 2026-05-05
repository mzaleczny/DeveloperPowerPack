@echo off

SET OUTDIR=%CD:\=/%/out
SET SAFEDIR_ZLIB_DEBUG=%OUTDIR%/build/x64-Debug/_deps/external_zlib-src
SET SAFEDIR_ZLIB_RELEASE=%OUTDIR%/build/x64-Release/_deps/external_zlib-src
mkdir "%SAFEDIR_ZLIB_DEBUG%" 2>nul
mkdir "%SAFEDIR_ZLIB_RELEASE%" 2>nul
git config --global --get safe.directory "%SAFEDIR_ZLIB_DEBUG%" >nul
if NOT %ERRORLEVEL% equ 0 (
    git config --global --add safe.directory "%SAFEDIR_ZLIB_DEBUG%"
)
git config --global --get safe.directory "%SAFEDIR_ZLIB_RELEASE%" >nul
if NOT %ERRORLEVEL% equ 0 (
    git config --global --add safe.directory "%SAFEDIR_ZLIB_RELEASE%"
)



SET SAFEDIR_GLM_DEBUG=%OUTDIR%/build/x64-Debug/_deps/external_glm-src
SET SAFEDIR_GLM_RELEASE=%OUTDIR%/build/x64-Release/_deps/external_glm-src
mkdir "%SAFEDIR_GLM_DEBUG%" 2>nul
mkdir "%SAFEDIR_GLM_RELEASE%" 2>nul
git config --global --get safe.directory "%SAFEDIR_GLM_DEBUG%" >nul
if NOT %ERRORLEVEL% equ 0 (
    git config --global --add safe.directory "%SAFEDIR_GLM_DEBUG%"
)
git config --global --get safe.directory "%SAFEDIR_GLM_RELEASE%" >nul
if NOT %ERRORLEVEL% equ 0 (
    git config --global --add safe.directory "%SAFEDIR_GLM_RELEASE%"
)



SET SAFEDIR_ASSIMP_DEBUG=%OUTDIR%/build/x64-Debug/_deps/external_assimp-src
SET SAFEDIR_ASSIMP_RELEASE=%OUTDIR%/build/x64-Release/_deps/external_assimp-src
mkdir "%SAFEDIR_ASSIMP_DEBUG%" 2>nul
mkdir "%SAFEDIR_ASSIMP_RELEASE%" 2>nul
git config --global --get safe.directory "%SAFEDIR_ASSIMP_DEBUG%" >nul
if NOT %ERRORLEVEL% equ 0 (
    git config --global --add safe.directory "%SAFEDIR_ASSIMP_DEBUG%"
)
git config --global --get safe.directory "%SAFEDIR_ASSIMP_RELEASE%" >nul
if NOT %ERRORLEVEL% equ 0 (
    git config --global --add safe.directory "%SAFEDIR_ASSIMP_RELEASE%"
)
