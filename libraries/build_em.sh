#!/bin/bash

emcmake cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -S . -B out
emcmake cmake --build out
