
add_library(service_broker_core
        # ServiceBroker Architecture (New)
        Services/ServiceIdentity.h
        Services/ServiceIdentity.cpp
        Services/ServiceRegistry.h
        Services/ServiceRegistry.cpp
        Services/ServiceBroker.h
        Services/ServiceBroker.cpp
        Services/ServiceClient.h
        Services/ServiceClient.cpp
        Services/ServiceMonitor.h
        Services/ServiceMonitor.cpp
        # Utils
        Utils/Utils.cpp
        Utils/Utils.h
)

target_include_directories(service_broker_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/schemas
    ../third_party
    ../shared
)

target_link_libraries(service_broker_core PUBLIC
    inih_lib
    ValiJSON::valijson
    rdws_shared_validation
    rdws_shared_config
)

# --- Executables ---
add_executable(service_broker_app
    main.cpp
)
target_link_libraries(service_broker_app PRIVATE service_broker_core)

# ServiceBroker Examples
add_executable(service_broker_example
    service_broker_example.cpp
)
target_link_libraries(service_broker_example PRIVATE service_broker_core)

add_executable(service_broker_monitor
    service_broker_monitor.cpp
)
target_link_libraries(service_broker_monitor PRIVATE service_broker_core)

add_executable(example_service_client
    example_service_client.cpp
)
target_link_libraries(example_service_client PRIVATE service_broker_core)

# --- Unit tests ---
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

