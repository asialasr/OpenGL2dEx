#!/bin/bash

# TODO(sasiala): this is a quickly (and poorly) written script.
# I need to learn more about scripting & improve this greatly.

# nuclear option - should require flag
git submodule foreach 'git co .'
git submodule foreach 'git clean -df'

# update submodule
git submodule update --init --recursive

# build glfw
cd libs/glfw
cmake CMakeLists.txt
cmake --build .
cd ../..
