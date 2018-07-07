cmake_minimum_required (VERSION 3.1)
project (CSCE590)

include(UseLATEX.cmake)
include(UseNoweb.cmake)

find_package(OpenCV 3 REQUIRED)
find_package(ImageMagick COMPONENTS Magick++ REQUIRED)

set(LATEX_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/build)
set(NOWEB_OUTPUT_PATH noweb)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_CURRENT_SOURCE_DIR}/noweb noweb bin build)

file(RELATIVE_PATH REL_SRC_DIR ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

notangle(A1 src/A1.nw Q1.cpp)
add_executable(A1_Q1 ${A1_Q1_cpp})
target_link_libraries(A1_Q1 ${OpenCV_LIBS})
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/A1/output/ppm_image_to_png.png
         ${CMAKE_CURRENT_SOURCE_DIR}/A1/output/ppm_image_to_jpg.jpg
         ${CMAKE_CURRENT_SOURCE_DIR}/A1/output/ppm_image_to_tiff.tiff

         ${CMAKE_CURRENT_SOURCE_DIR}/A1/output/png_image_to_ppm.ppm
         ${CMAKE_CURRENT_SOURCE_DIR}/A1/output/png_image_to_jpg.jpg
         ${CMAKE_CURRENT_SOURCE_DIR}/A1/output/png_image_to_tiff.tiff

         ${CMAKE_CURRENT_SOURCE_DIR}/A1/output/jpg_image_to_png.png
         ${CMAKE_CURRENT_SOURCE_DIR}/A1/output/jpg_image_to_ppm.ppm
         ${CMAKE_CURRENT_SOURCE_DIR}/A1/output/jpg_image_to_tiff.tiff

         ${CMAKE_CURRENT_SOURCE_DIR}/A1/output/tiff_image_to_jpg.jpg
         ${CMAKE_CURRENT_SOURCE_DIR}/A1/output/tiff_image_to_png.png
         ${CMAKE_CURRENT_SOURCE_DIR}/A1/output/tiff_image_to_ppm.ppm
  DEPENDS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/A1_Q1
          ${CMAKE_CURRENT_SOURCE_DIR}/A1/images/jpg_image.jpg
          ${CMAKE_CURRENT_SOURCE_DIR}/A1/images/png_image.png
          ${CMAKE_CURRENT_SOURCE_DIR}/A1/images/ppm_image.ppm
          ${CMAKE_CURRENT_SOURCE_DIR}/A1/images/tiff_image.tiff
  COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/A1_Q1 ${REL_SRC_DIR}/A1 nodisplay
)

notangle(A1 src/A1.nw Q2.cpp)
add_executable(A1_Q2 ${A1_Q2_cpp})
target_link_libraries(A1_Q2 ${OpenCV_LIBS})
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/A1/output/my_histogram.csv
         ${CMAKE_CURRENT_SOURCE_DIR}/A1/output/cv_histogram.csv
         ${CMAKE_CURRENT_SOURCE_DIR}/A1/output/r_histogram.csv
         ${CMAKE_CURRENT_SOURCE_DIR}/A1/output/g_histogram.csv
         ${CMAKE_CURRENT_SOURCE_DIR}/A1/output/b_histogram.csv
  DEPENDS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/A1_Q2
          ${CMAKE_CURRENT_SOURCE_DIR}/A1/images/base_image.png
  COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/A1_Q2 ${REL_SRC_DIR}/A1 nodisplay
)

notangle(A1 src/A1.nw Q4.cpp)
add_executable(A1_Q4 ${A1_Q4_cpp})
target_link_libraries(A1_Q4 ${OpenCV_LIBS})
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/A1/output/image_inverted.png
  DEPENDS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/A1_Q4
          ${CMAKE_CURRENT_SOURCE_DIR}/A1/images/base_image.png
  COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/A1_Q4 ${REL_SRC_DIR}/A1 nodisplay
)

noweave(A1 src/A1.nw)
add_latex_document(${A1_A1_tex}
  IMAGE_DIRS A1/images
  INPUTS
    # Q2
    A1/output/my_histogram.csv
    A1/output/cv_histogram.csv
    A1/output/r_histogram.csv
    A1/output/g_histogram.csv
    A1/output/b_histogram.csv
  IMAGES
    # Q1
    A1/output/ppm_image_to_jpg.jpg
    A1/output/ppm_image_to_png.png
    A1/output/ppm_image_to_tiff.tiff

    A1/output/png_image_to_jpg.jpg
    A1/output/png_image_to_ppm.ppm
    A1/output/png_image_to_tiff.tiff

    A1/output/jpg_image_to_png.png
    A1/output/jpg_image_to_ppm.ppm
    A1/output/jpg_image_to_tiff.tiff

    A1/output/tiff_image_to_jpg.jpg
    A1/output/tiff_image_to_png.png
    A1/output/tiff_image_to_ppm.ppm

    # Q4
    A1/output/image_inverted.png
)
