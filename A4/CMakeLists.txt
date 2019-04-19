get_src_rel_path(REL_SRC_DIR)

file(GLOB A4_IMAGES images/*.jpg images/*.png images/*.ppm images/*.tiff)
small_images(A4 "${A4_IMAGES}")

notangle(A4 Q2.cpp src/Q2.nw.cpp src/Common.nw.cpp)
add_executable(A4_Q2 ${A4_Q2_cpp})
target_link_libraries(A4_Q2 ${OpenCV_LIBS})

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/output/hough_edges.png
         ${CMAKE_CURRENT_SOURCE_DIR}/output/hough.png
         ${CMAKE_CURRENT_SOURCE_DIR}/output/hough_annotated.png
  DEPENDS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/A4_Q2
          ${CMAKE_CURRENT_SOURCE_DIR}/images/for_hough.jpg  # TODO(HD) make sure this is correct
  COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/A4_Q2 ${REL_SRC_DIR} nodisplay
)

noweave(A4 src/Q2.nw.cpp)

noweave(A4 src/Common.nw.cpp)
add_latex_document(src/A4.tex
  IMAGE_DIRS images
  INPUTS
    src/Q1.tex
    ${A4_Q2_tex}
    ${A4_Common_tex}

  IMAGES

    # Q2
    output/hough_edges.png
    output/hough.png
    output/hough_annotated.png
)

pdf_compress(${LATEX_OUTPUT_PATH}/A4.pdf)

clean_noweb()
file(MAKE_DIRECTORY output)

tar(A4
  BINARIES
  build/A4-compressed.pdf

  SOURCES
  ${CMAKE_TOP_LEVEL_FILES}

  A4/CMakeLists.txt
  A4/src
  A4/images

  A4/${A4_Q2_cpp}

  A4/${A4_Q2_tex}
  A4/${A4_Common_tex}
)

add_custom_target(run_A4_Q2
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/output/hough.png
)
