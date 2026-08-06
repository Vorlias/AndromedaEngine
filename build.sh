#!/bin/bash


cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DOPENGL_RENDERER=OFF -DBUILTIN_SHADER_COMPILER=ON
cmake --build build --config Release