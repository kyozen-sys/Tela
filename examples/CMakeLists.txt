file(GLOB EXAMPLE_DIRS CONFIGURE_DEPENDS *)

foreach(EXAMPLE_DIR ${EXAMPLE_DIRS})
    if (NOT IS_DIRECTORY ${EXAMPLE_DIR})
        continue()
    endif()

    get_filename_component(EXAMPLE_NAME ${EXAMPLE_DIR} NAME)

    file(GLOB_RECURSE EXAMPLE_SOURCES CONFIGURE_DEPENDS ${EXAMPLE_DIR}/*.cpp)

    if (NOT EXAMPLE_SOURCES)
        continue()
    endif()

    add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCES})

    set_target_properties(${EXAMPLE_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY
            "${CMAKE_BINARY_DIR}/examples/${EXAMPLE_NAME}"
    )

    foreach(RESOURCE_DIR assets)
        if(NOT EXISTS "${EXAMPLE_DIR}/${RESOURCE_DIR}")
            continue()
        endif()

        add_custom_command(
            TARGET
                ${EXAMPLE_NAME} POST_BUILD
            COMMAND
                ${CMAKE_COMMAND} -E copy_directory "${EXAMPLE_DIR}/${RESOURCE_DIR}" "$<TARGET_FILE_DIR:${EXAMPLE_NAME}>/${RESOURCE_DIR}"
        )
    endforeach()
    
    target_link_libraries(${EXAMPLE_NAME} PRIVATE ${PROJECT_NAME})
endforeach()