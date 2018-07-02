cmake_minimum_required (VERSION 3.1)
project (CSCE590)

include(UseLATEX.cmake)
include(UseNoweb.cmake)

find_package(OpenCV 3 REQUIRED)

set(LATEX_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/build)
set(NOWEB_OUTPUT_PATH noweb)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_CURRENT_SOURCE_DIR}/noweb noweb bin build)

# notangle(A1 src/test.nw main.cpp)
# noweave(A1 src/test.nw)
# add_executable(A1_main ${A1_main_cpp})
# add_latex_document(${A1_test_tex})

notangle(A1 src/Q4.nw.cpp Q4.cpp)
noweave(A1 src/Q4.nw.cpp)
add_executable(A1_Q4 ${A1_Q4_cpp})
target_link_libraries(A1_Q4 ${OpenCV_LIBS})
add_latex_document(${A1_Q4_tex}
  IMAGE_DIRS A1/images
)

notangle(A1 src/Q2.nw.cpp Q2.cpp)
noweave(A1 src/Q2.nw.cpp)
add_executable(A1_Q2 ${A1_Q2_cpp})
file(RELATIVE_PATH A1_DIR ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/A1)
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/A1/outputs/my_histogram.csv
         ${CMAKE_CURRENT_SOURCE_DIR}/A1/outputs/cv_histogram.csv
  DEPENDS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/A1_Q2
  COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/A1_Q2 ${A1_DIR}
)
target_link_libraries(A1_Q2 ${OpenCV_LIBS})
add_latex_document(${A1_Q2_tex}
  IMAGE_DIRS A1/images
  INPUTS A1/outputs/my_histogram.csv
         A1/outputs/cv_histogram.csv
)
