@echo off

mkdir "%SAFEDIR_SDL3_DEBUG%" 2>nul
git config --global --get safe.directory "%1%" >nul
if NOT %ERRORLEVEL% equ 0 (
    git config --global --add safe.directory "%1%"
)
