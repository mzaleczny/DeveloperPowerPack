@echo off

mkdir .cache

copy .cache\SDL3_ttf\CMakeLists.txt .cache\SDL3_ttf\CMakeLists.txt.bak
copy SDL3_ttf_PATCHED_CMakeLists.txt .cache\SDL3_ttf\CMakeLists.txt

SET OUT_MARIADB_RELEASE=out\Release\mariadb
IF EXIST "%OUT_MARIADB_RELEASE%" (
    rmdir /S /Q "%OUT_MARIADB_RELEASE%"
)
mkdir %OUT_MARIADB_RELEASE%

SET OUT_MARIADB_DEBUG=out\Debug\mariadb
IF EXIST "%OUT_MARIADB_DEBUG%" (
    rmdir /S /Q "%OUT_MARIADB_DEBUG%"
)
mkdir %OUT_MARIADB_DEBUG%


REM **************************************************************************************************************************************************************************
REM Buid libmaridb Debug and Release
REM **************************************************************************************************************************************************************************
cd .cache
git clone --recurse-submodules -j4 https://github.com/mariadb-corporation/mariadb-connector-cpp.git
cd mariadb-connector-cpp

cmake -DWITH_MSI=NO -DWITH_WININSTALL=NO -DWITH_PACKAGE=NO -DWITH_INSTALL=NO -DWITH_DOCS=NO -DWITH_EXAMPLES=NO -DWITH_TESTS=NO -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-I./libmariadb/include" -DCMAKE_INSTALL_PREFIX="..\..\%OUT_MARIADB_RELEASE%\install" -S . -B "..\..\%OUT_MARIADB_RELEASE%"
cmake --build "..\..\%OUT_MARIADB_RELEASE%" --config Release
cmake --build "..\..\%OUT_MARIADB_RELEASE%" --target install

cmake -DWITH_MSI=NO -DWITH_WININSTALL=NO -DWITH_PACKAGE=NO -DWITH_INSTALL=NO -DWITH_DOCS=NO -DWITH_EXAMPLES=NO -DWITH_TESTS=NO -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-I./libmariadb/include" -DCMAKE_INSTALL_PREFIX="..\..\%OUT_MARIADB_DEBUG%\install" -S . -B "..\..\%OUT_MARIADB_DEBUG%"
cmake --build "..\..\%OUT_MARIADB_DEBUG%" --config Debug
cmake --build "..\..\%OUT_MARIADB_DEBUG%" --target install

cd ..\..


REM **************************************************************************************************************************************************************************
REM Buid fcgi Release
REM **************************************************************************************************************************************************************************
cd .cache
git clone --recurse-submodules -j4 https://github.com/FastCGI-Archives/fcgi2.git
cd fcgi2
REM lib will be created in libfcgi/Release
nmake -f Makefile.nt CFG=release
REM lib will be created in libfcgi/Debug
nmake -f Makefile.nt CFG=debug

cd ..\..

