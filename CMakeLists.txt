cmake_minimum_required (VERSION 3.1)
project (CSCE590)

include(UseLATEX.cmake)
include(UseNoweb.cmake)

find_package(OpenCV 3 REQUIRED)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/build ${CMAKE_CURRENT_BINARY_DIR}/bin)

set(LATEX_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/build)
set(NOWEB_OUTPUT_PATH noweb)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

list(APPEND CMAKE_TOP_LEVEL_FILES
  CMakeLists.txt
  UseLATEX.cmake
  UseNoweb.cmake
)

set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES bin build)

set (CMAKE_CXX_STANDARD 11)

# function(src_path file_path)
#   file(RELATIVE_PATH file_rel_path ${CMAKE_CURRENT_SOURCE_DIR} )
# endfunction()

function(add_subdirectories)
  foreach(sub ${ARGN})
    if(EXISTS ${CMAKE_SOURCE_DIR}/${sub}/CMakeLists.txt)
      add_subdirectory(${CMAKE_SOURCE_DIR}/${sub} EXCLUDE_FROM_ALL)
    endif()
  endforeach()
endfunction()

add_subdirectories(A1 A2)
