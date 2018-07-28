get_src_rel_path(REL_SRC_DIR)

file(GLOB A2_IMAGES images/*.jpg images/*.png images/*.ppm images/*.tiff)
small_images(A2 "${A2_IMAGES}")

notangle(A2 Q1.cpp src/Q1.nw.cpp src/Common.nw.cpp)
add_executable(A2_Q1 ${A2_Q1_cpp})
target_link_libraries(A2_Q1 ${OpenCV_LIBS})

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/output/grassfire.png
         ${CMAKE_CURRENT_SOURCE_DIR}/output/skeleton.png
  DEPENDS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/A2_Q1
          ${CMAKE_CURRENT_SOURCE_DIR}/images/for_skeleton.png
  COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/A2_Q1 ${REL_SRC_DIR} nodisplay
)

noweave(A2 src/Q1.nw.cpp)

notangle(A2 Q2.cpp src/Q2.nw.cpp src/Common.nw.cpp)
add_executable(A2_Q2 ${A2_Q2_cpp})
target_link_libraries(A2_Q2 ${OpenCV_LIBS})

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/output/final_windmap.png
  DEPENDS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/A2_Q2
          ${CMAKE_CURRENT_SOURCE_DIR}/images/america.png
          ${CMAKE_CURRENT_SOURCE_DIR}/images/america_mask.png
          ${CMAKE_CURRENT_SOURCE_DIR}/images/windmap.jpg
  COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/A2_Q2 ${REL_SRC_DIR} nodisplay
)

noweave(A2 src/Q2.nw.cpp)

notangle(A2 Q4.cpp src/Q4.nw.cpp src/Common.nw.cpp)
add_executable(A2_Q4 ${A2_Q4_cpp})
target_link_libraries(A2_Q4 ${OpenCV_LIBS})

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/output/correlation.png
         ${CMAKE_CURRENT_SOURCE_DIR}/output/correlation_annotated.png
         ${CMAKE_CURRENT_SOURCE_DIR}/output/correlation_normed.png
         ${CMAKE_CURRENT_SOURCE_DIR}/output/correlation_normed_annotated.png
  DEPENDS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/A2_Q4
          ${CMAKE_CURRENT_SOURCE_DIR}/images/for_correlation_small.jpg
          ${CMAKE_CURRENT_SOURCE_DIR}/images/for_correlation_copy_template_small.jpg
  COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/A2_Q4 ${REL_SRC_DIR} nodisplay
)

noweave(A2 src/Q4.nw.cpp)

noweave(A2 src/Common.nw.cpp)
add_latex_document(src/A2.tex
  IMAGE_DIRS images
  INPUTS ${A2_Q1_tex}
         ${A2_Q2_tex}
         ${A2_Q4_tex}
         ${A2_Common_tex}
  IMAGES
    # Q1
    output/grassfire.png
    output/skeleton.png

    # Q2
    output/final_windmap.png

    # Q4
    output/correlation.png
    output/correlation_annotated.png
    output/correlation_normed.png
    output/correlation_normed_annotated.png
)

pdf_compress(${LATEX_OUTPUT_PATH}/A2.pdf)

clean_noweb()
file(MAKE_DIRECTORY output)

tar(A2
  BINARIES
  build/A2-compressed.pdf

  SOURCES
  ${CMAKE_TOP_LEVEL_FILES}

  A2/CMakeLists.txt
  A2/src
  A2/images

  A2/${A2_Q1_cpp}
  A2/${A2_Q2_cpp}
  A2/${A2_Q4_cpp}

  A2/${A2_Q1_tex}
  A2/${A2_Q2_tex}
  A2/${A2_Q4_tex}
  A2/${A2_Common_tex}
)

add_custom_target(run_A2_Q1
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/output/grassfire.png
)

add_custom_target(run_A2_Q2
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/output/final_windmap.png
)

add_custom_target(run_A2_Q4
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/output/correlation.png
)
