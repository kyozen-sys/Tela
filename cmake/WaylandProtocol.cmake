function(wayland_protocol target xml)
    get_filename_component(PROTOCOL_NAME ${xml} NAME_WE)

    set(PROTOCOL_DIR "${CMAKE_CURRENT_BINARY_DIR}/wayland")

    file(MAKE_DIRECTORY ${PROTOCOL_DIR})

    set(PROTOCOL_SOURCES "${PROTOCOL_DIR}/${PROTOCOL_NAME}-protocol.c")
    set(PROTOCOL_HEADERS "${PROTOCOL_DIR}/${PROTOCOL_NAME}-client-protocol.h")

    add_custom_command(OUTPUT ${PROTOCOL_SOURCES} ${PROTOCOL_HEADERS}
        COMMAND ${WAYLAND_SCANNER} private-code ${xml} ${PROTOCOL_SOURCES}
        COMMAND ${WAYLAND_SCANNER} client-header ${xml} ${PROTOCOL_HEADERS}
        DEPENDS ${xml}
        COMMENT "Generating Wayland protocol sources for ${PROTOCOL_NAME}"
        VERBATIM
    )

    target_sources(${target} PRIVATE ${PROTOCOL_SOURCES})

    target_include_directories(${target} PRIVATE ${PROTOCOL_DIR})
endfunction()