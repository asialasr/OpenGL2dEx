#!/bin/bash

# nuclear option - should require flag
git submodule foreach 'git co .'
git submodule foreach 'git clean -df'

# update submodule
env -i git submodule update --init --recursive

# build assimp
cd assimp
cmake CMakeLists.txt
cmake --build .
cd ..

# build glfw
cd glfw
cmake CMakeLists.txt
cmake --build .
cd ..

