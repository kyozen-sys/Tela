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

    target_link_libraries(${EXAMPLE_NAME} PRIVATE ${PROJECT_NAME})
endforeach()