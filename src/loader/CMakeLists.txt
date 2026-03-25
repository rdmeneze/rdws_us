
add_library(loader_core
        Loader.cpp
        Services/Services.cpp
        Services/Services.h
        Services/Service.h
        Utils/Utils.cpp
        Utils/Utils.h
        Services/ServiceIPCConfig.h
        Services/ServiceInstance.h
        Services/ServiceInstance.cpp
        Services/ServiceManager.h
        Services/ServiceManager.cpp
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
    rdws_shared_config
)

# --- Executables ---
add_executable(loader_app
    main.cpp
)
target_link_libraries(loader_app PRIVATE loader_core)

# Service Manager Test
add_executable(service_manager_test
    service_manager_test.cpp
)
target_link_libraries(service_manager_test PRIVATE loader_core)

# --- Copy configuration files ---
add_custom_command(TARGET loader_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/services.json
        $<TARGET_FILE_DIR:loader_app>/services.json
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/example_services.json
        $<TARGET_FILE_DIR:loader_app>/example_services.json
    COMMENT "Copying configuration files"
    #--  COMMENT "Copying services.json to the binary directory"
)

# --- Unit tests ---
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

