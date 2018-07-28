get_src_rel_path(REL_SRC_DIR)

file(GLOB A3_IMAGES images/*.jpg images/*.png images/*.ppm images/*.tiff)
small_images(A3 "${A3_IMAGES}")

notangle(A3 Q1.cpp src/Q1.nw.cpp src/Common.nw.cpp)
add_executable(A3_Q1 ${A3_Q1_cpp})
target_link_libraries(A3_Q1 ${OpenCV_LIBS})

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/output/conv_avg_1.png
         ${CMAKE_CURRENT_SOURCE_DIR}/output/conv_avg_2.png
         ${CMAKE_CURRENT_SOURCE_DIR}/output/conv_gauss_1.png
         ${CMAKE_CURRENT_SOURCE_DIR}/output/conv_gauss_2.png
         ${CMAKE_CURRENT_SOURCE_DIR}/output/conv_vedge_1.png
         ${CMAKE_CURRENT_SOURCE_DIR}/output/conv_vedge_2.png
         ${CMAKE_CURRENT_SOURCE_DIR}/output/conv_hedge_1.png
         ${CMAKE_CURRENT_SOURCE_DIR}/output/conv_hedge_2.png
         ${CMAKE_CURRENT_SOURCE_DIR}/output/conv_sharp_1.png
         ${CMAKE_CURRENT_SOURCE_DIR}/output/conv_sharp_2.png
         ${CMAKE_CURRENT_SOURCE_DIR}/output/conv_custom_1.png
         ${CMAKE_CURRENT_SOURCE_DIR}/output/conv_custom_2.png
  DEPENDS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/A3_Q1
          ${CMAKE_CURRENT_SOURCE_DIR}/images/for_conv_1.png
          ${CMAKE_CURRENT_SOURCE_DIR}/images/for_conv_2.png
  COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/A3_Q1 ${REL_SRC_DIR} nodisplay
)

noweave(A3 src/Q1.nw.cpp)

notangle(A3 Q2.cpp src/Q2.nw.cpp src/Common.nw.cpp)
add_executable(A3_Q2 ${A3_Q2_cpp})
target_link_libraries(A3_Q2 ${OpenCV_LIBS})

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/output/thresh_1.png
         ${CMAKE_CURRENT_SOURCE_DIR}/output/thresh_2.png
         ${CMAKE_CURRENT_SOURCE_DIR}/output/thresh_3.png
         ${CMAKE_CURRENT_SOURCE_DIR}/output/thresholds.csv
  DEPENDS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/A3_Q2
          ${CMAKE_CURRENT_SOURCE_DIR}/images/for_thresh_1.png
          ${CMAKE_CURRENT_SOURCE_DIR}/images/for_thresh_2.png
  COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/A3_Q2 ${REL_SRC_DIR} nodisplay
)

noweave(A3 src/Q2.nw.cpp)

noweave(A3 src/Common.nw.cpp)
add_latex_document(src/A3.tex
  IMAGE_DIRS images
  INPUTS
    ${A3_Q1_tex}
    ${A3_Q2_tex}
    ${A3_Common_tex}

    # Q2
    output/thresholds.csv
  IMAGES
    # Q1
    output/conv_avg_1.png
    output/conv_avg_2.png
    output/conv_gauss_1.png
    output/conv_gauss_2.png
    output/conv_vedge_1.png
    output/conv_vedge_2.png
    output/conv_hedge_1.png
    output/conv_hedge_2.png
    output/conv_sharp_1.png
    output/conv_sharp_2.png
    output/conv_custom_1.png
    output/conv_custom_2.png

    # Q2
    output/thresh_1.png
    output/thresh_2.png
    output/thresh_3.png
)

pdf_compress(${LATEX_OUTPUT_PATH}/A3.pdf)

clean_noweb()
file(MAKE_DIRECTORY output)

tar(A3
  BINARIES
  build/A3-compressed.pdf

  SOURCES
  ${CMAKE_TOP_LEVEL_FILES}

  A3/CMakeLists.txt
  A3/src
  A3/images

  A3/${A3_Q1_cpp}
  A3/${A3_Q2_cpp}

  A3/${A3_Q1_tex}
  A3/${A3_Q2_tex}
  A3/${A3_Common_tex}
)

add_custom_target(run_A3_Q1
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/output/conv_avg_1.png
)

add_custom_target(run_A3_Q2
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/output/thresh_1.png
)
