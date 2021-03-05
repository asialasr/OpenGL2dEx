target_include_directories(OpenGL2dEx PRIVATE 
    ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources_local(OpenGL2dEx PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/glad.c)