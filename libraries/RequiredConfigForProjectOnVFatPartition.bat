@echo off

SET OUTDIR=%CD:\=/%/out
SET SAFEDIR_ZLIB_DEBUG=%OUTDIR%/build/x64-Debug/_deps/external-zlib-src
SET SAFEDIR_ZLIB_RELEASE=%OUTDIR%/build/x64-Release/_deps/external-zlib-src
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
