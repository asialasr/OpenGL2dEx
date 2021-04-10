#!/bin/bash

# TODO(sasiala): this is a quickly (and poorly) written script.
# I need to learn more about scripting & improve this.

# nuclear option - should require flag
git submodule foreach 'git co .'
git submodule foreach 'git clean -dfx'

# update submodule
git submodule update --init --recursive

# build glfw
cd libs/glfw
cmake CMakeLists.txt
cmake --build .
cd ../..