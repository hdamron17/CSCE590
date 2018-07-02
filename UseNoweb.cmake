# Copyright 2018 Hunter Damron
# Provides simple cmake interface to noweb for literate programming

# find_program(NOWEB_CMD noweb)
# if(NOWEB_CMD STREQUAL "NOWEB_CMD-NOTFOUND")
#   message(FATAL_ERROR "Unable to find command noweb")
# endif()

find_program(NOWEAVE_CMD noweave)
if(NOWEAVE_CMD STREQUAL "NOWEAVE_CMD-NOTFOUND")
  message(FATAL_ERROR "Unable to find command noweave")
endif()

find_program(NOTANGLE_CMD notangle)
if(NOTANGLE_CMD STREQUAL "NOTANGLE_CMD-NOTFOUND")
  message(FATAL_ERROR "Unable to find command notangle")
endif()

function(get_noweb_output_path)
  if(NOT NOWEB_OUTPUT_PATH)
    message(FATAL_ERROR "Please set NOWEB_OUTPUT_PATH to use noweb")
  endif()
endfunction()

function(notangle prefix noweb_file code_file)
  get_noweb_output_path()
  file(MAKE_DIRECTORY "${NOWEB_OUTPUT_PATH}/${prefix}")

  get_filename_component(code_file_name ${code_file} NAME)
  string(REPLACE "." "_" code_file_name ${code_file_name})
  set(out_file ${NOWEB_OUTPUT_PATH}/${prefix}/${code_file})
  set(notangle_out_file ${out_file} PARENT_SCOPE)
  set(${prefix}_${code_file_name} ${out_file} PARENT_SCOPE)
  set(out_file_abs ${CMAKE_CURRENT_SOURCE_DIR}/${out_file})

  set(input_file ${CMAKE_CURRENT_SOURCE_DIR}/${prefix}/${noweb_file})

  add_custom_command(
    PRE_BUILD
    OUTPUT ${out_file_abs}
    DEPENDS ${input_file}
    COMMAND ${NOTANGLE_CMD} ${input_file} -R${code_file} > ${out_file_abs}
  )
  add_custom_target(${prefix}_${code_file_name} ALL DEPENDS ${out_file_abs})
endfunction()

function(noweave prefix noweb_file)
  get_noweb_output_path()
  file(MAKE_DIRECTORY "${NOWEB_OUTPUT_PATH}/${prefix}")

  get_filename_component(noweb_file_name ${noweb_file} NAME_WE)
  set(out_file ${NOWEB_OUTPUT_PATH}/${prefix}/${noweb_file_name}.tex)
  set(noweave_out_file ${out_file} PARENT_SCOPE)
  set(${prefix}_${noweb_file_name}_tex ${out_file} PARENT_SCOPE)
  set(out_file_abs ${CMAKE_CURRENT_SOURCE_DIR}/${out_file})

  set(input_file ${CMAKE_CURRENT_SOURCE_DIR}/${prefix}/${noweb_file})

  add_custom_command(
    PRE_BUILD
    OUTPUT ${out_file_abs}
    DEPENDS ${input_file}
    COMMAND ${NOWEAVE_CMD} -index -autodefs c -delay ${input_file} > ${out_file_abs}
  )
  add_custom_target(${prefix}_${noweb_file_name}_tex ALL DEPENDS ${out_file_abs})
endfunction()

function(noweave_pdf prefix noweb_file)
  noweave(${prefix} ${noweb_file})
  message("~DBG: ${noweave_out_file}")

endfunction()
