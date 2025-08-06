function(tiktoken_embed_resource RSRC_FILE RESOURCE_NAME RETVAL_VAR)
    set(MSVC_STRING_LITERAL_LEN_LIMIT 16380)

    set(OUT_CPP_FILE "${CMAKE_BINARY_DIR}/resources/resource_${RESOURCE_NAME}.cpp")
    add_custom_command(OUTPUT ${OUT_CPP_FILE}
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/tools/file2cpp.py" "${RSRC_FILE}" "${OUT_CPP_FILE}" resource_${RESOURCE_NAME}
        COMMENT "Embedding resource ${RSRC_FILE}..."
    )
    set(${RETVAL_VAR} "${OUT_CPP_FILE}" PARENT_SCOPE) # retval
endfunction()