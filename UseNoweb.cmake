# Copyright 2018 Hunter Damron
# Provides simple cmake interface to noweb for literate programming

find_program(NOWEB_CMD noweb)
if(NOWEB_CMD STREQUAL "noweb-NOTFOUND")
  message(FATAL_ERROR "Unable to find command noweb")
endif()

find_program(NOWEAVE_CMD noweave)
if(NOWEAVE_CMD STREQUAL "noweave-NOTFOUND")
  message(FATAL_ERROR "Unable to find command noweave")
endif()

find_program(NOTANGLE_CMD notangle)
if(NOTANGLE_CMD STREQUAL "notangle-NOTFOUND")
  message(FATAL_ERROR "Unable to find command notangle")
endif()

set(NOWEB_DIR noweb)

function(notangle prefix noweb_file code_file)
  file(MAKE_DIRECTORY "${NOWEB_DIR}/${prefix}")

  get_filename_component(code_file_name ${code_file} NAME)
  string(REPLACE "." "_" code_file_name ${code_file_name})
  set(out_file ${NOWEB_DIR}/${prefix}/${code_file})
  set(notangle_out_file ${out_file} PARENT_SCOPE)
  set(${prefix}_${code_file_name} ${out_file} PARENT_SCOPE)

  add_custom_command(
    PRE_BUILD
    OUTPUT ${out_file}
    DEPENDS ${prefix}/${noweb_file}
    COMMAND ${NOTANGLE_CMD} ${prefix}/${noweb_file} -R${code_file} > ${out_file}
  )
  add_custom_target(${prefix}_${code_file_name} ALL DEPENDS ${out_file})
endfunction()

function(noweave prefix noweb_file)
  file(MAKE_DIRECTORY "${NOWEB_DIR}/${prefix}")

  get_filename_component(noweb_file_name ${noweb_file} NAME_WE)
  set(out_file ${NOWEB_DIR}/${prefix}/${noweb_file_name}.tex)
  set(noweave_out_file ${out_file} PARENT_SCOPE)
  set(${prefix}_${noweb_file_name}_tex ${out_file} PARENT_SCOPE)

  message("Dependency: ${prefix}/${noweb_file} -> ${out_file}")

  add_custom_command(
    PRE_BUILD
    OUTPUT ${out_file}
    DEPENDS ${prefix}/${noweb_file}
    COMMAND ${NOWEAVE_CMD} -index -autodefs c -delay ${prefix}/${noweb_file} > ${out_file}
  )
  add_custom_target(${prefix}_${noweb_file_name}_tex ALL DEPENDS ${out_file})
endfunction()

function(noweave_pdf prefix noweb_file)
  noweave(${prefix} ${noweb_file})
  message("~DBG: ${noweave_out_file}")
  
endfunction()
