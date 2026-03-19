
add_library(loader_core
        Loader.cpp
        Services.cpp
        Services.h
        Service.h
        Utils.cpp
        Utils.h
)

target_include_directories(loader_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/schemas
    ../third_party
    ../shared
)

target_link_libraries(loader_core PUBLIC
    inih_lib
    ValiJSON::valijson
    rdws_shared_validation
)

# --- Executable ---
add_executable(loader_app
    main.cpp
)
target_link_libraries(loader_app PRIVATE loader_core)

# --- Copy configuration files ---
add_custom_command(TARGET loader_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/services.json
        $<TARGET_FILE_DIR:loader_app>/services.json
    COMMENT "Copying services.json to the binary directory"
)

# --- Unit tests ---
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

