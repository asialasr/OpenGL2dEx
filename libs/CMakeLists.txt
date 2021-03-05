# freetype
target_include_directories(OpenGL2dEx PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/freetype-windows-binaries/include)
target_link_libraries(OpenGL2dEx ${CMAKE_CURRENT_LIST_DIR}/freetype-windows-binaries/win32/freetype.lib)
# TODO dll for freetype

# GLAD
add_subdirectory(glad)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
target_link_libraries(OpenGL2dEx glfw)
# TODO: see GLFW readme for building apps - may need OpenGL

# GLM
set(glm_DIR ${CMAKE_CURRENT_LIST_DIR}/glm/cmake/glm)
find_package(glm REQUIRED)
target_link_libraries(OpenGL2dEx glm::glm)

# irrklang
target_include_directories(OpenGL2dEx PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/irrklang/include)
target_link_libraries(OpenGL2dEx ${CMAKE_CURRENT_LIST_DIR}/irrklang/lib/Win32-visualStudio/irrklang.lib)
# TODO irrklang dlls

# stb
target_include_directories(OpenGL2dEx PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/stb)