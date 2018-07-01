cmake_minimum_required (VERSION 2.8.11)
project (CSCE590)

include(UseLATEX.cmake)
include(UseNoweb.cmake)

set(LATEX_OUTPUT_PATH build)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

# set(DIRECTORIES A1 A2)
# foreach(DIR IN ITEMS ${DIRECTORIES})
#   message("DBG: ${DIR}")
# endforeach()

notangle(A1 src/test.nw main.cpp)
noweave(A1 src/test.nw)


add_executable(A1_main ${A1_main_cpp})
add_latex_document(${A1_test_tex})
