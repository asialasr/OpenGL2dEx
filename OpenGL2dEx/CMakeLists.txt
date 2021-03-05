add_subdirectory(audio)
add_subdirectory(fonts)
add_subdirectory(levels)
add_subdirectory(shaders)
add_subdirectory(stbi)
add_subdirectory(util)

target_sources_local(
    OpenGL2dEx
    PRIVATE
        main.cpp
)