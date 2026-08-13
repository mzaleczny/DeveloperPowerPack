#!/bin/bash

mkdir .cache

if grep -q "BUILD_WITH_MARIADB \"1\"" ./TilcConfigure.cmake; then
    OUT_MARIADB_RELEASE=./out/Release/mariadb
    if [ -d "$OUT_MARIADB_RELEASE" ]; then
        rm -rf "$OUT_MARIADB_RELEASE"
    fi
    mkdir "$OUT_MARIADB_RELEASE"

    OUT_MARIADB_DEBUG=./out/Debug/mariadb
    if [ -d "$OUT_MARIADB_DEBUG" ]; then
        rm -rf "$OUT_MARIADB_DEBUG"
    fi
    mkdir "$OUT_MARIADB_DEBUG"


# **************************************************************************************************************************************************************************
# Buid libmaridb Debug and Release
# **************************************************************************************************************************************************************************
    cd .cache
#    git clone --recurse-submodules -j4 https://github.com/mariadb-corporation/mariadb-connector-cpp.git
    git clone --recurse-submodules -j4 https://github.com/mariadb-corporation/mariadb-connector-c.git
#    cd mariadb-connector-cpp
    cd mariadb-connector-c

    cmake -DWITH_MSI=NO \
      -DWITH_WININSTALL=NO \
      -DWITH_PACKAGE=NO \
      -DWITH_INSTALL=NO \
      -DWITH_DOCS=NO \
      -DWITH_EXAMPLES=NO \
      -DWITH_TESTS=NO \
      -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
      -DWITH_UNIT_TESTS=OFF \
      -DWITH_CURL=OFF \
      -DWITH_FCGI=OFF \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_FLAGS="-I./libmariadb/include" \
      -DCMAKE_INSTALL_PREFIX="../../$OUT_MARIADB_RELEASE/install" \
      -S . -B "../../$OUT_MARIADB_RELEASE"

    cmake --build "../../$OUT_MARIADB_RELEASE" --config Release
    cmake --build "../../$OUT_MARIADB_RELEASE" --target install

    cmake -DWITH_MSI=NO \
      -DWITH_WININSTALL=NO \
      -DWITH_PACKAGE=NO \
      -DWITH_INSTALL=NO \
      -DWITH_DOCS=NO \
      -DWITH_EXAMPLES=NO \
      -DWITH_TESTS=NO \
      -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
      -DWITH_UNIT_TESTS=OFF \
      -DWITH_CURL=OFF \
      -DWITH_FCGI=OFF \
      -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_CXX_FLAGS="-I./libmariadb/include" \
      -DCMAKE_INSTALL_PREFIX="../../$OUT_MARIADB_DEBUG/install" \
      -S . -B "../../$OUT_MARIADB_DEBUG"
    cmake --build "../../$OUT_MARIADB_DEBUG" --config Debug
    cmake --build "../../$OUT_MARIADB_DEBUG" --target install

    cd ../..
fi;


# **************************************************************************************************************************************************************************
# Buid fcgi Release
# **************************************************************************************************************************************************************************
#cd .cache
#git clone --recurse-submodules -j4 https://github.com/FastCGI-Archives/fcgi2.git
#cd fcgi2
#REM lib will be created in libfcgi/Release
#nmake -f Makefile.nt CFG=release
#REM lib will be created in libfcgi/Debug
#nmake -f Makefile.nt CFG=debug

#cd ..\..

