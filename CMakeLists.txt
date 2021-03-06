cmake_minimum_required(VERSION 3.17)
function(copy_resource_to_build target src dest)
#    add_custom_command(
#        TARGET ${target} POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy
#                ${CMAKE_CURRENT_LIST_DIR}/${src}
#                ${CMAKE_CURRENT_BINARY_DIR}/${dest})
    configure_file(${src} ${dest} COPYONLY)
endfunction()

# Function from: https://crascit.com/2016/01/31/enhanced-source-file-handling-with-target_sources/
# Used for compatibility with older CMake versions
# NOTE: This helper function assumes no generator expressions are used
#       for the source files
function(target_sources_local target)
  if(POLICY CMP0076)
    # New behavior is available, so just forward to it by ensuring
    # that we have the policy set to request the new behavior, but
    # don't change the policy setting for the calling scope
    cmake_policy(PUSH)
    cmake_policy(SET CMP0076 NEW)
    target_sources(${target} ${ARGN})
    cmake_policy(POP)
    return()
  endif()

  # Must be using CMake 3.12 or earlier, so simulate the new behavior
  unset(_srcList)
  get_target_property(_targetSourceDir ${target} SOURCE_DIR)

  foreach(src ${ARGN})
    if(NOT src STREQUAL "PRIVATE" AND
       NOT src STREQUAL "PUBLIC" AND
       NOT src STREQUAL "INTERFACE" AND
       NOT IS_ABSOLUTE "${src}")
      # Relative path to source, prepend relative to where target was defined
      file(RELATIVE_PATH src "${_targetSourceDir}" "${CMAKE_CURRENT_LIST_DIR}/${src}")
    endif()
    list(APPEND _srcList ${src})
  endforeach()
  target_sources(${target} ${_srcList})
endfunction()

project(OpenGL2dEx)

add_executable(OpenGL2dEx)

add_subdirectory(OpenGL2dEx)
add_subdirectory(libs)

target_include_directories(OpenGL2dEx PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/OpenGL2dEx
    ${CMAKE_CURRENT_LIST_DIR}/libs)