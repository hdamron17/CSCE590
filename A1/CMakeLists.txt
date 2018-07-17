file(RELATIVE_PATH REL_SRC_DIR ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
if(NOT REL_SRC_DIR)
  set(REL_SRC_DIR .)
endif()

file(GLOB A1_IMAGES images/*.jpg images/*.png images/*.ppm images/*.tiff)
small_images(A1 "${A1_IMAGES}")

notangle(A1 Q1.cpp src/A1.nw)
add_executable(A1_Q1 ${A1_Q1_cpp})
target_link_libraries(A1_Q1 ${OpenCV_LIBS})
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/output/ppm_image_to_png.png
         ${CMAKE_CURRENT_SOURCE_DIR}/output/ppm_image_to_jpg.jpg
         ${CMAKE_CURRENT_SOURCE_DIR}/output/ppm_image_to_tiff.tiff

         ${CMAKE_CURRENT_SOURCE_DIR}/output/png_image_to_ppm.ppm
         ${CMAKE_CURRENT_SOURCE_DIR}/output/png_image_to_jpg.jpg
         ${CMAKE_CURRENT_SOURCE_DIR}/output/png_image_to_tiff.tiff

         ${CMAKE_CURRENT_SOURCE_DIR}/output/jpg_image_to_png.png
         ${CMAKE_CURRENT_SOURCE_DIR}/output/jpg_image_to_ppm.ppm
         ${CMAKE_CURRENT_SOURCE_DIR}/output/jpg_image_to_tiff.tiff

         ${CMAKE_CURRENT_SOURCE_DIR}/output/tiff_image_to_jpg.jpg
         ${CMAKE_CURRENT_SOURCE_DIR}/output/tiff_image_to_png.png
         ${CMAKE_CURRENT_SOURCE_DIR}/output/tiff_image_to_ppm.ppm
  DEPENDS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/A1_Q1
          ${CMAKE_CURRENT_SOURCE_DIR}/images/jpg_image.jpg
          ${CMAKE_CURRENT_SOURCE_DIR}/images/png_image.png
          ${CMAKE_CURRENT_SOURCE_DIR}/images/ppm_image.ppm
          ${CMAKE_CURRENT_SOURCE_DIR}/images/tiff_image.tiff
  COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/A1_Q1 ${REL_SRC_DIR} nodisplay
)

notangle(A1 Q2.cpp src/A1.nw)
add_executable(A1_Q2 ${A1_Q2_cpp})
target_link_libraries(A1_Q2 ${OpenCV_LIBS})
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/output/my_histogram.csv
         ${CMAKE_CURRENT_SOURCE_DIR}/output/cv_histogram.csv
         ${CMAKE_CURRENT_SOURCE_DIR}/output/r_histogram.csv
         ${CMAKE_CURRENT_SOURCE_DIR}/output/g_histogram.csv
         ${CMAKE_CURRENT_SOURCE_DIR}/output/b_histogram.csv
  DEPENDS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/A1_Q2
          ${CMAKE_CURRENT_SOURCE_DIR}/images/base_image.png
  COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/A1_Q2 ${REL_SRC_DIR} nodisplay
)

notangle(A1 Q4.cpp src/A1.nw)
add_executable(A1_Q4 ${A1_Q4_cpp})
target_link_libraries(A1_Q4 ${OpenCV_LIBS})
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/output/image_inverted.png
  DEPENDS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/A1_Q4
          ${CMAKE_CURRENT_SOURCE_DIR}/images/base_image.png
  COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/A1_Q4 ${REL_SRC_DIR} nodisplay
)

noweave(A1 src/A1.nw)
add_latex_document(${A1_A1_tex}
  IMAGE_DIRS images
  INPUTS
    # Q2
    output/my_histogram.csv
    output/cv_histogram.csv
    output/r_histogram.csv
    output/g_histogram.csv
    output/b_histogram.csv
  IMAGES
    # Q1
    output/ppm_image_to_jpg.jpg
    output/ppm_image_to_png.png
    output/ppm_image_to_tiff.tiff

    output/png_image_to_jpg.jpg
    output/png_image_to_ppm.ppm
    output/png_image_to_tiff.tiff

    output/jpg_image_to_png.png
    output/jpg_image_to_ppm.ppm
    output/jpg_image_to_tiff.tiff

    output/tiff_image_to_jpg.jpg
    output/tiff_image_to_png.png
    output/tiff_image_to_ppm.ppm

    # Q4
    output/image_inverted.png
)

pdf_compress(${LATEX_OUTPUT_PATH}/A1.pdf)

tar(A1
  BINARIES
  build/A1-compressed.pdf

  SOURCES
  ${CMAKE_TOP_LEVEL_FILES}
  A1/CMakeLists.txt
  A1/src
  A1/images
  A1/noweb
)

clean_noweb()
file(MAKE_DIRECTORY output)
