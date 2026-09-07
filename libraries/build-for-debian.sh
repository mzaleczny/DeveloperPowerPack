#!/bin/bash

./PrerequisitiesBeforeBuild.sh
cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DSDL_X11_XTEST=OFF -DCMAKE_BUILD_TYPE=Release -S . -B out
cmake --build out --config Release
