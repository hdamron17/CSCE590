# Copyright 2018 Hunter Damron
# Provides simple cmake interface to noweb for literate programming

# find_program(NOWEB_CMD noweb)
# if(${NOWEB_CMD} STREQUAL "NOWEB_CMD-NOTFOUND")
#   message(FATAL_ERROR "Unable to find command noweb")
# endif()

find_program(NOWEAVE_CMD noweave)
if(${NOWEAVE_CMD} STREQUAL "NOWEAVE_CMD-NOTFOUND")
  message(FATAL_ERROR "Unable to find command noweave")
endif()

find_program(NOTANGLE_CMD notangle)
if(${NOTANGLE_CMD} STREQUAL "NOTANGLE_CMD-NOTFOUND")
  message(FATAL_ERROR "Unable to find command notangle")
endif()

find_program(CPIF_CMD cpif)
if(${CPIF_CMD} STREQUAL "CPIF_CMD-NOTFOUND")
  message(ERROR "Unable to find command cpif")
  set(NOWEB_WRITE > )
  # MAGICK_WRITE empty
else()
  set(NOWEB_WRITE | ${CPIF_CMD})
  set(MAGICK_WRITE - | ${CPIF_CMD})
endif()

function(get_noweb_output_path)
  if(NOT NOWEB_OUTPUT_PATH)
    message(FATAL_ERROR "Please set NOWEB_OUTPUT_PATH to use noweb")
  endif()
endfunction()

function(notangle prefix code_file noweb_file)
  get_noweb_output_path()
  file(MAKE_DIRECTORY "${NOWEB_OUTPUT_PATH}")

  get_filename_component(code_file_name ${code_file} NAME)
  string(REPLACE "." "_" code_file_name ${code_file_name})
  set(out_file ${NOWEB_OUTPUT_PATH}/${code_file})
  set(notangle_out_file ${out_file} PARENT_SCOPE)
  set(${prefix}_${code_file_name} ${out_file} PARENT_SCOPE)
  set(out_file_abs ${CMAKE_CURRENT_SOURCE_DIR}/${out_file})

  list(APPEND input_file ${CMAKE_CURRENT_SOURCE_DIR}/${noweb_file})
  foreach(alt_noweb ${ARGN})
    list(APPEND input_file ${CMAKE_CURRENT_SOURCE_DIR}/${alt_noweb})
  endforeach()

  add_custom_command(
    OUTPUT ${out_file_abs}
    DEPENDS ${input_file}
    VERBATIM COMMAND ${NOTANGLE_CMD} ${input_file} -R${code_file} ${NOWEB_WRITE} ${out_file_abs}
  )
  add_custom_target(${prefix}_${code_file_name} ALL DEPENDS ${out_file_abs})
endfunction()

function(noweave prefix noweb_file)
  get_noweb_output_path()
  file(MAKE_DIRECTORY "${NOWEB_OUTPUT_PATH}")

  get_filename_component(noweb_file_name ${noweb_file} NAME_WE)
  set(out_file ${NOWEB_OUTPUT_PATH}/${noweb_file_name}.tex)
  set(noweave_out_file ${out_file} PARENT_SCOPE)
  set(${prefix}_${noweb_file_name}_tex ${out_file} PARENT_SCOPE)
  set(out_file_abs ${CMAKE_CURRENT_SOURCE_DIR}/${out_file})

  list(APPEND input_file ${CMAKE_CURRENT_SOURCE_DIR}/${noweb_file})
  foreach(alt_noweb ${ARGN})
    list(APPEND input_file ${CMAKE_CURRENT_SOURCE_DIR}/${alt_noweb})
  endforeach()

  add_custom_command(
    OUTPUT ${out_file_abs}
    DEPENDS ${input_file}
    VERBATIM COMMAND ${NOWEAVE_CMD} -delay ${input_file} ${NOWEB_WRITE} ${out_file_abs}  # TODO(HD) add back '-n -index'
  )
  add_custom_target(${prefix}_${noweb_file_name}_tex ALL DEPENDS ${out_file_abs})
endfunction()

function(pdf_compress input_pdf)
  get_filename_component(PDF_DIR ${input_pdf} DIRECTORY)
  get_filename_component(PDF_FILENAME ${input_pdf} NAME_WE)
  get_filename_component(PDF_EXT ${input_pdf} EXT)
  set(INFILE ${PDF_DIR}/${PDF_FILENAME}${PDF_EXT})
  set(OUTFILE ${PDF_DIR}/${PDF_FILENAME}-compressed${PDF_EXT})
  add_custom_command(OUTPUT ${OUTFILE}
    DEPENDS ${INFILE}
    COMMAND gs -sDEVICE=pdfwrite -dCompatibilityLevel=1.4 -dNOPAUSE -dQUIET -dBATCH -sOutputFile=${OUTFILE} ${INFILE}
  )
  add_custom_target(${PDF_FILENAME}_compressed_pdf
    DEPENDS ${OUTFILE}
  )
endfunction()

find_program(MAGICK_CMD magick)
if(${MAGICK_CMD} STREQUAL "MAGICK_CMD-NOTFOUND")
  message("Unable to find command magick")
else()
  function(small_images target_prefix images)
    add_custom_target(${target_prefix}_shrink_images)
    foreach(image ${images})
      add_custom_command(
        TARGET ${target_prefix}_shrink_images
        COMMAND ${MAGICK_CMD} convert -resize ${IMAGE_SIZE}x${IMAGE_SIZE}\\> ${image} ${MAGICK_WRITE} ${image}
      )
    endforeach()
  endfunction()

  if(NOT IMAGE_SIZE)
    set(IMAGE_SIZE 800)
  endif()
endif()

macro(clean_noweb)
  get_filename_component(NOWEB_ABS ${NOWEB_OUTPUT_PATH} ABSOLUTE)
  set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
    ${NOWEB_ABS}
    ${CMAKE_CURRENT_SOURCE_DIR}/output
  )
endmacro()

# Output file will be ${tar_file}.tar.gz
function(tar tar_file)
  set(tar_bin tar_binary)

  set(multiValueArgs SOURCES BINARIES)
  cmake_parse_arguments(tar "" "" "${multiValueArgs}" ${ARGN})

  set(output_file ${CMAKE_BINARY_DIR}/${tar_file}.tar.gz)

  foreach(src_file ${tar_SOURCES})
    list(APPEND input_files ${src_file})
    list(APPEND input_files_abs ${CMAKE_SOURCE_DIR}/${src_file})
  endforeach()

  foreach(bin_file ${tar_BINARIES})
    set(bin_file_cp ${CMAKE_SOURCE_DIR}/${tar_bin}/${bin_file})
    set(bin_file_in ${CMAKE_BINARY_DIR}/${bin_file})
    add_custom_command(
      OUTPUT ${bin_file_cp}
      DEPENDS ${bin_file_in}
      COMMAND ${CMAKE_COMMAND} -E copy ${bin_file_in} ${bin_file_cp}
    )
    list(APPEND input_files ${tar_bin}/${bin_file})
    list(APPEND input_files_abs ${bin_file_cp})
  endforeach()

  add_custom_command(
    OUTPUT ${output_file}
    DEPENDS ${input_files_abs}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} -E tar cfz ${output_file} ${input_files}
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/${tar_bin}
  )
  add_custom_target(${tar_file}_compress
    DEPENDS ${output_file}
  )
  set_property(TARGET APPEND PROPERTY CLEAN_NO_CUSTOM false)
endfunction()
