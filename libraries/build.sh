#!/bin/bash

./PrerequisitiesBeforeBuild.sh
cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=Debug -S . -B out
cmake --build out --config Debug
