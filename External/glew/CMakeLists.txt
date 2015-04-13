cmake_minimum_required(VERSION 2.8)

add_library(glew src/glew.c)
set_target_properties(glew PROPERTIES INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include
                                      COMPILE_DEFINITIONS "GLEW_NO_GLU=1")

