set(SHIJIMA_GENERATED_DIR "${CMAKE_BINARY_DIR}/generated")
set(SHIJIMA_RESOURCE_GENERATOR "${CMAKE_CURRENT_LIST_DIR}/generate_resources.py")

function(shijima_generate_resource output mode)
    add_custom_command(
        OUTPUT "${output}"
        COMMAND Python3::Interpreter "${SHIJIMA_RESOURCE_GENERATOR}" "${mode}" "${output}" ${ARGN}
        DEPENDS "${SHIJIMA_RESOURCE_GENERATOR}" ${ARGN}
        COMMENT "Generating ${mode} resource: ${output}"
        VERBATIM
    )
endfunction()
