@echo off

SET OUTDIR=%CD:\=/%/out
SET SAFEDIR_ZLIB_DEBUG=%OUTDIR%/build/x64-Debug/_deps/external-zlib-src
SET SAFEDIR_ZLIB_RELEASE=%OUTDIR%/build/x64-Release/_deps/external-zlib-src
git config --global --unset safe.directory "%SAFEDIR_ZLIB_DEBUG%"
git config --global --unset safe.directory "%SAFEDIR_ZLIB_RELEASE%"


SET SAFEDIR_GLM_DEBUG=%OUTDIR%/build/x64-Debug/_deps/glm-src
SET SAFEDIR_GLM_RELEASE=%OUTDIR%/build/x64-Release/_deps/glm-src
git config --global --unset safe.directory "%SAFEDIR_GLM_DEBUG%"
git config --global --unset safe.directory "%SAFEDIR_GLM_RELEASE%"
