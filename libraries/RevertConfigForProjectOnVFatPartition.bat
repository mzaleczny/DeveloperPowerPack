@echo off

SET OUTDIR=%CD:\=/%/out
SET SAFEDIR_ZLIB_DEBUG=%OUTDIR%/build/x64-Debug/_deps/external_zlib-src
SET SAFEDIR_ZLIB_RELEASE=%OUTDIR%/build/x64-Release/_deps/external_zlib-src
git config --global --unset safe.directory "%SAFEDIR_ZLIB_DEBUG%"
git config --global --unset safe.directory "%SAFEDIR_ZLIB_RELEASE%"


SET SAFEDIR_GLM_DEBUG=%OUTDIR%/build/x64-Debug/_deps/external_glm-src
SET SAFEDIR_GLM_RELEASE=%OUTDIR%/build/x64-Release/_deps/external_glm-src
git config --global --unset safe.directory "%SAFEDIR_GLM_DEBUG%"
git config --global --unset safe.directory "%SAFEDIR_GLM_RELEASE%"


SET SAFEDIR_ASSIMP_DEBUG=%OUTDIR%/build/x64-Debug/_deps/external_assimp-src
SET SAFEDIR_ASSIMP_RELEASE=%OUTDIR%/build/x64-Release/_deps/external_assimp-src
git config --global --unset safe.directory "%SAFEDIR_ASSIMP_DEBUG%"
git config --global --unset safe.directory "%SAFEDIR_ASSIMP_RELEASE%"
